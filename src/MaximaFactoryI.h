#ifndef __MAXIMAFACTORYI_H__
#define __MAXIMAFACTORYI_H__

#include <IARnet/ServiceContext.h>
#include "Maxima.h"
#include <IARnet/Service.h>
#include "FileServerImpl.h"

namespace IARnet
{

namespace Maxima
{

class ICE_DECLSPEC_EXPORT MaximaFactoryI : virtual public MaximaFactory,
    virtual public IARnet::Node::Service
{
	public:
		MaximaFactoryI(const IARnet::Node::ServiceContext &context);
		~MaximaFactoryI();
		
        virtual void serviceInit(const std::string &proxy);
        
		virtual MaximaResourcePrx launchMaxima(const ::Ice::Current &);
        
        virtual void putFile(const std::string &fileName,
            const Files::FileContent &file, const Ice::Current &);
    
        virtual Files::FileContent getFile(const std::string &fileName,
            const Ice::Current &);
        
        virtual Files::SeqOfFS dispatch(const std::string &fileName,
            const Files::SeqOfFS &servers, const Ice::Current &);
        
    private:
        IARnet::Node::ServiceContext _context;
        Files::FileServerImpl _fs;
        MasterFactoryPrx _master;
        MaximaFactoryPrx _proxy;
        size_t _maximaCount;
};

}

}

#endif // __MAXIMAFACTORYI_H__

