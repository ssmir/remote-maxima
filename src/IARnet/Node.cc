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

#include "Node.h"
#include "Security.h"
#include "ServantLocator.h"
#include "InfoService.h"
#include "Service.h"
#include <Ice/DynamicLibrary.h>
#include <Ice/ObjectAdapter.h>

using namespace IARnet::Node;

namespace {
    
Ice::PropertyDict removePrefix(const Ice::PropertyDict &props)
{
    Ice::PropertyDict result;
    
    for (Ice::PropertyDict::const_iterator i = props.begin();
        i != props.end(); ++i)
    {
        const std::string &key = i->first;
        size_t dot = key.find_first_of('.');
        std::string newKey = (dot == key.npos) ? key : key.substr(dot + 1);
        
        result[newKey] = i->second;
    }
    return result;
}

Ice::PropertyDict extractMetadata(const Ice::PropertyDict &props)
{
    std::string md("Metadata.");
    Ice::PropertyDict result;
    
    for (Ice::PropertyDict::const_iterator i = props.begin();
        i != props.end(); ++i)
    {
        if (i->first.find(md) == 0)
        {
            std::string key = i->first;
            key.erase(0, md.size());
            result[key] = i->second;
        }
    }
    return result;
}

}

Node::Node()
: _servantLocator(NULL)
{
}

Node::~Node()
{
}

void Node::init(Ice::CommunicatorPtr comm)
{
    _comm = comm;
    
    Ice::PropertiesPtr config = comm->getProperties();
    _log = comm->getLogger();
    
    char hostname[256];
    gethostname(hostname, 255);
    
    _nodeName = config->getPropertyWithDefault("Node.Name", hostname);

    _log->trace("Node", "Starting up... Name: " + _nodeName);

    IARnet::Security::AuthorizationServicePrx authService =
        IARnet::Security::AuthorizationServicePrx::checkedCast(
            comm->propertyToProxy("AuthorizationService.Proxy"));
    _log->trace("Node", "AuthorizationService: "
        + comm->proxyToString(authService));

    IARnet::Security::SSLAuthorizationServicePrx sslAuthService =
        IARnet::Security::SSLAuthorizationServicePrx::checkedCast(
            comm->propertyToProxy("SSLAuthorizationService.Proxy"));
    _log->trace("Node", "SSLAuthorizationService: "
        + comm->proxyToString(sslAuthService));
    
    //IARnet::InfoService::InfoQueryPrx queryPrx =
    //    IARnet::InfoService::InfoQueryPrx::checkedCast(
    //        comm->propertyToProxy("InfoService.Proxy"));
    _infoPublish = IARnet::InfoService::InfoPublishPrx::checkedCast(
            comm->propertyToProxy("InfoService.Proxy"), "publish");
    _log->trace("Node", "Connected to InfoService: "
        + comm->proxyToString(_infoPublish));
    
    Ice::ObjectAdapterPtr adapter =
        comm->createObjectAdapterWithEndpoints("Node",
            config->getPropertyWithDefault("Node.Endpoints", "tcp"));
    
    _servantLocator = new ServantLocator(_nodeName, adapter, authService,
        sslAuthService);
    _servantLocatorPtr = _servantLocator;
    adapter->addServantLocator(_servantLocatorPtr, "");
    adapter->activate();
    _log->trace("Node", "Initialized object adapter");
    
    startServices(config);
    
    _log->trace("Node", "Started");
}

void Node::stopServices()
{
    for (ServiceList::reverse_iterator it = _services.rbegin();
        it != _services.rend(); ++it)
    {
        stopService(it->_serviceId);
    }
}

void Node::startServices(const Ice::PropertiesPtr &config)
{
    Ice::PropertyDict serviceIds =
        removePrefix(config->getPropertiesForPrefix("Service."));
    std::string loadOrder = config->getProperty("Node.LoadOrder");
    size_t next = 0;
    while (next < loadOrder.size())
    {
        size_t space = loadOrder.find(' ', next);
        std::string serviceId = loadOrder.substr(next, space);
        next = (space == loadOrder.npos) ? loadOrder.size() : space + 1;
        
        if (serviceIds.find(serviceId) != serviceIds.end())
        {
            deployService(serviceId, config);
            startService(serviceId);
            serviceIds.erase(serviceId);
        }
    }
    for (Ice::PropertyDict::iterator it = serviceIds.begin();
        it != serviceIds.end(); ++it)
    {
        deployService(it->first, config);
        startService(it->first);
    }
}

void Node::deployService(const std::string &serviceId,
    const Ice::PropertiesPtr &config)
{
    _log->trace("Node", "Deploing " + serviceId);
    
    if (findService(serviceId) != _services.end())
    {
        throw std::runtime_error("Service is already deployed: " + serviceId);
    }
    
    ServiceEntry entry;
    
    entry._library = new IceInternal::DynamicLibrary();
    
    std::string entryPoint = config->getProperty("Service." + serviceId);
    IceInternal::DynamicLibrary::symbol_type symbol =
        entry._library->loadEntryPoint(entryPoint, false);
    if(symbol == 0)
    {
        std::string msg = entry._library->getErrorMessage();
        throw std::runtime_error("Service deploy failed for " + serviceId + 
            ": unable to load entry point '" + entryPoint + "': " + msg);
    }
    entry._factory = (ServiceFactory)symbol;
    
    entry._serviceId = serviceId;
    entry._started = false;
    
    entry._serviceParams = removePrefix(config->getPropertiesForPrefix(serviceId));
    
    entry._info = new IARnet::InfoService::ServiceInfo();
    entry._info->description = entry._serviceParams["Description"];
    entry._info->metadata = extractMetadata(entry._serviceParams);
    
    std::string &tags = entry._serviceParams["Tags"];
    size_t next = 0;
    while (next < tags.size())
    {
        size_t comma = tags.find(',', next);
        entry._info->tags.push_back(tags.substr(next, comma));
        next = (comma == tags.npos) ? tags.size() : comma + 1;
    }
    
    _services.push_back(entry);
    
    _log->trace("Node", "Deployed service: " + serviceId);
}

void Node::startService(const std::string &serviceId)
{
    _log->trace("Node", "Starting " + serviceId);
    
    ServiceList::iterator entryPtr = findService(serviceId);
    if (entryPtr == _services.end())
    {
        throw std::runtime_error("Service was not deployed: " + serviceId);
    }
    
    if (entryPtr->_started)
    {
        throw std::runtime_error("Service is already started: " + serviceId);
    }
    
    ServiceContext srvContext(serviceId, entryPtr->_serviceParams,
        _servantLocator, _comm);
    
    Ice::ObjectPtr servant = entryPtr->_factory(srvContext);
    
    entryPtr->_info->proxy = _servantLocator->addServant(servant, serviceId,
        serviceId, _nodeName)->ice_toString();
    
    entryPtr->_info->types = servant->ice_ids();
    
    Service *service = dynamic_cast<Service *>(servant.get());
    if (service != NULL)
    {
        service->serviceInit(entryPtr->_info->proxy);
    }
    
    entryPtr->_started = true;
    
    if (_infoPublish)
    {
        try
        {
            _infoPublish->publishService(entryPtr->_info);
        }
        catch (const IARnet::InfoService::PublishedByAnotherClientException &)
        {
            _log->trace("Node", "Failed to publish " + serviceId
                + " in InfoService: Published by another client");
        }
    }
    
    _log->trace("Node", "Started service: " + serviceId);
}

void Node::stopService(const std::string &serviceId)
{
    _log->trace("Node", "Stopping " + serviceId);
    
    ServiceList::iterator it = findService(serviceId);
    if (it == _services.end())
    {
        throw std::runtime_error("Failed to stop service '" + serviceId
            + "': service is not deployed");
    }
    if (!it->_started)
    {
        throw std::runtime_error("Failed to stop service '" + serviceId
            + "': service is already stopped");
    }
    
    if (_infoPublish)
    {
        try
        {
            _infoPublish->unpublishService(it->_info->proxy);
        }
        catch (const IARnet::InfoService::ServiceNotPublishedException &)
        {
            _log->trace("Node", "Failed to unpublish " + serviceId
                + " in InfoService: Not published");
        }
        catch (const IARnet::InfoService::PublishedByAnotherClientException &)
        {
            _log->trace("Node", "Failed to unpublish " + serviceId
                + " in InfoService: Published by another client");
        }
    }
    
    _servantLocator->removeServantsForService(serviceId);
    
    it->_started = false;
    
    _log->trace("Node", "Service stopped: " + serviceId);
}

Node::ServiceList::iterator Node::findService(const std::string &serviceId)
{
    ServiceList::iterator it = _services.begin();
    while (it != _services.end())
    {
        if (it->_serviceId == serviceId)
        {
            return it;
        }
        ++it;
    }
    return it;
}

