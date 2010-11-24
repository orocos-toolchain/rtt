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
#include "../Logger.hpp"

using namespace RTT;
using namespace detail;
using namespace std;


InputPortInterface::InputPortInterface(std::string const& name, ConnPolicy const& default_policy)
: PortInterface(name)
  , cmanager(this)
  , default_policy( default_policy )
  , new_data_on_port_event(0) {}

InputPortInterface::~InputPortInterface()
{
    cmanager.disconnect();
    if ( new_data_on_port_event) {
        new_data_on_port_event->disconnect();
        delete new_data_on_port_event;
    }
}

ConnPolicy InputPortInterface::getDefaultPolicy() const
{ return default_policy; }

InputPortInterface::NewDataOnPortEvent* InputPortInterface::getNewDataOnPortEvent()
{
    if (!new_data_on_port_event)
        new_data_on_port_event = new NewDataOnPortEvent();
    return new_data_on_port_event;
}

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
    cmanager.addConnection( port_id, channel_output);
    return true;
}

bool InputPortInterface::channelReady(ChannelElementBase::shared_ptr channel)
{
    if ( channel && channel->inputReady() )
        return true;
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

FlowStatus InputPortInterface::read(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default InputPortInterface::read(datasource) implementation"); }
/** Returns true if this port is connected */
bool InputPortInterface::connected() const
{ return cmanager.connected(); }

void InputPortInterface::clear()
{
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

