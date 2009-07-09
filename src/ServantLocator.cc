#include <IARnet/ServantLocator.h>

#include <Ice/Connection.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/Communicator.h>

using namespace IARnet::Node;

namespace
{

std::string getFromContext(const Ice::Context &ctx, const std::string &key)
{
    Ice::Context::const_iterator it = ctx.find(key);
    if (it == ctx.end())
    {
        return std::string();
    }
    return it->second;
}
    
}

ServantLocator::ServantLocator(const std::string &nodeName,
    const Ice::ObjectAdapterPtr &adapter,
    const IARnet::Security::AuthorizationServicePrx &authService,
    const IARnet::Security::SSLAuthorizationServicePrx &sslAuthService)
:   _authService(authService),
    _sslAuthService(sslAuthService),
    _nodeName(nodeName),
    _adapter(adapter)
{
    _log = _adapter->getCommunicator()->getLogger();
    _accessDeniedServant = new AccessDeniedServant();
}

bool ServantLocator::checkAuthorization(const std::string &clientId,
    const std::string &password, const IARnet::Security::RequestInfo &info)
{
    if (!_authService)
    {
        return false;
    }
    
    std::string permission = clientId + " " + password + " "
        + info.objectId.category + "/" + info.objectId.name + " " + info.facet;
    
    if (_grantedPermissions.find(permission) != _grantedPermissions.end())
    {
        return true;
    }
    
    std::string reason;
    bool result = _authService->authorize(clientId, password, info, reason);
    if (result)
    {
        _grantedPermissions[permission] = bpt::microsec_clock::universal_time();
    }
    return result;
}

bool ServantLocator::checkAuthorizationSSL(const IceSSL::ConnectionInfo &conInfo,
    const IARnet::Security::RequestInfo &info)
{
    if (!_sslAuthService)
    {
        return false;
    }
    
    std::string permission = (std::string)conInfo.certs[0]->getSubjectDN() +
        "  " + info.objectId.category + "/" + info.objectId.name + " " +
        info.facet;
    
    if (_grantedPermissions.find(permission) != _grantedPermissions.end())
    {
        return true;
    }
    
    std::vector<std::string> strCerts(conInfo.certs.size());
    for (size_t i = 0; i < conInfo.certs.size(); ++i) {
        strCerts[i] = conInfo.certs[i]->encode();
    }
    
    IARnet::Security::RequestInfo info2(info);
    
    hostent *hostEntry = gethostbyaddr((const char *)&conInfo.remoteAddr/*.sin_addr*/,
        sizeof(in_addr), AF_INET);
    if (hostEntry != NULL)
    {
        info2.remoteHost = hostEntry->h_name;
    }
    
    //info2.remoteHost = inet_ntoa(conInfo.remoteAddr.sin_addr);
    
    std::string reason;
    bool result = _sslAuthService->authorize(strCerts, info2, reason);
    if (result)
    {
        _grantedPermissions[permission] = bpt::microsec_clock::universal_time();
    }
    return result;
}

bool ServantLocator::checkAuthorizations(const Ice::Current& current,
    const std::string &serviceId, std::string &clientId)
{
    clientId = getFromContext(current.ctx, "IARnet.User");
    std::string password(getFromContext(current.ctx, "IARnet.Password"));
    
    IARnet::Security::RequestInfo info;
    
    info.serverName = _nodeName;
    info.serviceId = serviceId;
    info.objectId = current.id;
    info.facet = current.facet;
    
    if (!clientId.empty())
    {
        return checkAuthorization(clientId, password, info);
    }
    else if (current.con && current.con->type() == "ssl")
    {
        IceSSL::ConnectionInfo conInfo = IceSSL::getConnectionInfo(current.con);
        if (!conInfo.certs.empty())
        {
            clientId = conInfo.certs[0]->getSubjectDN();
            return checkAuthorizationSSL(conInfo, info);
        }
    }
    clientId = "anonymous";
    return checkAuthorization(clientId, "", info);
}

bool ServantLocator::checkAccessGranted(const Ice::Current& current,
    const std::string &serviceId, const std::set<std::string> &allowedClients,
    std::string &clientId)
{
    bool accessGranted = checkAuthorizations(current, serviceId, clientId);
    if (!_authService && !_sslAuthService)
    {
        return true;
    }
    
    if (accessGranted && !allowedClients.empty()
        && allowedClients.find(clientId) == allowedClients.end())
    {
        return false;
    }
    
    return accessGranted;
}

Ice::ObjectPtr ServantLocator::locate(const Ice::Current& current,
    Ice::LocalObjectPtr& cookie)
{   
    IceUtil::Mutex::Lock lock(*this);
    
    ServantMap::iterator it = _servants.find(std::make_pair(
        current.id, current.facet));
    if (it == _servants.end())
    {
		_log->trace("ServantLocator", " >>> " 
				+ current.id.category + "/" + current.id.name + " "
				+ current.operation + " UNKNOWN");
        return Ice::ObjectPtr();
    }
    ServantEntry &servantEntry = it->second;
    
    std::string clientId;
    if (!checkAccessGranted(current, servantEntry._serviceId,
        servantEntry._allowedClients, clientId))
    {
        _log->trace("ServantLocator", clientId + " >>> " 
                + current.id.category + "/" + current.id.name + " "
                + current.operation + " DENIED");
        return _accessDeniedServant;
    }
    
    _log->trace("ServantLocator", clientId + " >>> " 
            + current.id.category + "/" + current.id.name + " "
            + current.operation + " GRANTED");
    
    //current.ctx["Request.ClientId"] = clientId;
    servantEntry._lastAccessTime = bpt::microsec_clock::universal_time();
    
    return servantEntry._servant;
}

void ServantLocator::finished(const Ice::Current& current,
        const Ice::ObjectPtr& servant, const Ice::LocalObjectPtr& cookie)
{
}

void ServantLocator::deactivate(const std::string& category)
{
}

Ice::ObjectPrx ServantLocator::addServant(const Ice::ObjectPtr &servant,
    const std::string &serviceId, const std::string &name,
    const std::string &category, const std::string &facet,
    const bpt::time_duration &timeout,
    const std::set<std::string> &allowedClients)
{
    IceUtil::Mutex::Lock lock(*this);
    
    Ice::Identity id;
    id.name = name;
    id.category = category;
    ServantMap::iterator it = _servants.find(std::make_pair(id, facet));
    if (it != _servants.end())
    {
        throw AlreadyRegisteredException(id, facet);
    }
    
    ServantEntry servantEntry;
    servantEntry._serviceId = serviceId;
    servantEntry._lastAccessTime = bpt::microsec_clock::universal_time();
    servantEntry._servant = servant;
    servantEntry._timeout = timeout;
    servantEntry._allowedClients = allowedClients;
    
    _servants[std::make_pair(id, facet)] = servantEntry;
    
    Ice::ObjectPrx result = _adapter->createProxy(id);
    
    _log->trace("ServantLocator", "Added servant " + id.category + "/" + id.name 
        + " facet: '" + facet + "'");
    
    return result;
}

void ServantLocator::removeServant(const std::string &name,
    const std::string &category, const std::string &facet)
{
    IceUtil::Mutex::Lock lock(*this);
    
    Ice::Identity id;
    id.name = name;
    id.category = category;
    ServantMap::iterator it = _servants.find(std::make_pair(id, facet));
    
    if (it == _servants.end())
    {
        _log->trace("ServantLocator", "Failed to remove servant " + category
            + "/" + name + " (" + facet + "): it doesn't exist");
        return;
    }
    _servants.erase(it);
    
    _log->trace("ServantLocator", "Servant removed: " + id.category + "/"
        + id.name + " facet: '" + facet + "'");
}

void ServantLocator::removeServantsForService(const std::string &serviceId)
{
    IceUtil::Mutex::Lock lock(*this);
    
    ServantMap::iterator it = _servants.begin();
    while (it != _servants.end())
    {
        ServantMap::iterator copy = it++;
        if (copy->second._serviceId == serviceId)
        {
            _servants.erase(copy);
        }
    }
}

ServantLocator::AlreadyRegisteredException::AlreadyRegisteredException(
    const Ice::Identity &id,
    const std::string &facet)
: std::runtime_error("Servant for identity " + id.category + "/" + id.name
        + ", facet '" + facet + "' is already activated.")
{
}

ServantLocator::AlreadyRegisteredException::~AlreadyRegisteredException() throw()
{
}

