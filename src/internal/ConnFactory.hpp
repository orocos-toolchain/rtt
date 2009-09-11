#ifndef ORO_CONN_FACTORY_HPP
#define ORO_CONN_FACTORY_HPP

#include <string>
#include "Channels.hpp"
#include "ConnInputEndPoint.hpp"
#include "ConnOutputEndPoint.hpp"
#include "../base/PortInterface.hpp"
#include "../base/InputPortInterface.hpp"
#include "../base/OutputPortInterface.hpp"
#include "../interface/DataFlowInterface.hpp"

#include "../base/DataObject.hpp"
#include "../base/Buffer.hpp"

namespace RTT
{ namespace internal {

    /** This class provides the basic tools to create channels that represent
     * connections between two ports
     */
    class RTT_API ConnFactory
    {
    public:

        /** This method is analoguous to the static ConnFactory::buildOutputHalf.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the types::TypeInfo object
         */
        virtual base::ChannelElementBase* buildRemoteOutputHalf(types::TypeInfo const* type_info,
                base::InputPortInterface& output, const ConnPolicy& policy) = 0;

        /** This method creates the connection element that will store data
         * inside the connection, based on the given policy
         */
        template<typename T>
        static base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy)
        {
            if (policy.type == ConnPolicy::DATA)
            {
                base::DataObjectInterface<T>* data_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    data_object = new base::DataObjectLocked<T>();
                else
                    data_object = new base::DataObjectLockFree<T>();

                ChannelDataElement<T>* result = new ChannelDataElement<T>(data_object);
                data_object->deref(); // data objects are initialized with a refcount of 1
                return result;
            }
            else if (policy.type == ConnPolicy::BUFFER)
            {
                base::BufferInterface<T>* buffer_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    buffer_object = new base::BufferLocked<T>(policy.size);
                else
                    buffer_object = new base::BufferLockFree<T>(policy.size);

                return new ChannelBufferElement<T>(typename base::BufferInterface<T>::shared_ptr(buffer_object));
            }
            return NULL;
        }

        /** During the process of building a connection between two ports, this
         * method builds the input half (starting from the OutputPort). In the
         * returned pair, the first element is the connection element that
         * should be connected to the port, and the second element is the one
         * that will be connected to the output-half of the connection
         *
         * The \c output_channel argument is the connection element that has been returned
         * by buildOutputHalf.
         *
         * @see buildOutputHalf
         */
        template<typename T>
        static base::ChannelElementBase* buildInputHalf(OutputPort<T>& port, ConnPolicy const& policy, base::ChannelElementBase* output_channel)
        {
            base::ChannelElementBase* endpoint = new ConnInputEndpoint<T>(&port);
            if (policy.pull)
            {
                base::ChannelElementBase* data_object = buildDataStorage<T>(policy);
                endpoint->setOutput(data_object);
                data_object->setOutput(output_channel);
            }
            else
                endpoint->setOutput(output_channel);

            return endpoint;
        }

        /** During the process of building a connection between two ports, this
         * method builds the output part of the channel, that is the half that
         * is connected to the input port. The returned value is the connection
         * element that should be connected to the end of the input-half.
         *
         * @see buildInputHalf
         */
        template<typename T>
        static base::ChannelElementBase* buildOutputHalf(InputPort<T>& port, ConnPolicy const& policy)
        {
            base::ChannelElementBase* endpoint = new ConnOutputEndpoint<T>(&port);
            if (!policy.pull)
            {
                base::ChannelElementBase* data_object = buildDataStorage<T>(policy);
                data_object->setOutput(endpoint);
                return data_object;
            }
            else return endpoint;
        }

        /**
         * Creates a connection from a local output_port to a local or remote input_port.
         * This function contains all logic to decide on how connections must be created to
         * local or remote input ports.
         *
         * In order to set up out-of-band communication between input_port and output_port,
         * use a different transport number in the policy parameter than the transport of the input port.
         *
         */
        template<typename T>
        static bool createConnection(OutputPort<T>& output_port, base::InputPortInterface& input_port, internal::ConnPolicy const& policy)
        {
            if ( input_port.connected() ) {
                log(Error) << "Can not connect to connected InputPort." <<endlog();
                return false;
            }

            if ( !output_port.isLocal() ) {
                log(Error) << "Need a local OutputPort to create connections." <<endlog();
                return false;
            }

            InputPort<T>* input_p = dynamic_cast<InputPort<T>*>(&input_port);

            // This is the input channel element of the output half
            base::ChannelElementBase* output_half = 0;
            if (input_port.isLocal() && policy.transport == 0)
            {
                // Local connection
                if (!input_p)
                {
                    log(Error) << "Port " << input_port.getName() << " is not compatible with " << output_port.getName() << endlog();
                    return false;
                }

                output_half = ConnFactory::buildOutputHalf<T>(*input_p, policy);
            }
            else
            {
                if ( !input_port.isLocal() ) {
                    output_half = ConnFactory::createRemoteConnection( output_port, input_port, policy);
                } else
                    output_half = ConnFactory::createOutOfBandConnection<T>( output_port, *input_p, policy);
            }

            if (!output_half)
                return false;

            // Since output is local, buildInputHalf is local as well.
            // This this the input channel element of the whole connection
            base::ChannelElementBase::shared_ptr channel_input =
                ConnFactory::buildInputHalf<T>(output_port, policy, output_half);

            // Register the channel's input to the output port.
            output_port.addConnection( input_port.getPortID(), channel_input, policy );

            return true;
        }

        template<class T>
        static bool createStream(OutputPort<T>& output_port, internal::ConnPolicy const& policy)
        {
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
            RTT::base::ChannelElementBase* chan = type->getProtocol(policy.transport)->createChannel(&output_port, policy.name_id, 0, true);
            
            chan = buildInputHalf( output_port, policy, chan);

            output_port.addConnection( 0, chan, policy);
            return true;
        }

        template<class T>
        static bool createStream(InputPort<T>& input_port, internal::ConnPolicy const& policy)
        {
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
            RTT::base::ChannelElementBase* chan = type->getProtocol(policy.transport)->createChannel(&input_port,policy.name_id, 0, false);

            if ( !chan ) {
                log(Error) << "Transport failed to create remote channel for input stream of port "<<input_port.getName() << endlog();
                return false;
            }
            
            // In stream mode, a buffer is always installed at input side.
            //
            ConnPolicy policy2 = policy;
            policy2.pull = false;
            RTT::base::ChannelElementBase* outhalf = buildOutputHalf( input_port, policy2);

            chan->setOutput( outhalf );
            return true;
        }
    protected:

        static base::ChannelElementBase* createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, internal::ConnPolicy const& policy);

        template<class T>
        static base::ChannelElementBase* createOutOfBandConnection(OutputPort<T>& output_port, InputPort<T>& input_port, internal::ConnPolicy const& policy) {
            // create input half using a transport.
            const types::TypeInfo* type = output_port.getTypeInfo();
            if ( type->getProtocol(policy.transport) == 0 ) {
                log(Error) << "Could not create out-of-band transport for port "<< output_port.getName() << " with transport id " << policy.transport <<endlog();
                log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
            }

            // we force the creation of a buffer on input side
            ConnPolicy policy2 = policy;
            policy2.pull = false;

            RTT::base::ChannelElementBase* output_half = ConnFactory::buildOutputHalf<T>(input_port, policy2);

            if ( input_port.isLocal() ) {
                RTT::base::ChannelElementBase* ceb_input = type->getProtocol(policy.transport)->createChannel(&input_port,policy2.name_id, 0, false);
                if (ceb_input) {
                    log(Info) <<"Receiving data for port "<<input_port.getName() << " from out-of-band protocol "<< policy.transport << " with id "<< policy2.name_id<<endlog();
                } else {
                    log(Error) << "The type transporter for type "<<type->getTypeName()<< " failed to create a remote channel for port " << input_port.getName()<<endlog();
                }
                ceb_input->setOutput(output_half);
                output_half = ceb_input;
            }

            if ( output_port.isLocal() ) {

                RTT::base::ChannelElementBase* ceb_output = type->getProtocol(policy.transport)->createChannel(&output_port,policy2.name_id, 0, true);
                if (ceb_output) {
                    log(Info) <<"Redirecting data for port "<< output_port.getName() << " to out-of-band protocol "<< policy.transport << " with id "<< policy2.name_id <<endlog();
                } else {
                    log(Error) << "The type transporter for type "<<type->getTypeName()<< " failed to create a remote channel for port " << output_port.getName()<<endlog();
                }
                ceb_output->setOutput(output_half);
                output_half = ceb_output;
            }
            // Important ! since we made a copy above, we need to set the original to the changed name_id.
            policy.name_id = policy2.name_id;

            return output_half;

        }

    };

}}

#endif

