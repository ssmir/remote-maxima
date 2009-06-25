#include <IARnet/AccessDeniedServant.h>
#include <IARnet/Security.h>

#include <Ice/ObjectAdapter.h>
#include <Ice/Initialize.h>
#include <Ice/Stream.h>

using namespace IARnet::Node;

bool AccessDeniedServant::ice_invoke(const std::vector<Ice::Byte> &inParams,
    std::vector<Ice::Byte> &outParams, const Ice::Current &current)
{
    Ice::OutputStreamPtr out = Ice::createOutputStream(
        current.adapter->getCommunicator());
    out->writeException(IARnet::Security::AccessDeniedException());
    out->finished(outParams);
    return false;
}

