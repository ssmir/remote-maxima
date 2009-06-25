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

