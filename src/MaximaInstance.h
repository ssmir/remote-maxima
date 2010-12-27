/*  Maxima API
    Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>
    
    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file BOOST_LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef __MAXIMAINSTANCE_H__
#define __MAXIMAINSTANCE_H__

#include <cstddef>
#include <string>
#include <deque>

#include "boost/process.hpp"
#include "boost/regex.hpp"
#include "boost/filesystem.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"

namespace bp = ::boost::process;
namespace fs = ::boost::filesystem;
using std::size_t;

namespace MaximaAPI
{

/** Represents a proxy to an instance of a Maxima process. MaximaInstance
	allows a user to interact with the Maxima at a higher level.
	That is, the user can send a command and receive a result rather than write
	raw bytes to a socket or a pipe connected to the Maxima process's stdin/out.
	MaximaInstance objects should only be created by MaximaDriver. In order
	to destroy a MaximaInstance and the corresponding Maxima process you should
	delete the MaximaInstance
	
	\author Sergey Smirnov
	*/
class MaximaInstance {	
	public:
        class MaximaIOHook
        {
        public:
            virtual ~MaximaIOHook();
            virtual std::string handle(const std::string &first,
                const std::string &second) = 0;
        };
        
        /** Launches a Maxima process and constructs a corresponding 
			MaximaInstance object to interact with Maxima. If user doesn't
            need a MaximaInstance
			object anymore he should delete it himself.
			
			maximag-disp.lisp must be in the working
			directory of the application that's using MaximaDriver.
            
            @param[in] maximaPath A full path to a Maxima executable.
            @param[in] workingDir Path to a working directory of a
                new maxima process. The directory must exist.
            @param[in] utilsDir A path to a directory that contains
                maximag-disp.lisp and winkill.exe.
		*/
        MaximaInstance(const std::string &maximaPath,
            const std::string &workingDir = ".",
            const std::string &utilsDir = ".");
        
		/**
		*/
		~MaximaInstance();
		
		/** Sends a command to the Maxima process and returns the last result.
			\sa MaximaInstance::sendCommand for the details of input processing
			\return Maxima output, starting after the last (\%o\\d+) and ending
				just before the prompt start.
		*/
		std::string executeCommand(const std::string &command);
		
		/** Sends a command to the Maxima process and returns the raw Maxima
			output.
			\sa MaximaInstance::sendCommand for the details of input processing
			\return Raw Maxima output
		*/
		std::string executeInteractive(const std::string &command);
		
		/** Interrupts the Maxima process, equivalent to sending SIGINT to 
			the Maxima process or to pressing Ctrl+C in the Maxima console.
		*/
		void interruptMaxima();
        
        /** For testing only: send a command and interrupt it after delay
            seconds.
            
            @return Reply::isInterrupted()
        */
        bool executeAndInterrupt(const std::string &command,
            size_t delay);
		
		bp::process::id_type getId() const;
        const fs::path &getWorkingDirectory() const;
        
        void setMaximaIOHook(const std::string &hookRegex, MaximaIOHook *hook);

    private:
        struct Reply {
            typedef std::deque<char> RawReply;
            typedef RawReply::iterator It;
            typedef std::pair<It, It> Range;
            
            RawReply reply;
            
            Range prompt;
            std::deque<Range> outs;
            std::deque<Range> betweens;
            
            /** Reads Maxima output until a Maxima prompt is found, then parses
                it. Maxima output is read using MaximaInstance::readsome2
                \param in A stream to which Maxima writes its output.
            */
            Reply(std::istream &in);
            
            /** Constructs a string with a Maxima reply as if Maxima was used
                from the console.
            */
            std::string concatenateParts();
            
            /** True if a prompt that terminates the reply is good, i.e. (%i\d+)
            */
            bool isGoodPrompt() const { return _goodPrompt; }
            
            /** Returns a number from a good terminating prompt */
            size_t getPromptId() const { return _promptId; }
            
            /** True if the reply contains (Console interrupt|UserBreak) in 
                betweens.
            */
            bool isInterrupted() const;
            
        private:
            Reply(const Reply &);
            void operator=(const Reply &);
            
            bool _goodPrompt;
            size_t _promptId;
        };
        
        typedef boost::shared_ptr<MaximaInstance::Reply> ReplyPtr;
        
        /** Reads as much data as possible with std::istream::readsome().
            \param in A stream to be read
            \param[out] reply A container to which the data will be appended
            \return The number of bytes read
        */
        static size_t readsome2(std::istream &in, Reply::RawReply &reply);
        
        /** sendCommand() + Reply::Reply()
        */
        ReplyPtr executeCommandRaw(const std::string &command);
        
        /** Just sends a command to the Maxima.
            If the command doesn't end with ';' or '$', ';' will be appended.
            \param[in] command The command itself
        */
        void sendCommand(std::string command);
        
        /** Checks whenther an input expression is valid enough to not hang
            the MaximaInstance. State of the checker if initialized by a caller
            with a zero value. New state of the checker is returned after each
            call to the checkInputExpression(). STATE_END is the final state.
            
            @param[in] nextChar A next input expression character to process.
            @param[in] checkerState Current checker state.
            @return New state of the checker.
        */
        int checkInputExpression(char nextChar, int checkerState) const;
        
        /** Read a reply from Maxima skipping replies from the past.
        */
        ReplyPtr readReply();

		void readPid();

		void doInterrupt(int pid);
        
        boost::scoped_ptr<bp::child> process;
        MaximaIOHook *maximaIOHook;
        boost::regex maximaIOHookRegex;
        fs::path _workingDirectory;
        fs::path _utilsDir;
        size_t _lastPromptId;
		size_t _pid;
};

}

#endif // __MAXIMAINSTANCE_H__

