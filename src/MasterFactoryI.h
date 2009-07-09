#ifndef __MASTERFACTORY_H__
#define __MASTERFACTORY_H__

#include "MaximaFactoryI.h"

namespace IARnet
{
namespace Maxima
{

class ICE_DECLSPEC_EXPORT MasterFactoryI : virtual public MasterFactory,
    virtual public MaximaFactoryI
{
public:
    MasterFactoryI(const IARnet::Node::ServiceContext &context);
    
    virtual void registerFactory(const MaximaFactoryPrx &factory,
        const Ice::Current &);
    
    virtual void unregisterFactory(const MaximaFactoryPrx &factory,
        const Ice::Current &);
    
    virtual Factories getSlaveFactories(const Ice::Current &);
    
private:
    Factories _factories;
};

}
}

#endif // __MASTERFACTORY_H__

