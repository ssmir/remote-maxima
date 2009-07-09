#ifndef __ACCESSDENIEDSERVANT_H__
#define __ACCESSDENIEDSERVANT_H__

#include "SOExportDefs.h"
#include <Ice/Object.h>

namespace IARnet
{
    
namespace Node
{

class NODEAPI AccessDeniedServant : public Ice::Blobject
{
public:
    virtual bool ice_invoke(const std::vector<Ice::Byte> &inParams,
        std::vector<Ice::Byte> &outParams, const Ice::Current &current);
};

}

}

#endif // __ACCESSDENIEDSERVANT_H__

