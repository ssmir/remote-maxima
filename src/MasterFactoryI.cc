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

