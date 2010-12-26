/*  remote-maxima - a CAS Maxima C++ API and a Grid-service
	Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "MaximaResourceI.h"
#include <MaximaAPI/MaximaInstance.h>

using namespace IARnet::Maxima;

MaximaResourceI::MaximaResourceI(const std::string &maximaPath,
	const std::string &workingDirectory,
    const IARnet::Node::ServiceContext &context)
:   _maximaPath(maximaPath),
	_workingDirectory(workingDirectory),
    _context(context),
	_fs(_workingDirectory)
{
    _log = _context.getCommunicator()->getLogger();
	launchMaxima();
}

MaximaResourceI::~MaximaResourceI()
{
	_maxima.reset();
}

std::string MaximaResourceI::executeCommand(const ::std::string &command,
	const ::Ice::Current&)
{
    IceUtil::Mutex::Lock lock(*this);
    
	try {
		return _maxima->executeCommand(command);
	} catch (const std::exception &e) {
		throw MaximaException(e.what());
	}
}

std::string MaximaResourceI::executeInteractive(const ::std::string &command,
	const ::Ice::Current &)
{
    IceUtil::Mutex::Lock lock(*this);
    
	try {
		return _maxima->executeInteractive(command);
	} catch (const std::exception &e) {
		throw MaximaException(e.what());
	}
}

void MaximaResourceI::interrupt(const Ice::Current &)
{
    //_log->trace("MaximaResourceI", "Interrupting maxima...");
	try
    {
        _maxima->interruptMaxima();
	}
    catch (const std::exception &e)
    {
		throw MaximaException(e.what());
	}
}

void MaximaResourceI::restart(const Ice::Current &)
{
    IceUtil::Mutex::Lock lock(*this);
	_maxima.reset();
	launchMaxima();
}

void MaximaResourceI::launchMaxima()
{
	try
    {
		_maxima.reset(new MaximaAPI::MaximaInstance(_maximaPath, _workingDirectory));
	}
    catch (const std::exception &e)
    {
		throw MaximaException(e.what());
	}
}

void MaximaResourceI::destroy(const ::Ice::Current &)
{
    _context.removeServant(_workingDirectory);
}

void MaximaResourceI::putFile(const std::string &fileName,
    const Files::FileContent &file, const Ice::Current &)
{
    _fs.putFile(fileName, file);
}

IARnet::Files::FileContent MaximaResourceI::getFile(const std::string &fileName,
    const Ice::Current &)
{
    return _fs.getFile(fileName);
}

IARnet::Files::SeqOfFS MaximaResourceI::dispatch(const std::string &fileName,
    const Files::SeqOfFS &servers, const Ice::Current &)
{
    return _fs.dispatch(fileName, servers);
}

void MaximaResourceI::write(const std::string &fileName, int offset,
    const Files::FileContent &chunk, const Ice::Current &)
{
    _fs.write(fileName, offset, chunk);
}

void MaximaResourceI::close(const std::string &fileName, const Ice::Current &)
{
    _fs.close(fileName);
}

IARnet::Files::FileContent MaximaResourceI::read(const std::string &fileName, int offset,
    int chunkSize, const Ice::Current &)
{
    return _fs.read(fileName, offset, chunkSize);
}

