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

#ifndef __MAXIMAFACTORYI_H__
#define __MAXIMAFACTORYI_H__

#include <IARnet/ServiceContext.h>
#include "Maxima.h"
#include <IARnet/Service.h>
#include "FileServerImpl.h"

namespace IARnet
{

namespace Maxima
{

class ICE_DECLSPEC_EXPORT MaximaFactoryI : virtual public MaximaFactory,
    virtual public IARnet::Node::Service
{
	public:
		MaximaFactoryI(const IARnet::Node::ServiceContext &context);
		~MaximaFactoryI();
		
        virtual void serviceInit(const std::string &proxy);
        
		virtual MaximaResourcePrx launchMaxima(const ::Ice::Current &);
        
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
        IARnet::Node::ServiceContext _context;
        Files::FileServerImpl _fs;
        MasterFactoryPrx _master;
        MaximaFactoryPrx _proxy;
        size_t _maximaCount;
        
    protected:
        bool _masterFactory;
};

}

}

#endif // __MAXIMAFACTORYI_H__

