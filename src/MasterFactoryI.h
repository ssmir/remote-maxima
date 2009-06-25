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

