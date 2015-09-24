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
#include "../internal/ConnFactory.hpp"
#include "../Logger.hpp"
#include <exception>
#include <stdexcept>

#ifdef RTT_V2_DATAFLOW_COMPATIBILITY_MODE
# include "../TaskContext.hpp"
#endif

using namespace RTT;
using namespace RTT::detail;
using namespace std;


InputPortInterface::InputPortInterface(std::string const& name, ConnPolicy const& default_policy)
: PortInterface(name)
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

#ifdef RTT_V2_DATAFLOW_COMPATIBILITY_MODE
static std::string toString(const ConnID *conn_id)
{
    const internal::LocalConnID *local_conn_id = dynamic_cast<const internal::LocalConnID *>(conn_id);
    if (local_conn_id) {
        std::string port_name = local_conn_id->ptr->getName();
        if (local_conn_id->ptr->getInterface() && local_conn_id->ptr->getInterface()->getOwner()) {
            port_name = local_conn_id->ptr->getInterface()->getOwner()->getName() + "." + port_name;
        }
        return "from port " + port_name;
    }

    const internal::StreamConnID *stream_conn_id = dynamic_cast<const internal::StreamConnID *>(conn_id);
    if (stream_conn_id) {
        return "from stream " + stream_conn_id->name_id;
    }

    return "from unknown source";
}
#endif

bool InputPortInterface::addConnection(ConnID* cid, ChannelElementBase::shared_ptr channel, const ConnPolicy& policy)
{
#ifdef RTT_V2_DATAFLOW_COMPATIBILITY_MODE
    // Log errors for incompatible connection policies if RTT_V2_DATAFLOW_COMPATIBILITY_MODE is defined:
    internal::ConnectionManager::Connections connections = cmanager.getConnections();
    for(internal::ConnectionManager::Connections::const_iterator connection = connections.begin(); connection != connections.end(); ++connection) {
        const ConnID *other_cid = connection->get<0>().get();
        const ConnPolicy &other = connection->get<2>();

        // check if push/pull connections have been mixed
        if (policy.pull != other.pull) {
            log(Error) << "[RTT_V2_COMPATIBILITY_MODE] You mixed push and pull connections to input port " << getName() << ":" << nlog()
                       << "[RTT_V2_COMPATIBILITY_MODE]   The new connection " << toString(cid) << " with policy " << policy.toString() << nlog()
                       << "[RTT_V2_COMPATIBILITY_MODE]   conflicts with connection " << toString(other_cid) << " with policy " << other.toString() << endlog();

        // check if buffer policies match (push connections only)
        } else if (policy.pull == ConnPolicy::PUSH) {
            if ((policy.type != other.type) ||
                (policy.lock_policy != other.lock_policy) ||
                (policy.size != other.size)) {
                log(Error) << "[RTT_V2_COMPATIBILITY_MODE] You mixed push connections to input port " << getName() << "  with different buffer policies:" << nlog()
                           << "[RTT_V2_COMPATIBILITY_MODE]   The new connection " << toString(cid) << " with policy " << policy.toString() << nlog()
                           << "[RTT_V2_COMPATIBILITY_MODE]   conflicts with connection " << toString(other_cid) << " with policy " << other.toString() << endlog();
            }
        }
    }
#endif

    // input ports don't check the connection policy.
    return cmanager.addConnection( cid, channel, policy);
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

void InputPortInterface::disconnect()
{
    cmanager.disconnect();
}

bool InputPortInterface::disconnect(PortInterface* port)
{
    return cmanager.disconnect(port);
}

bool InputPortInterface::createConnection( internal::SharedConnectionBase::shared_ptr shared_connection, ConnPolicy const& policy )
{
    return internal::ConnFactory::createAndCheckSharedConnection(0, this, shared_connection, policy);
}

base::ChannelElementBase::shared_ptr InputPortInterface::buildRemoteChannelOutput(
                base::OutputPortInterface& output_port,
                types::TypeInfo const* type_info,
                base::InputPortInterface& input, const ConnPolicy& policy)
{
    return base::ChannelElementBase::shared_ptr();
}
