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

#include <IARnet/Node.h>
#include <Ice/Application.h>
#include <IceUtil/Mutex.h>
#include <signal.h>

class NodeApplication : public Ice::Application
{
public:
    NodeApplication()
    {
        _node = NULL;
        _interrupted = false;
    }
    
    ~NodeApplication()
    {
        delete _node;
    }
    
    virtual int run(int argc, char *argv[])
    {
        callbackOnInterrupt();
        
        _node = new IARnet::Node::Node();
        _node->init(communicator());
        communicator()->waitForShutdown();
        return 0;
    }
    
    virtual void interruptCallback(int)
    {
        if (!_interrupted)
        {
            _interrupted = true;
            try
            {
                _node->stopServices();
            }
            catch (const Ice::Exception &e)
            {
                std::cout << "Ice exception: " << e << std::endl;
            }
        }
        communicator()->destroy();
    }
    
private:
    IARnet::Node::Node *_node;
    bool _interrupted;
};

IARnet::Node::Node *node;
Ice::CommunicatorPtr communicator;
IceUtil::Mutex mutex;
bool terminated = false;

void handler(int)
{
    IceUtil::Mutex::Lock lock(mutex);
    
    if (terminated)
    {
        return;
    }
    
    try
    {
        node->stopServices();
    }
    catch (const Ice::Exception &e)
    {
        std::cout << "Ice exception: " << e << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "std::exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown error" << std::endl;
    }
    communicator->destroy();
    terminated = true;
}

int main(int argc, char **argv)
{
#ifndef _WIN32
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    
    communicator = Ice::initialize(argc, argv);
    
    node = new IARnet::Node::Node();
    node->init(communicator);
    
    communicator->waitForShutdown();
    
    delete node;
	return 0;
#else
	NodeApplication app;
	return app.main(argc, argv);
#endif
}

