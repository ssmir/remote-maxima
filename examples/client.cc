#include <Ice/Application.h>
#include "Maxima.h"
#include "FileServerImpl.h"
#include <iterator>
#include <algorithm>

namespace Maxima = ::IARnet::Maxima;

class MaximaClient : public Ice::Application
{
	public:
		virtual int run(int, char*[]);
		virtual void interruptCallback(int);
        
    private:
        bool processInput(const std::string &in);
        
        Maxima::MaximaResourcePrx _maxima;
};

int main(int argc, char* argv[])
{
	MaximaClient app;
	return app.main(argc, argv);
}

class ECCallback : public Maxima::AMI_MaximaResource_executeCommand
{
public:
    ECCallback(const std::string &in)
    :   _in(in)
    {
    }
    
    virtual void ice_exception(const Ice::Exception &ex)
    {
        std::cout << "Exception arrived for '" << _in << "':\n";
        try
        {
            ex.ice_throw();
        }
        catch (const Maxima::MaximaException &e)
        {
            std::cout << e.reason << std::endl;
        }
    }

    virtual void ice_response(const std::string &result)
    {
        std::cout << "Result arrived for '" << _in << "' ";
        if (result.size() > 1024)
        {
            std::cout << "(truncated from " << result.size() << " bytes):\n";
            std::copy(result.begin(), result.begin() + 1024,
                std::ostream_iterator<char>(std::cout));
            std::cout << std::endl;
        }
        else
        {
            std::cout << ":\n" << result << std::endl;
        }
    }
    
private:
    std::string _in;
};

bool MaximaClient::processInput(const std::string &in)
{
    if (in == "q")
    {
        return false;
    }
    
    if (in == "i")
    {
        _maxima->interrupt();
        return true;
    }
	
    if (in == "r")
    {
        _maxima->restart();
        return true;
    }
    
    if (in.find("p ") == 0)
    {
        fs::path p("//:" + in.substr(2));
        std::cout << "Uploading " << p << std::endl;
        _maxima->putFile(p.leaf(), IARnet::Files::FileServerImpl::loadFile(p));
        return true;
    }
    if (in.find("g ") == 0)
    {
        std::string p(in.substr(2));
        std::cout << "Downloading " << p << std::endl;
        IARnet::Files::FileServerImpl::saveFile(p, _maxima->getFile(p));
        return true;
    }
    
    _maxima->executeCommand_async(new ECCallback(in), in);

    return true;
}

int MaximaClient::run(int argc, char* argv[])
{
	callbackOnInterrupt();

	Maxima::MasterFactoryPrx master = 
		Maxima::MasterFactoryPrx::checkedCast(
			communicator()->propertyToProxy("MasterFactory.Proxy"));
	if (!master)
    {
		std::cerr << argv[0] << ": invalid proxy" << std::endl;
		return EXIT_FAILURE;
	}
    
    Maxima::Factories factories = master->getSlaveFactories();
    std::cout << "Registered factories:\n";
    for (size_t i = 0; i < factories.size(); ++i)
    {
        std::cout << i << ": " << factories[i] << std::endl;
    }
    
    std::cout << "Choose a factory> ";
    std::cout.flush();
    
    std::string line;
    std::getline(std::cin, line);    
    size_t id = atoi(line.c_str());
    id %= factories.size();

	try
    {
		_maxima = factories[id]->launchMaxima();
        std::cout << "\nMaximaResource from factory "
            << id << " was successfully launched" << std::endl;
		
		while (true)
        {
			std::cout << "> ";
			std::string buf;
			std::getline(std::cin, buf);
            
            if (!processInput(buf))
            {
                break;
            }
		}
		
		_maxima->destroy();
    }
    catch (const Maxima::MaximaException &e)
    {
        std::cout << "MaximaError: " << e.reason << std::endl;
    }
	
	return EXIT_SUCCESS;
}

void MaximaClient::interruptCallback(int)
{
	try
    {
		communicator()->destroy();
	}
    catch (const IceUtil::Exception& ex)
    {
		std::cerr << appName() << ": " << ex << std::endl;
	}
    catch (...)
    {
		std::cerr << appName() << ": unknown exception" << std::endl;
	}
	exit(EXIT_SUCCESS);
}

