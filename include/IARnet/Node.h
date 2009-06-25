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

