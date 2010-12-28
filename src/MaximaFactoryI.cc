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

#include "MaximaFactoryI.h"
#include "MaximaInstance.h"
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
    _masterFactory = false;
}

MaximaFactoryI::~MaximaFactoryI()
{
    if (_master && !_masterFactory)
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
    
    if (_master)// && !_masterFactory)
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

void MaximaFactoryI::write(const std::string &fileName, int offset,
    const Files::FileContent &chunk, const Ice::Current &)
{
    _fs.write(fileName, offset, chunk);
}

void MaximaFactoryI::close(const std::string &fileName, const Ice::Current &)
{
    _fs.close(fileName);
}

IARnet::Files::FileContent MaximaFactoryI::read(const std::string &fileName, int offset,
    int chunkSize, const Ice::Current &)
{
    return _fs.read(fileName, offset, chunkSize);
}

extern "C" ICE_DECLSPEC_EXPORT
Ice::Object *createMaximaFactory(const IARnet::Node::ServiceContext &context)
{
    return new MaximaFactoryI(context);
}

