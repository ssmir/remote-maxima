#include "MasterFactoryI.h"

using namespace IARnet::Maxima;

MasterFactoryI::MasterFactoryI(const IARnet::Node::ServiceContext &context)
:   MaximaFactoryI(context)
{
}

void MasterFactoryI::registerFactory(const MaximaFactoryPrx &factory,
    const Ice::Current &)
{
    _factories.push_back(factory);
}

void MasterFactoryI::unregisterFactory(const MaximaFactoryPrx &factory,
    const Ice::Current &)
{
    Factories::iterator it = std::find(_factories.begin(), _factories.end(),
        factory);
    
    if (it != _factories.end())
    {
        _factories.erase(it);
    }
}

Factories MasterFactoryI::getSlaveFactories(const Ice::Current &)
{
    return _factories;
}

extern "C" ICE_DECLSPEC_EXPORT
Ice::Object *createMasterFactory(const IARnet::Node::ServiceContext &context)
{
    return new MasterFactoryI(context);
}

