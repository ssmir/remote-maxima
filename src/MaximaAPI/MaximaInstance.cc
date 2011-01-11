/* Simple C++ API for Maxima

Copyright (c) 2007, Sergey Smirnov <sasmir (at) gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MaximaInstance.h"

#include <fstream>
#include <signal.h>
#include <sstream>
#include "boost/algorithm/string.hpp"

namespace alg = boost::algorithm;

using namespace MaximaAPI;

enum Input
{
    IN_TERMINATOR,
    IN_BACKSLASH,
    IN_QUOTEMARK,
    IN_OTHER
};

const int STATE_END = -1;

MaximaInstance::MaximaIOHook::~MaximaIOHook()
{
}

MaximaInstance::MaximaInstance(const std::string &maximaPath,
    const std::string &workingDir, const std::string &utilsDir)
:   _workingDirectory(fs::system_complete(workingDir)),
    _utilsDir(fs::system_complete(utilsDir)),
    _lastPromptId(1),
	_pid(0)
{
	std::vector<std::string> args;
	args.push_back(maximaPath);
	args.push_back("-q");
	args.push_back("-p");
	args.push_back((_utilsDir / "maximag-disp.lisp").external_file_string());

	bp::context ctx;
	ctx.stdin_behavior = bp::capture_stream();
	ctx.stdout_behavior = bp::capture_stream();
	ctx.stderr_behavior = bp::redirect_stream_to_stdout();
	ctx.work_directory = _workingDirectory.external_directory_string();
    ctx.environment = bp::self::get_environment();

	//_log.info("Launching Maxima from %s", maximaPath.c_str());
    process.reset(new bp::child(bp::launch(maximaPath, args, ctx)));
	readPid();
	//_log.info("Maxima launched, pid=%d", process->get_id());
	Reply(process->get_stdout());
}

MaximaInstance::~MaximaInstance() {
    //sendCommand("quit()");
    process->get_stdin().close();
    process->wait();
}

void MaximaInstance::readPid()
{
#ifdef _WIN32
	std::string pidStr;
	std::getline(process->get_stdout(), pidStr);
	
	boost::regex pidRegex("(pid=)?(\\d+)");
	boost::match_results<std::string::iterator> pidRegexMatch;
    if (boost::regex_match(pidStr.begin(), pidStr.end(), pidRegexMatch,
        pidRegex))
    {
        std::istringstream pidStream(std::string(pidRegexMatch[2].first,
            pidRegexMatch[2].second));
        pidStream >> _pid;
		return;
    }
#endif
	_pid = process->get_id();
}

size_t MaximaInstance::readsome2(std::istream &in, Reply::RawReply &reply) {
	const size_t bufSize = 4096;
	char buf[bufSize];
	
	size_t readTotal = 0;
	size_t charsRead;
	
	do {
		charsRead = in.readsome(buf, bufSize);
		reply.insert(reply.end(), buf, buf + charsRead);
		readTotal += charsRead;
	} while (charsRead != 0);
	
	return readTotal;
}

void MaximaInstance::sendCommand(std::string command) {
	alg::trim(command);
    
    int state = 0;
    for (size_t i = 0; i < command.size(); ++i)
    {
        state = checkInputExpression(command[i], state);
        if (state == STATE_END && (i + 1) != command.size())
        {
            throw std::runtime_error("Bad expression: "
                "only one ;|$ terminated expression at a time is allowed");
        }
    }
    if (state != STATE_END)
    {
        command += ";";
    }
    
    if (!maximaIOHookRegex.empty())
    {
        boost::match_results<std::string::iterator> m;
        std::string::iterator start = command.begin();
        std::string::iterator end = command.end();
        while (boost::regex_search(start, end, m, maximaIOHookRegex))
        {
            std::string rep = maximaIOHook->handle(
                std::string(m[1].first, m[1].second),
                std::string(m[2].first, m[2].second));
            
            size_t newStartPosition = m[0].second - start + rep.size() - 
                (m[2].second - m[2].first);
            
            command.replace(m[2].first, m[2].second, rep);
            //command.insert(command.erase(m[2].first, m[2].second),
            //    rep.begin(), rep.end());
            
            start = command.begin() + newStartPosition;
            end = command.end();
        }
    }
	
	process->get_stdin() << command << std::endl;
}

MaximaInstance::Reply::Reply(std::istream &in) {
	boost::regex promptExpr("prompt;>>(.*)<<prompt;");
	
	boost::match_results<Reply::It> promptMatch;
	
	do {
        char c;
        if (!in.get(c))
        {
            std::string wasRead(reply.begin(), reply.end());
            throw std::runtime_error("Read failed. Was read: " + wasRead);
        }
        reply.push_back(c);
    } while (!(reply.back() == ';' && boost::regex_search(
        reply.begin(), reply.end(), promptMatch, promptExpr)));
    
	prompt = promptMatch[1];

	boost::regex outExpr("out;>>(.*?)<<out;");
	Reply::It start = reply.begin();
	Reply::It end = promptMatch[0].first;
	
	boost::match_results<Reply::It> what;
	while(boost::regex_search(start, end, what, outExpr)) {
		outs.push_back(what[1]);
		betweens.push_back(Reply::Range(start, what[0].first));
		start = what[0].second;
	}
	betweens.push_back(Reply::Range(start, end));
	
	boost::match_results<Reply::It> outInPrompt;
	if (boost::regex_search(promptMatch[1].first, promptMatch[1].second,
		outInPrompt, outExpr))
	{
		prompt = outInPrompt[1];
	}
    
    boost::regex goodPrompt("\\s*\\(%i(\\d+)\\)\\s*");
    boost::match_results<Reply::It> goodPromptMatch;
    if (boost::regex_match(prompt.first, prompt.second, goodPromptMatch,
        goodPrompt))
    {
        _goodPrompt = true;
        std::istringstream idStream(std::string(goodPromptMatch[1].first,
            goodPromptMatch[1].second));
        idStream >> _promptId;
    }
    else
    {
        _goodPrompt = false;
        _promptId = 0;
    }
}

MaximaInstance::ReplyPtr MaximaInstance::executeCommandRaw(
    const std::string &command)
{
	sendCommand(command);
	return readReply();
}

MaximaInstance::ReplyPtr MaximaInstance::readReply()
{
	size_t lastPromptId = _lastPromptId;
    
    ReplyPtr result;
    do
    {
        result = ReplyPtr(new Reply(process->get_stdout()));
    }
    while (result->outs.empty() && result->isGoodPrompt() &&
        result->getPromptId() <= lastPromptId && result->isInterrupted());
    
    _lastPromptId = result->getPromptId();
    return result;
}

std::string MaximaInstance::executeCommand(const std::string &command)
{
    //sendCommand(command);
    
	/*size_t lastPromptId = _lastPromptId;
    ReplyPtr reply = executeCommandRaw(command);
    while (reply->outs.empty() && reply->isGoodPrompt() &&
        reply->getPromptId() <= lastPromptId && reply->isInterrupted())
    {
        reply = ReplyPtr(new Reply(process->get_stdout()));
    }
    _lastPromptId = reply->getPromptId();*/
    
    ReplyPtr reply = executeCommandRaw(command);
    
	if (reply->outs.empty()) {
        if (!reply->isGoodPrompt())
        {
            interruptMaxima();
            executeCommandRaw(";");
            throw std::runtime_error("Unsupported: " + reply->concatenateParts());
        }
        if (reply->isInterrupted())
        {
            throw std::runtime_error("Command execution was interrupted.");
        }
        
		throw std::runtime_error("Maxima error: " + std::string(
			reply->betweens.back().first, reply->betweens.back().second));
	}
	
	boost::regex goodOut("\\s*\\(%o\\d+\\)\\s*(.*)\\s*");
	
	Reply::Range lastOut = reply->outs.back();
	
	boost::match_results<Reply::It> what;
	if (boost::regex_match(lastOut.first, lastOut.second, what, goodOut)) {
		return alg::trim_copy(std::string(what[1].first, what[1].second));
	}
	
	executeCommandRaw(";");
	throw std::runtime_error("Unsupported: " + reply->concatenateParts());
}

bool MaximaInstance::executeAndInterrupt(const std::string &command,
    size_t delay)
{
    sendCommand(command);
#ifndef _WIN32
    sleep(delay);
#else
	Sleep(delay * 1000);
#endif
    interruptMaxima();
    ReplyPtr reply = readReply();
    return reply->isInterrupted();
}

std::string MaximaInstance::Reply::concatenateParts() {
	std::string result;
	for (size_t i = 0; i < outs.size(); ++i) {
		result.insert(result.end(), betweens[i].first, betweens[i].second);
		result.insert(result.end(), outs[i].first, outs[i].second);
	}
	result.insert(result.end(), betweens.back().first, betweens.back().second);
	result.insert(result.end(), prompt.first, prompt.second);
	return result;
}

bool MaximaInstance::Reply::isInterrupted() const
{
    // for GCL, CLISP, SBCL, respectively
    boost::regex interrupt("Console interrupt|User break|Interactive interrupt");
    boost::match_results<Reply::It> match;
    return boost::regex_search(betweens.back().first, betweens.back().second,
        match, interrupt);
}

std::string MaximaInstance::executeInteractive(const std::string &command) {
	return executeCommandRaw(command)->concatenateParts();
}

void MaximaInstance::interruptMaxima()
{
	doInterrupt(getId());
}

void MaximaInstance::doInterrupt(int pid)
{
#ifndef _WIN32
    if (-1 == ::kill(pid, SIGINT))
    {
        std::ostringstream err;
        err << "Failed to send SIGINT to " << pid << "; errno: " << errno;
        throw std::runtime_error(err.str());
    }
#else
    fs::path winkill = _utilsDir / "winkill.exe";
    
    std::vector<std::string> args;
	args.push_back(winkill.external_file_string());
	args.push_back("-INT");
    
    std::ostringstream str;
    str << pid;
	args.push_back(str.str());

	bp::context ctx;
	ctx.m_stdin_behavior = bp::silence_stream();
	ctx.m_stdout_behavior = bp::inherit_stream();
	ctx.m_stderr_behavior = bp::redirect_stream_to_stdout();
	ctx.m_work_directory = _workingDirectory.external_directory_string();

    bp::child winkillProcess(bp::launch(winkill.external_file_string(),
        args, ctx));
    winkillProcess.wait();
#endif
}

bp::process::id_type MaximaInstance::getId() const {
	return _pid;
}

void MaximaInstance::setMaximaIOHook(const std::string &hookRegex,
    MaximaIOHook *hook)
{
    maximaIOHook = hook;
    maximaIOHookRegex = boost::regex(hookRegex);
}

int MaximaInstance::checkInputExpression(char c, int state) const
{
    /*
    Algorithm (states and transitions; pencil them on paper to understand):
    0: backslash->1, terminator->end, quot_mark->2, other->0
    1: any->0
    2: backslash->3, terminator->2, quot_mark->0, other->2
    3: any->2
    end:
    */
    Input in = IN_OTHER;
    if (c == ';' || c == '$')
    {
        in = IN_TERMINATOR;
    }
    else if (c == '\\')
    {
        in = IN_BACKSLASH;
    }
    else if (c == '"')
    {
        in = IN_QUOTEMARK;
    }
    
    switch (state)
    {
    case 0:
        switch (in)
        {
        case IN_BACKSLASH:
            return 1;
        case IN_TERMINATOR:
            return STATE_END;
        case IN_QUOTEMARK:
            return 2;
        default:
            return 0;
        }
    case 1:
        return 0;
    case 2:
        switch (in)
        {
        case IN_BACKSLASH:
            return 3;
        case IN_TERMINATOR:
            return 2;
        case IN_QUOTEMARK:
            return 0;
        default:
            return 2;
        }
    case 3:
        return 2;
    }
    return state;
}

const fs::path &MaximaInstance::getWorkingDirectory() const
{
    return _workingDirectory;
}

