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

