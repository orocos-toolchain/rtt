/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  InputPortInterface.cpp

                        InputPortInterface.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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


#include "PortInterface.hpp"
#include "InputPortInterface.hpp"
#include "OutputPortInterface.hpp"
#include "DataFlowInterface.hpp"
#include "../internal/ConnInputEndPoint.hpp"
#include "../Logger.hpp"
#include <exception>
#include <stdexcept>
#include <../os/traces.h>

using namespace RTT;
using namespace RTT::detail;
using namespace std;


InputPortInterface::InputPortInterface(std::string const& name, ConnPolicy const& default_policy)
: PortInterface(name)
  , cmanager(this)
  , default_policy( default_policy )
#ifdef ORO_SIGNALLING_PORTS
  , new_data_on_port_event(0)
#else
 , msignal_interface(false)
#endif
{}

InputPortInterface::~InputPortInterface()
{
    cmanager.disconnect();
#ifdef ORO_SIGNALLING_PORTS
    if ( new_data_on_port_event) {
        delete new_data_on_port_event;
    }
#endif
}

ConnPolicy InputPortInterface::getDefaultPolicy() const
{ return default_policy; }

#ifdef ORO_SIGNALLING_PORTS
InputPortInterface::NewDataOnPortEvent* InputPortInterface::getNewDataOnPortEvent()
{
    if (!new_data_on_port_event)
        new_data_on_port_event = new NewDataOnPortEvent();
    return new_data_on_port_event;
}
#endif
bool InputPortInterface::connectTo(PortInterface* other, ConnPolicy const& policy)
{
    OutputPortInterface* output = dynamic_cast<OutputPortInterface*>(other);
    if (! output) {
        log(Error) << "InputPort "<< getName() <<" could not connect to "<< other->getName() << ": not an Output port." <<endlog();
        return false;
    }
    return output->createConnection(*this, policy);
}

bool InputPortInterface::connectTo(PortInterface* other)
{
    return connectTo(other, default_policy);
}

bool InputPortInterface::addConnection(ConnID* port_id, ChannelElementBase::shared_ptr channel_output, const ConnPolicy& policy)
{
    // input ports don't check the connection policy.
    cmanager.addConnection( port_id, channel_output, policy);
    return true;
}

bool InputPortInterface::channelReady(ChannelElementBase::shared_ptr channel, RTT::ConnPolicy const& policy)
{
    // cid is deleted/owned by the ConnectionManager.
    if ( channel ) {
        internal::ConnID* cid = channel->getConnID();
        if (cid ) {
            this->addConnection(cid, channel, policy);
            if ( channel->inputReady() )
                return true;
        } else {
            log(Error) << "Can't add ChannelElement which is not a ConnInputEndPoint to Port "<< this->getName() <<endlog();
        }            
    }
    if (channel) {
        // in-the-middle disconnection, we need to inform both ends of
        // the channel that it's going to be disposed. Both endpoints
        // will inform their ports with a removal request.
        // From a design perspective, this removal must be initiated
        // by our connection manager and not by us.
        channel->disconnect(false);
        channel->disconnect(true);
    }

    return false;
}

bool InputPortInterface::removeConnection(ConnID* conn)
{
    return cmanager.removeConnection(conn);
}

#ifndef ORO_SIGNALLING_PORTS
void InputPortInterface::signal()
{
    if (iface && msignal_interface)
        iface->dataOnPort(this);
}
void InputPortInterface::signalInterface(bool true_false)
{
    msignal_interface = true_false;
}
#endif
FlowStatus InputPortInterface::read(DataSourceBase::shared_ptr source, bool copy_old_data)
{ throw std::runtime_error("calling default InputPortInterface::read(datasource) implementation"); }
/** Returns true if this port is connected */
bool InputPortInterface::connected() const
{ return cmanager.connected(); }

void InputPortInterface::traceRead(RTT::FlowStatus status)
{
    tracepoint(orocos_rtt, InputPort_read, status, cFullName);
}

void InputPortInterface::clear()
{
    tracepoint(orocos_rtt, InputPort_clear, cFullName);
    cmanager.clear();
}

void InputPortInterface::disconnect()
{
    cmanager.disconnect();
}

bool InputPortInterface::disconnect(PortInterface* port)
{
    return cmanager.disconnect(port);
}

base::ChannelElementBase::shared_ptr InputPortInterface::buildRemoteChannelOutput(
                base::OutputPortInterface& output_port,
                types::TypeInfo const* type_info,
                base::InputPortInterface& input, const ConnPolicy& policy)
{
    return base::ChannelElementBase::shared_ptr();
}

