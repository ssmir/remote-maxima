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

#ifndef __SERVICECONTEXT_H__
#define __SERVICECONTEXT_H__

#include "portdefs.h"
#include "SOExportDefs.h"

#include <Ice/Ice.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <set>

namespace bpt = boost::posix_time;

namespace IARnet
{
    
namespace Node
{

class NODEAPI ServantLocator;

class NODEAPI ServiceContext
{
public:
    ServiceContext(const std::string &serviceId,
        const Ice::PropertyDict &properties, ServantLocator *servantLocator,
        const Ice::CommunicatorPtr &communicator);
    
    Ice::ObjectPrx addServant(const Ice::ObjectPtr &servant,
        const std::string &name, const std::string &facet = std::string(),
        const bpt::time_duration &timeout = bpt::time_duration(),
        const std::set<std::string> &allowedClients = std::set<std::string>());
    
    void removeServant(const std::string &name,
        const std::string &facet = std::string());
    
    const std::string &getServiceId() const;
    
    const Ice::PropertyDict &getParameters() const;
    Ice::PropertyDict &getParameters();
    
    Ice::CommunicatorPtr getCommunicator() const;
    
    ServantLocator *getServantLocator() const;
    
private:
    std::string _serviceId;
    ServantLocator *_servantLocator;
    Ice::PropertyDict _properties;
    Ice::CommunicatorPtr _communicator;
};

}

}

#endif // __SERVICECONTEXT_H__

