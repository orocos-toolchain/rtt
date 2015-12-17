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
#include "../DataFlowInterface.hpp"
#include "../types/TypeMarshaller.hpp"

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

base::ChannelElementBase::shared_ptr RTT::internal::ConnFactory::buildRemoteChannelOutput(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, const ConnPolicy& policy)
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
        return base::ChannelElementBase::shared_ptr();
    }
    else if ( !type_info->getProtocol( transport ) )
    {
        log(Error) << "Type " << type_info->getTypeName() << " cannot be marshalled into the requested transporter (id:"<< transport<<")." << endlog();
        // This type cannot be marshalled into the right transporter
        return base::ChannelElementBase::shared_ptr();
    }
    else
    {
        return input_port.
                buildRemoteChannelOutput(output_port, type_info, input_port, policy);
    }
    return base::ChannelElementBase::shared_ptr();
}

bool ConnFactory::createAndCheckConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, base::ChannelElementBase::shared_ptr channel_input, base::ChannelElementBase::shared_ptr channel_output, ConnPolicy const& policy) {
    // connect channel input to channel output
    if (!channel_input->connectTo(channel_output, policy.mandatory)) {
        channel_input->disconnect(channel_output, true);
        channel_output->disconnect(channel_input, false);
        return false;
    }

    // Register the channel's input to the output port.
    // This is a bit hacky. We have to find the next channel element in the pipeline as seen from the ConnOutputEndpoint:
    base::ChannelElementBase::shared_ptr next_hop = channel_output;
    if (channel_input != output_port.getEndpoint()) {
        next_hop = channel_input;
        while(next_hop->getInput() && next_hop->getInput() != output_port.getEndpoint()) {
            next_hop = next_hop->getInput();
        }
    }
    if ( !output_port.addConnection( input_port.getPortID(), next_hop, policy ) ) {
        // setup failed.
        log(Error) << "The output port "<< output_port.getName()
                   << " could not successfully use the connection to input port " << input_port.getName() <<endlog();
        channel_input->disconnect(channel_output, true);
        return false;
    }

    // Notify input that the connection is now complete and test the connection
    if ( !channel_output->channelReady( channel_input, policy, output_port.getPortID() ) ) {
        log(Error) << "The input port "<< input_port.getName()
                   << " could not successfully read from the connection from output port " << output_port.getName() <<endlog();
        output_port.disconnect( &input_port );
        channel_output->disconnect(channel_input, false);
        return false;
    }

    log(Debug) << "Connected output port "<< output_port.getName()
              << " successfully to " << input_port.getName() <<endlog();
    return true;
}

base::ChannelElementBase::shared_ptr ConnFactory::createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr channel_input, StreamConnID* conn_id) {
    if (policy.transport == 0 ) {
        log(Error) << "Need a transport for creating streams." <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }
    const types::TypeInfo* type = output_port.getTypeInfo();
    if ( type->getProtocol(policy.transport) == 0 ) {
        log(Error) << "Could not create transport stream for port "<< output_port.getName() << " with transport id " << policy.transport <<endlog();
        log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }
    types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*> ( type->getProtocol(policy.transport) );
    if (ttt) {
        int size_hint = ttt->getSampleSize( output_port.getDataSource() );
        policy.data_size = size_hint;
    } else {
        log(Debug) <<"Could not determine sample size for type " << type->getTypeName() << endlog();
    }
    RTT::base::ChannelElementBase::shared_ptr chan_stream = type->getProtocol(policy.transport)->createStream(&output_port, policy, /* is_sender = */ true);
            
    if ( !chan_stream ) {
        log(Error) << "Transport failed to create remote channel for output stream of port "<<output_port.getName() << endlog();
        return base::ChannelElementBase::shared_ptr();
    }

    conn_id->name_id = policy.name_id;
    channel_input->connectTo( chan_stream, policy.mandatory );

    if ( !output_port.addConnection( conn_id, chan_stream, policy ) ) {
        // setup failed: manual cleanup.
        channel_input->disconnect( chan_stream, true );
        log(Error) << "Failed to create output stream for output port "<< output_port.getName() <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }

    log(Info) << "Created output stream for output port "<< output_port.getName() <<endlog();
    return chan_stream;
}

base::ChannelElementBase::shared_ptr ConnFactory::createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr outhalf, StreamConnID* conn_id) {
    if (policy.transport == 0 ) {
        log(Error) << "Need a transport for creating streams." <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }
    const types::TypeInfo* type = input_port.getTypeInfo();
    if ( type->getProtocol(policy.transport) == 0 ) {
        log(Error) << "Could not create transport stream for port "<< input_port.getName() << " with transport id " << policy.transport <<endlog();
        log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }

    // note: don't refcount this final input chan, because no one will
    // take a reference to it. It would be destroyed upon return of this function.
    RTT::base::ChannelElementBase::shared_ptr chan = type->getProtocol(policy.transport)->createStream(&input_port, policy, /* is_sender = */ false);

    if ( !chan ) {
        log(Error) << "Transport failed to create remote channel for input stream of port " << input_port.getName() << endlog();
        return base::ChannelElementBase::shared_ptr();
    }

    chan = chan->getOutputEndPoint();
    conn_id->name_id = policy.name_id;

    chan->connectTo( outhalf, policy.mandatory );
    if ( !outhalf->channelReady(chan, policy, conn_id) ) {
        // setup failed: manual cleanup.
        chan->disconnect(true);
        log(Error) << "Failed to create input stream for input port " << input_port.getName() <<endlog();
        return base::ChannelElementBase::shared_ptr();
    }

    log(Info) << "Created input stream for input port " << input_port.getName() <<endlog();
    return chan;
}

bool ConnFactory::createAndCheckSharedConnection(base::OutputPortInterface* output_port, base::InputPortInterface* input_port, SharedConnectionBase::shared_ptr shared_connection, ConnPolicy const& policy)
{
    if (!shared_connection) return false;

    // check if the found connection is compatible to the requested policy
    if (
        (policy.buffer_policy != Shared) ||
        (shared_connection->getConnPolicy()->type != policy.type) ||
        (shared_connection->getConnPolicy()->size != policy.size) ||
        (shared_connection->getConnPolicy()->lock_policy != policy.lock_policy)
       )
    {
        log(Error) << "You mixed incompatible connection policies for shared connection '" << shared_connection->getName() << "': "
                   << "The new connection requests a " << policy << " connection, "
                   << "but the existing connection is of type " << *(shared_connection->getConnPolicy()) << "." << endlog();
        return false;
    }

    // set name_id in ConnPolicy (mutable field)
    policy.name_id = shared_connection->getName();

    // connect the output port...
    if (output_port && output_port->getSharedConnection() != shared_connection) {
        if ( !output_port->addConnection( shared_connection->getConnID(), shared_connection, policy ) ) {
            // setup failed.
            log(Error) << "The output port "<< output_port->getName()
                       << " could not successfully connect to shared connection '" << shared_connection->getName() << "'." << endlog();
            return false;
        }

        output_port->getEndpoint()->connectTo(shared_connection, policy.mandatory);
    }

    // ... and the input port
    if (input_port && input_port->isLocal() && input_port->getSharedConnection() != shared_connection) {
        if ( !input_port->addConnection( shared_connection->getConnID(), shared_connection, policy ) ) {
            // setup failed.
            log(Error) << "The input port "<< input_port->getName()
                       << " could not successfully connect to shared connection '" << shared_connection->getName() << "'." << endlog();
            return false;
        }

        shared_connection->connectTo(input_port->getEndpoint(), policy.mandatory);
    }

    return true;
}

bool ConnFactory::findSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy, SharedConnectionBase::shared_ptr &shared_connection)
{
    shared_connection.reset();

    if (output_port) {
        shared_connection = output_port->getSharedConnection();
    }

    if (input_port) {
        if (!shared_connection) {
            shared_connection = input_port->getSharedConnection();
        } else {
            assert(output_port); // must be set if shared_connection has been set before

            // For the case both, the output and the input port already have shared connections, check if it matches the one of the input port:
            SharedConnectionBase::shared_ptr input_ports_shared_connection = input_port->getSharedConnection();
            if (shared_connection == input_ports_shared_connection) {
                RTT::log(RTT::Info) << "Output port '" << output_port->getName() << "' and input port '" << input_port->getName() << "' are already connected to the same shared connection." << RTT::endlog();
                // return SharedConnectionBase::shared_ptr();
            } else if (input_ports_shared_connection) {
                RTT::log(RTT::Error) << "Output port '" << output_port->getName() << "' and input port '" << input_port->getName() << "' are already connected to different shared connections!" << RTT::endlog();
                shared_connection.reset();
                return true;
            }
        }
    }

    if (!policy.name_id.empty()) {
        if (!shared_connection) {
            // lookup shared connection by the given name
            shared_connection = SharedConnectionRepository::Instance()->get(policy.name_id);
        } else if (shared_connection->getName() != policy.name_id) {
            RTT::log(RTT::Error) << "At least one of the given ports is already connected to shared connection '" << shared_connection->getName() << "' but you requested to connect to '" << policy.name_id << "'!" << RTT::endlog();
            shared_connection.reset();
            return true;
        }
    }

    return bool(shared_connection);
}
