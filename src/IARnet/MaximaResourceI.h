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

#ifndef __MAXIMARESOURCEI_H__
#define __MAXIMARESOURCEI_H__

#include "ServiceContext.h"
#include "Maxima.h"
#include "FileServerImpl.h"
#include <IceUtil/Mutex.h>
#include <Ice/Logger.h>
#include <boost/scoped_ptr.hpp>

namespace MaximaAPI
{
class MaximaInstance;
}

namespace IARnet
{
    
namespace Maxima
{

class ICE_DECLSPEC_EXPORT MaximaResourceI : public MaximaResource,
    public IceUtil::Mutex
{
	public:
		MaximaResourceI(const std::string &maximaPath,
			const std::string &workingDirectory,
			const IARnet::Node::ServiceContext &context);
        
		~MaximaResourceI();
		
		virtual std::string executeCommand(const std::string &command,
            const Ice::Current&);
		
		virtual std::string executeInteractive(const std::string &command,
            const Ice::Current &);
		
        virtual void interrupt(const Ice::Current &);
        
        virtual void restart(const Ice::Current &);

		virtual void destroy(const Ice::Current &);
        
        virtual void putFile(const std::string &fileName,
            const Files::FileContent &file, const Ice::Current &);
    
        virtual Files::FileContent getFile(const std::string &fileName,
            const Ice::Current &);
        
        virtual Files::SeqOfFS dispatch(const std::string &fileName,
            const Files::SeqOfFS &servers, const Ice::Current &);
        
        virtual void write(const std::string &fileName, int offset,
            const Files::FileContent &chunk, const Ice::Current &);
        
        virtual void close(const std::string &fileName, const Ice::Current &);

        virtual Files::FileContent read(const std::string &fileName, int offset,
            int chunkSize, const Ice::Current &);
        
    private:
		void launchMaxima();

		const std::string _maximaPath;
		const std::string _workingDirectory;
        boost::scoped_ptr<MaximaAPI::MaximaInstance> _maxima;
        IARnet::Node::ServiceContext _context;
        Files::FileServerImpl _fs;
        Ice::LoggerPtr _log;
};

}

}

#endif // __MAXIMARESOURCEI_H__

