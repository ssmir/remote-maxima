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

