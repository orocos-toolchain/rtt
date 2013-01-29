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

base::ChannelElementBase::shared_ptr ConnFactory::createLocalConnection(base::OutputPortInterface& output_port,
		base::InputPortInterface& input_port, const ConnPolicy& policy)
{
	// If input and output types are different
	if (output_port.getTypeInfo() != input_port.getTypeInfo()) {
		// check that a conversion is possible from A to B
		if (! input_port.getTypeInfo()->isConvertible(output_port.getTypeInfo())) {
			// No possible conversion! Connection cannot be created
			Logger::log(Logger::Error) << "No possible conversion from "
					<< output_port.getTypeInfo() << " to " << input_port.getTypeInfo()
					<< ". Port connection not possible!" << endlog();
			return base::ChannelElementBase::shared_ptr();
		}
	}

	// ChannelConversionElementOut<T_Out> -- ConnOutputEndPoint<T_Out> --> InputPort<T_Out>
    base::ChannelElementBase::shared_ptr endpoint = input_port.buildLocalChannel(output_port, base::ChannelElementBase::shared_ptr(), policy);
    // ChannelDataElement<T_In> -- ChannelConversionElementIn<T_In> --> ChannelConversionElementOut
    return output_port.buildLocalChannel(input_port, endpoint, policy);
	//return buildBufferedChannelOutput<T_In>(input_port, output_port.getPortID(), policy);
}

base::ChannelElementBase::shared_ptr ConnFactory::createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, const ConnPolicy& policy)
{
    // Remote connection
    // if the policy's transport is set to zero, use the input ports server protocol,
    // otherwise, use the policy's protocol
    int transport = policy.transport == 0 ? input_port.serverProtocol() : policy.transport;
    // input type of the connection
    types::TypeInfo const* type_in = output_port.getTypeInfo();
    if (!type_in) {
        // There is no type info registered for this type
        log(Error) << "Type of port " << output_port.getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
        return base::ChannelElementBase::shared_ptr();
    }
    // output type of the connection
    types::TypeInfo const* type_out = input_port.getTypeInfo();
    if (!type_out) {
        // There is no type info registered for this type
    	log(Error) << "Type of port " << input_port.getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
    	return base::ChannelElementBase::shared_ptr();
    }


    if ( !type_in->getProtocol( transport ) )
    {
        log(Error) << "Type " << type_in->getTypeName() << " cannot be marshalled into the requested transporter (id:"<< transport<<")." << endlog();
        // This type cannot be marshalled into the right transporter
        return base::ChannelElementBase::shared_ptr();
    }
    else
    {
        //return input_port.buildRemoteChannelOutput(output_port, type_info, input_port, policy);
    	return input_port.buildRemoteChannel(output_port, NULL);
    }
    return base::ChannelElementBase::shared_ptr();
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
        log(Debug) << "Connected output port "<< output_port.getName()
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
	const types::TypeInfo* type = output_port.getTypeInfo();
	if ( ! type->hasAnyProtocol(policy.transport) ) {
		log(Error) << "Could not create transport stream for port "<< output_port.getName() << " with transport id " << policy.transport <<endlog();
		log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
		return false;
	}

	RTT::base::ChannelElementBase::shared_ptr out_half;
	if (type->hasProtocol(policy.transport)) {
    	types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*> ( type->getProtocol(policy.transport) );
    	if (ttt) {
    		int size_hint = ttt->getSampleSize( output_port.getDataSource() );
    		policy.data_size = size_hint;
    	} else {
    		log(Debug) << "Could not determine sample size for type " << type->getTypeName() << endlog();
    	}
		out_half = type->getProtocol(policy.transport)->createStream(&output_port, policy, true);
	}

	// if conversion needed, add a pair of ChannelConversionElements
	else if (type->hasAnyProtocol(policy.transport)) {
		// 1. get the type and transporter to convert into
		types::TypeInfo* type_out = 0;
		types::TypeTransporter* tt_out = 0;
		std::vector<std::string> type_vector = types::Types()->getTypes();
		for (std::vector<std::string>::iterator i = type_vector.begin(); i != type_vector.end(); i++) {
			type_out = types::Types()->type(*i);
			if (type_out && type_out->isConvertible(type) && type_out->hasProtocol(policy.transport)) {
				log(Debug) << "Found a conversion from type " << type->getTypeName() << " to type " << *i
						<< " that supports transport " << policy.transport << endlog();
				tt_out = type_out->getProtocol(policy.transport);
				break;
			}
		}
		if (type_out == 0) {
			log(Error) << "Unable to find a conversion for type " << type->getTypeName()
        		<< " that provides a transporter for protocol " << policy.transport << endlog();
			return false;
		}
		// 1'/ determine sample size
    	types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*> ( type_out->getProtocol(policy.transport) );
    	if (ttt) {
    		int size_hint = ttt->getSampleSize( type_out->outputPort("temp")->getDataSource() );
    		policy.data_size = size_hint;
    	} else {
    		log(Debug) << "Could not determine sample size for type " << type_out->getTypeName() << endlog();
    	}

		// 2. from the end point, constructs the channel stream
		RTT::base::ChannelElementBase::shared_ptr chan_stream = tt_out->createStream(&output_port, policy, true);
		// 3. now constructs the ChannelConversionElementOut<T_Out>
		base::PortInterface* input_port = type_out->inputPort("temp");
		base::ChannelElementBase::shared_ptr cceo = input_port->buildRemoteChannel(output_port, NULL);
		cceo->setOutput(chan_stream);
		// 4. now constructs the ChannelConversionElementIn<T_In>
    	out_half = output_port.buildRemoteChannel(*input_port, cceo);
		delete input_port;
	}

	if ( !out_half ) {
		log(Error) << "Transport failed to create remote channel for output stream of port "<<output_port.getName() << endlog();
		return false;
	}

    chan->setOutput( out_half );
    if ( output_port.addConnection( new StreamConnID(policy.name_id), chan, policy) ) {
        log(Info) << "Created output stream for output port "<< output_port.getName() <<endlog();
        return true;
    }
    // setup failed.
    log(Error) << "Failed to create output stream for output port "<< output_port.getName() <<endlog();
    chan->disconnect(true);
    return false;
}

bool ConnFactory::createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const & policy, base::ChannelElementBase::shared_ptr outhalf, StreamConnID* sid) {
	const types::TypeInfo* type = input_port.getTypeInfo();
	RTT::base::ChannelElementBase::shared_ptr out_half, chan_stream;
	if (type->hasProtocol(policy.transport))
		chan_stream = type->getProtocol(policy.transport)->createStream(&input_port, policy, false);

	// if conversion needed, add a pair of ChannelConversionElements
	else {
		// 1. get the type and transporter to convert into
		types::TypeInfo* type_in = 0;
		types::TypeTransporter* tt_in;
		std::vector<std::string> type_vector = types::Types()->getTypes();
		for (std::vector<std::string>::iterator i = type_vector.begin(); i != type_vector.end(); i++) {
			type_in = types::Types()->type(*i);
			if (type_in && type->isConvertible(type_in) && type_in->hasProtocol(policy.transport)) {
				log(Debug) << "Found a conversion from type " << type_in->getTypeName() << " to type " << type->getTypeName()
	        		<< " that supports transport " << policy.transport << endlog();
				tt_in = type_in->getProtocol(policy.transport);
				break;
			}
		}
		if (type_in == 0) {
			log(Error) << "Unable to find a conversion for type " << type->getTypeName()
				<< " that provides a transporter for protocol " << policy.transport << endlog();
			return false;
		}

		base::OutputPortInterface* output_port = type_in->outputPort("temp");
		// 2. constructs the channel stream
		chan_stream = tt_in->createStream(&input_port, policy, false);
		// 3. now constructs the ChannelConversionElementOut<T_Out>
		base::ChannelElementBase::shared_ptr cceo = input_port.buildLocalChannel(*output_port, base::ChannelElementBase::shared_ptr(), policy);
		// 4. now constructs the ChannelConversionElementIn<T_In>
		//base::ChannelElementBase::shared_ptr data_storage = output_port->buildRemoteChannel(input_port, cceo);
		base::ChannelElementBase::shared_ptr data_storage = output_port->buildLocalChannel(input_port, cceo, policy);
		chan_stream->setOutput(data_storage);
		delete output_port;
	}

	if ( !chan_stream ) {
		log(Error) << "Transport failed to create remote channel for input stream of port "<<input_port.getName() << endlog();
		log(Error) << "Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
		return false;
	}

	// In stream mode, a buffer is always installed at input side.
	/* unused?
	ConnPolicy policy2 = policy;
	policy2.pull = false;
	// pass new name upwards.
	policy.name_id = policy2.name_id;
	sid->name_id = policy2.name_id;
	*/
	sid->name_id = policy.name_id;

	chan_stream->getOutputEndPoint()->setOutput( outhalf );
	if ( input_port.channelReady( outhalf ) == true ) {
		log(Info) << "Created input stream for input port "<< input_port.getName() <<endlog();
		return true;
	}
	// setup failed: manual cleanup.
	outhalf = 0; // deleted by channelReady() above !
	log(Error) << "Failed to create input stream for input port "<< input_port.getName() <<endlog();
	input_port.removeConnection(sid);
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

    // check if marshaller supports size hints:
    types::TypeMarshaller* ttt = dynamic_cast<types::TypeMarshaller*>( type->getProtocol(policy.transport) );
    if (ttt) {
        policy2.data_size = ttt->getSampleSize(  output_port.getDataSource() );
    } else {
        log(Debug) <<"Could not determine sample size for type " << type->getTypeName() << endlog();
    }
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


