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

bool ConnFactory::createAndCheckConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, base::ChannelElementBase::shared_ptr channel_input, ConnPolicy policy) {
    // Register the channel's input to the output port.
    if ( output_port.addConnection( input_port.getPortID(), channel_input, policy ) ) {
        // notify input that the connection is now complete.
        if ( input_port.channelReady( channel_input->getOutputEndPoint() ) == false ) {
            output_port.disconnect( &input_port );
            log(Error) << "The input port "<< input_port.getName()
                       << " could not successfully read from the connection from output port " << output_port.getName() <<endlog();

            return false;
        }
        log(Info) << "Connected output port "<< output_port.getName()
                  << " successfully to " << input_port.getName() <<endlog();
        return true;
    }
    // setup failed.
    channel_input->disconnect(true);
    log(Error) << "The output port "<< output_port.getName()
               << " could not successfully use the connection to input port " << input_port.getName() <<endlog();
    return false;
}

bool ConnFactory::createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr chan, StreamConnID* conn_id) {
    if (policy.transport == 0 ) {
        log(Error) << "Need a transport for creating streams." <<endlog();
        return false;
    }
    const types::TypeInfo* type = output_port.getTypeInfo();
    if ( type->getProtocol(policy.transport) == 0 ) {
        log(Error) << "Could not create transport stream for port "<< output_port.getName() << " with transport id " << policy.transport <<endlog();
        log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        return false;
    }
    types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*> ( type->getProtocol(policy.transport) );
    if (ttt) {
        int size_hint = ttt->getSampleSize( output_port.getDataSource() );
        policy.data_size = size_hint;
    } else {
        log(Warning) <<"Could not determine sample size for type " << type->getTypeName() << endlog();
    }
    RTT::base::ChannelElementBase::shared_ptr chan_stream = type->getProtocol(policy.transport)->createStream(&output_port, policy, true);
            
    if ( !chan_stream ) {
        log(Error) << "Transport failed to create remote channel for output stream of port "<<output_port.getName() << endlog();
        return false;
    }
    chan->setOutput( chan_stream );

    if ( output_port.addConnection( new StreamConnID(policy.name_id), chan, policy) ) {
        log(Info) << "Created output stream for output port "<< output_port.getName() <<endlog();
        return true;
    }
    // setup failed.
    log(Error) << "Failed to create output stream for output port "<< output_port.getName() <<endlog();
    return false;
}

bool ConnFactory::createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr outhalf, StreamConnID* conn_id) {
    if (policy.transport == 0 ) {
        log(Error) << "Need a transport for creating streams." <<endlog();
        return false;
    }
    const types::TypeInfo* type = input_port.getTypeInfo();
    if ( type->getProtocol(policy.transport) == 0 ) {
        log(Error) << "Could not create transport stream for port "<< input_port.getName() << " with transport id " << policy.transport <<endlog();
        log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        return false;
    }

    // note: don't refcount this final input chan, because no one will
    // take a reference to it. It would be destroyed upon return of this function.
    RTT::base::ChannelElementBase* chan = type->getProtocol(policy.transport)->createStream(&input_port,policy, false);

    if ( !chan ) {
        log(Error) << "Transport failed to create remote channel for input stream of port "<<input_port.getName() << endlog();
        return false;
    }

    // In stream mode, a buffer is always installed at input side.
    //
    ConnPolicy policy2 = policy;
    policy2.pull = false;
    // pass new name upwards.
    policy.name_id = policy2.name_id;
    conn_id->name_id = policy2.name_id;

    chan->getOutputEndPoint()->setOutput( outhalf );
    if ( input_port.channelReady( chan->getOutputEndPoint() ) == true ) {
        log(Info) << "Created input stream for input port "<< input_port.getName() <<endlog();
        return true;
    }
    // setup failed: manual cleanup.
    chan = 0; // deleted by channelReady() above !
    log(Error) << "Failed to create input stream for input port "<< input_port.getName() <<endlog();
    return false;
}

base::ChannelElementBase::shared_ptr ConnFactory::createAndCheckOutOfBandConnection( base::OutputPortInterface& output_port, 
                                                                                     base::InputPortInterface& input_port, 
                                                                                     ConnPolicy const& policy, 
                                                                                     base::ChannelElementBase::shared_ptr output_half, 
                                                                                     StreamConnID* conn_id) 
{
    // create input half using a transport.
    const types::TypeInfo* type = output_port.getTypeInfo();
    if ( type->getProtocol(policy.transport) == 0 ) {
        log(Error) << "Could not create out-of-band transport for port "<< output_port.getName() << " with transport id " << policy.transport <<endlog();
        log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        return 0;
    }

    // we force the creation of a buffer on input side
    ConnPolicy policy2 = policy;
    policy2.pull = false;
    conn_id->name_id = policy2.name_id;

    types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*>( type->getProtocol(policy.transport) );
    int size_hint = ttt->getSampleSize(  output_port.getDataSource() );
    policy2.data_size = size_hint;
    // XXX: this seems to be always true
    if ( input_port.isLocal() ) {
        RTT::base::ChannelElementBase::shared_ptr ceb_input = type->getProtocol(policy.transport)->createStream(&input_port, policy2, false);
        if (ceb_input) {
            log(Info) <<"Receiving data for port "<<input_port.getName() << " from out-of-band protocol "<< policy.transport << " with id "<< policy2.name_id<<endlog();
        } else {
            log(Error) << "The type transporter for type "<<type->getTypeName()<< " failed to create a remote channel for port " << input_port.getName()<<endlog();
            return 0;
        }
        ceb_input->getOutputEndPoint()->setOutput(output_half);
        output_half = ceb_input;
    }

    // XXX: this seems to be always true
    if ( output_port.isLocal() ) {

        RTT::base::ChannelElementBase::shared_ptr ceb_output = type->getProtocol(policy.transport)->createStream(&output_port, policy2, true);
        if (ceb_output) {
            log(Info) <<"Redirecting data for port "<< output_port.getName() << " to out-of-band protocol "<< policy.transport << " with id "<< policy2.name_id <<endlog();
        } else {
            log(Error) << "The type transporter for type "<<type->getTypeName()<< " failed to create a remote channel for port " << output_port.getName()<<endlog();
            return 0;
        }
        // this mediates the 'channel ready leads to initial data sample'.
        // it is probably not necessary, since streams don't assume this relation.
        ceb_output->getOutputEndPoint()->setOutput(output_half);
        output_half = ceb_output;
    }
    // Important ! since we made a copy above, we need to set the original to the changed name_id.
    policy.name_id = policy2.name_id;
    conn_id->name_id = policy2.name_id;

    return output_half;

}

