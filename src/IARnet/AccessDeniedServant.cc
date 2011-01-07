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

#include "AccessDeniedServant.h"
#include "Security.h"

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

