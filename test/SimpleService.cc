#include <IARnet/ServiceContext.h>
#include <IARnet/Service.h>

class SimpleService : public Ice::Object, public IARnet::Node::Service
{
public:
    SimpleService(Ice::PropertyDict params)
    {
        std::cout << "SimpleService constructor parameters:" << std::endl;
        for (Ice::PropertyDict::iterator i = params.begin();
            i != params.end(); ++i)
        {
            std::cout << i->first << " = " << i->second << std::endl;
        }
    }
    virtual ~SimpleService()
    {
        std::cout << "SimpleService destructor" << std::endl;
    }
    virtual void serviceInit(const std::string &proxy)
    {
        std::cout << "SimpleService::serviceInit: proxy: " << proxy << std::endl;
    }
};

extern "C"
Ice::Object *createSimpleService(const IARnet::Node::ServiceContext &context)
{
    return new SimpleService(context.getParameters());
}

