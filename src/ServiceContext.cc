#include <IARnet/ServiceContext.h>
#include <IARnet/ServantLocator.h>

using namespace IARnet::Node;

ServiceContext::ServiceContext(const std::string &serviceId,
    const Ice::PropertyDict &properties, ServantLocator *servantLocator,
    const Ice::CommunicatorPtr &communicator)
:   _serviceId(serviceId), _properties(properties),
    _communicator(communicator)
{
    _servantLocator = servantLocator;
}

Ice::ObjectPrx ServiceContext::addServant(const Ice::ObjectPtr &servant,
    const std::string &name, const std::string &facet,
    const bpt::time_duration &timeout,
    const std::set<std::string> &allowedClients)
{
    return _servantLocator->addServant(servant, _serviceId, name, _serviceId,
        facet, timeout, allowedClients);
}

void ServiceContext::removeServant(const std::string &name, const std::string &facet)
{
    _servantLocator->removeServant(name, _serviceId, facet);
}

const std::string &ServiceContext::getServiceId() const
{
    return _serviceId;
}

const Ice::PropertyDict &ServiceContext::getParameters() const
{
    return _properties;
}

Ice::PropertyDict &ServiceContext::getParameters()
{
    return _properties;
}

Ice::CommunicatorPtr ServiceContext::getCommunicator() const
{
    return _communicator;
}

ServantLocator *ServiceContext::getServantLocator() const
{
    return _servantLocator;
}

