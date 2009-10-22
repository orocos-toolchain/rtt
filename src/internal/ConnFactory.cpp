/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnFactory.cpp

                        ConnFactory.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "../Port.hpp"
#include "ConnFactory.hpp"
#include "../base/InputPortInterface.hpp"
#include "../interface/DataFlowInterface.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::internal;

bool LocalConnID::isSameID(ConnID const& id) const
{
    LocalConnID const* real_id = dynamic_cast<LocalConnID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->ptr == this->ptr;
}

ConnID* LocalConnID::clone() const {
    return new LocalConnID(this->ptr);
}

bool StreamConnID::isSameID(ConnID const& id) const
{
    StreamConnID const* real_id = dynamic_cast<StreamConnID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->name_id == this->name_id;
}

ConnID* StreamConnID::clone() const {
    return new StreamConnID(this->name_id);
}

base::ChannelElementBase::shared_ptr RTT::internal::ConnFactory::createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, const ConnPolicy& policy)
{
    // Remote connection
    // if the policy's transport is set to zero, use the input ports server protocol,
    // otherwise, use the policy's protocol
    int transport = policy.transport == 0 ? input_port.serverProtocol() : policy.transport;
    types::TypeInfo const* type_info = output_port.getTypeInfo();
    if (!type_info || input_port.getTypeInfo() != type_info)
    {
        log(Error) << "Type of port " << output_port.getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
        // There is no type info registered for this type
        return false;
    }
    else if ( !type_info->getProtocol( transport ) )
    {
        log(Error) << "Type " << type_info->getTypeName() << " cannot be marshalled into the requested transporter (id:"<< transport<<")." << endlog();
        // This type cannot be marshalled into the right transporter
        return false;
    }
    else
    {
        assert( input_port.getConnFactory() );
        return input_port.
                getConnFactory()->buildRemoteChannelOutput(output_port, type_info, input_port, policy);
    }
    return 0;
}
