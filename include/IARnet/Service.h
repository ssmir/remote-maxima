#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <string>
#include "SOExportDefs.h"

namespace IARnet
{
    
namespace Node
{

class NODEAPI Service
{
public:
    virtual ~Service();
    virtual void serviceInit(const std::string &proxy) = 0;
};

}

}

#endif // __SERVICE_H__

