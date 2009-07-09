#include "MaximaFactoryI.h"
#include <MaximaAPI/MaximaInstance.h>
#include "MaximaResourceI.h"

#include <ios>
#include <fstream>
#include <boost/filesystem.hpp>

using namespace IARnet::Maxima;

namespace fs = ::boost::filesystem;

MaximaFactoryI::MaximaFactoryI(const IARnet::Node::ServiceContext &context)
:   _context(context),
    _fs("common")
{
    _maximaCount = 0;
}

MaximaFactoryI::~MaximaFactoryI()
{
    if (_master && _proxy != _master)
    {
        _master->unregisterFactory(_proxy);
    }
}

MaximaResourcePrx MaximaFactoryI::launchMaxima(const ::Ice::Current &) 
{
    std::ostringstream dir;
    dir << "maxima" << _maximaCount++;
    
    return MaximaResourcePrx::checkedCast(_context.addServant(
        new MaximaResourceI(_context.getParameters()["MaximaPath"],
			dir.str(), _context), dir.str()));
}

void MaximaFactoryI::serviceInit(const std::string &proxy)
{
    std::string masterProxy = _context.getParameters()["MasterProxy"];
    if (!masterProxy.empty())
    {
        _master = MasterFactoryPrx::checkedCast(
            _context.getCommunicator()->stringToProxy(masterProxy));
    }
    
    _proxy = MaximaFactoryPrx::checkedCast(
        _context.getCommunicator()->stringToProxy(proxy));
    
    if (_master)// && _proxy != _master)
    {
        _master->registerFactory(_proxy);
    }
}

void MaximaFactoryI::putFile(const std::string &fileName,
    const Files::FileContent &file, const Ice::Current &)
{
    _fs.putFile(fileName, file);
}

IARnet::Files::FileContent MaximaFactoryI::getFile(const std::string &fileName,
    const Ice::Current &)
{
    return _fs.getFile(fileName);
}

IARnet::Files::SeqOfFS MaximaFactoryI::dispatch(const std::string &fileName,
    const Files::SeqOfFS &servers, const Ice::Current &)
{
    return _fs.dispatch(fileName, servers);
}

extern "C" ICE_DECLSPEC_EXPORT
Ice::Object *createMaximaFactory(const IARnet::Node::ServiceContext &context)
{
    return new MaximaFactoryI(context);
}

