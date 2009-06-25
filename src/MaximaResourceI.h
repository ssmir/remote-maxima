#ifndef __MAXIMARESOURCEI_H__
#define __MAXIMARESOURCEI_H__

#include <IARnet/ServiceContext.h>
#include "Maxima.h"
#include "FileServerImpl.h"
#include <IceUtil/Mutex.h>
#include <Ice/Logger.h>
#include <boost/scoped_ptr.hpp>

namespace MaximaAPI
{
class MaximaInstance;
}

namespace IARnet
{
    
namespace Maxima
{

class ICE_DECLSPEC_EXPORT MaximaResourceI : public MaximaResource,
    public IceUtil::Mutex
{
	public:
		MaximaResourceI(const std::string &maximaPath,
			const std::string &workingDirectory,
			const IARnet::Node::ServiceContext &context);
        
		~MaximaResourceI();
		
		virtual std::string executeCommand(const std::string &command,
            const Ice::Current&);
		
		virtual std::string executeInteractive(const std::string &command,
            const Ice::Current &);
		
        virtual void interrupt(const Ice::Current &);
        
        virtual void restart(const Ice::Current &);

		virtual void destroy(const Ice::Current &);
        
        virtual void putFile(const std::string &fileName,
            const Files::FileContent &file, const Ice::Current &);
    
        virtual Files::FileContent getFile(const std::string &fileName,
            const Ice::Current &);
        
        virtual Files::SeqOfFS dispatch(const std::string &fileName,
            const Files::SeqOfFS &servers, const Ice::Current &);
        
    private:
		void launchMaxima();

		std::string _maximaPath;
		std::string _workingDirectory;
        boost::scoped_ptr<MaximaAPI::MaximaInstance> _maxima;
        IARnet::Node::ServiceContext _context;
        Files::FileServerImpl _fs;
        Ice::LoggerPtr _log;
};

}

}

#endif // __MAXIMARESOURCEI_H__

