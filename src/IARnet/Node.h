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

#ifndef __NODE_H__
#define __NODE_H__

#include "SOExportDefs.h"
#include "portdefs.h"
#include "ServantLocator.h"
#include "InfoService.h"
#include "ServiceContext.h"
#include <Ice/Ice.h>
#include <Ice/DynamicLibraryF.h>
#include <Ice/Communicator.h>
#include <Ice/Properties.h>
#include <list>

namespace IARnet
{

namespace Node
{

class NODEAPI Node
{
public:
    Node();
    ~Node();
    
    void init(Ice::CommunicatorPtr comm);
    void stopServices();
    
    void deployService(const std::string &serviceId,
        const Ice::PropertiesPtr &config);
    void startService(const std::string &serviceId);
    void stopService(const std::string &serviceId);
    
private:
    typedef Ice::Object * (*ServiceFactory)(const ServiceContext &context);
    
    struct ServiceEntry
    {
        std::string _serviceId;
        IARnet::InfoService::ServiceInfoPtr _info;
        bool _started;
        Ice::PropertyDict _serviceParams;
        ServiceFactory _factory;
        IceInternal::DynamicLibraryPtr _library;
    };
    
    Ice::LoggerPtr _log;
    ServantLocator *_servantLocator;
    Ice::ServantLocatorPtr _servantLocatorPtr;
    std::string _nodeName;
    IARnet::InfoService::InfoPublishPrx _infoPublish;
    Ice::CommunicatorPtr _comm;
    
    typedef std::list<ServiceEntry> ServiceList;
    ServiceList _services;
    
    ServiceList::iterator findService(const std::string &serviceId);
    void startServices(const Ice::PropertiesPtr &config);
};

}

}

#endif // __NODE_H__

