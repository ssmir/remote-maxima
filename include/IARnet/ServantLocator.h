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

#ifndef __SERVANTLOCATOR_H__
#define __SERVANTLOCATOR_H__

#include "SOExportDefs.h"
#include "portdefs.h"
#include <set>
#include <string>
#include <stdexcept>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Ice/Logger.h>
#include <Ice/ServantLocator.h>
#include <IceSSL/Plugin.h>
#include <IceUtil/Mutex.h>

#include "Security.h"
#include "AccessDeniedServant.h"

namespace bpt = boost::posix_time;
    
namespace IARnet
{
    
namespace Node
{

class NODEAPI ServantLocator : public Ice::ServantLocator,
    public IceUtil::Mutex
{
public:
    class NODEAPI AlreadyRegisteredException : public std::runtime_error
    {
    public:
        AlreadyRegisteredException(const Ice::Identity &id,
            const std::string &facet);
        ~AlreadyRegisteredException() throw ();
    };
    
    ServantLocator(const std::string &nodeName,
        const Ice::ObjectAdapterPtr &adapter,
        const IARnet::Security::AuthorizationServicePrx &authService,
        const IARnet::Security::SSLAuthorizationServicePrx &sslAuthService);
    
    virtual Ice::ObjectPtr locate(const Ice::Current& current,
        Ice::LocalObjectPtr& cookie);

    virtual void finished(const Ice::Current& current,
        const Ice::ObjectPtr& servant, const Ice::LocalObjectPtr& cookie);

    virtual void deactivate(const std::string& category);
    
    Ice::ObjectPrx addServant(const Ice::ObjectPtr &servant,
        const std::string &serviceId, const std::string &name,
        const std::string &category, const std::string &facet = std::string(),
        const bpt::time_duration &timeout = bpt::time_duration(),
        const std::set<std::string> &allowedClients = std::set<std::string>());
    
    void removeServant(const std::string &name, const std::string &category,
        const std::string &facet);
    void removeServantsForService(const std::string &serviceId);
    
private:
    struct ServantEntry
    {
        Ice::ObjectPtr _servant;
        std::string _serviceId;
        std::set<std::string> _allowedClients;
        bpt::time_duration _timeout;
        bpt::ptime _lastAccessTime;
    };
    
    bool checkAuthorization(const std::string &clientId,
        const std::string &password, const IARnet::Security::RequestInfo &info);
    
    bool checkAuthorizationSSL(const IceSSL::ConnectionInfo &conInfo,
        const IARnet::Security::RequestInfo &info);
    
    bool checkAuthorizations(const Ice::Current& current,
        const std::string &serviceId, std::string &clientId);
    
    bool checkAccessGranted(const Ice::Current& current,
        const std::string &serviceId, const std::set<std::string> &allowedClients,
        std::string &clientId);
    
    typedef std::map<std::pair<Ice::Identity, std::string>, ServantEntry>
        ServantMap;
    
    IARnet::Security::AuthorizationServicePrx _authService;
    IARnet::Security::SSLAuthorizationServicePrx _sslAuthService;
    
    std::map<std::string, bpt::ptime> _grantedPermissions;
    ServantMap _servants;
    
    const std::string _nodeName;
    Ice::LoggerPtr _log;
    Ice::ObjectPtr _accessDeniedServant;
    Ice::ObjectAdapterPtr _adapter;
};

}

}

#endif // __SERVANTLOCATOR_H__

