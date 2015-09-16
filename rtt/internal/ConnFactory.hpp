/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnFactory.hpp

                        ConnFactory.hpp -  description
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


#ifndef ORO_CONN_FACTORY_HPP
#define ORO_CONN_FACTORY_HPP

#include <string>
#include "Channels.hpp"
#include "ConnInputEndPoint.hpp"
#include "ConnOutputEndPoint.hpp"
#include "SharedConnection.hpp"
#include "../base/PortInterface.hpp"
#include "../base/InputPortInterface.hpp"
#include "../base/OutputPortInterface.hpp"
#include "../DataFlowInterface.hpp"

#include "../base/DataObject.hpp"
#include "../base/DataObjectUnSync.hpp"
#include "../base/Buffer.hpp"
#include "../base/BufferUnSync.hpp"
#include "../Logger.hpp"

namespace RTT
{ namespace internal {

    /**
     * Represents a local connection created by the ConnFactory.
     */
    struct LocalConnID : public ConnID
    {
        base::PortInterface const* ptr;
        LocalConnID(base::PortInterface const* obj)
            : ptr(obj) {}
        virtual ConnID* clone() const;
        virtual bool isSameID(ConnID const& id) const;
    };

    /**
     * Represents a Stream connection created by the ConnFactory.
     */
    struct RTT_API StreamConnID : public ConnID
    {
        std::string name_id;
        StreamConnID(const std::string& name)
            : name_id(name) {}
        virtual ConnID* clone() const;
        virtual bool isSameID(ConnID const& id) const;
    };


    /** This class provides the basic tools to create channels that represent
     * connections between two ports.
     *
     * The ports and type transports use these functions to setup connections.
     * The interface may change as the needs of these 'users' change.
     */
    class RTT_API ConnFactory
    {
    public:
        virtual ~ConnFactory() {}

        /**
         * Returns a new InputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        virtual base::InputPortInterface* inputPort(std::string const& name) const = 0;

        /**
         * Returns a new OutputPort<T> object where T is the type represented by
         * this TypeInfo object.
         */
        virtual base::OutputPortInterface* outputPort(std::string const& name) const = 0;

        /**
         * Creates single data or buffered storage for this type.
         *
         * @param policy Describes the kind of storage requested by the user
         * @return a storage element.
         */
        virtual base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const = 0;

        /**
         * Creates the output endpoint of a communication channel and adds it to an InputPort.
         *
         * @param port The input port to connect the channel's output end to.
         * @param policy Describes the kind of connection requested by the user
         * @return The created endpoint.
         */
        virtual base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port, ConnPolicy const& policy) const = 0;
        /**
         * Creates the input endpoint (starting point) of a communication channel and adds it to an OutputPort.
         *
         * @param port The output port to connect the channel's input end to.
         * @param policy Describes the kind of connection requested by the user
         * @return The created endpoint.
         */
        virtual base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port, ConnPolicy const& policy) const = 0;

        /** This method creates the connection element that will store data
         * inside the connection, based on the given policy
         * @todo: shouldn't this belong in the template type info ? This allows the type lib to
         * choose which locked/lockfree algorithms are implemented and leaves out 4x code generation
         * for each alternative in each compilation unit. Contra: needs T in typelib.
         * @todo: since setDataSample, initial_value is no longer needed.
         */
        template<typename T>
        static base::ChannelElement<T>* buildDataStorage(ConnPolicy const& policy, const T& initial_value = T())
        {
            if (policy.type == ConnPolicy::DATA)
            {
                typename base::DataObjectInterface<T>::shared_ptr data_object;
                switch (policy.lock_policy)
                {
#ifndef OROBLD_OS_NO_ASM
                case ConnPolicy::LOCK_FREE:
                    data_object.reset( new base::DataObjectLockFree<T>(initial_value) );
                    break;
#else
                case ConnPolicy::LOCK_FREE:
                    RTT::log(Warning) << "lock free connection policy is unavailable on this system, defaulting to LOCKED" << RTT::endlog();
#endif
                case ConnPolicy::LOCKED:
                    data_object.reset( new base::DataObjectLocked<T>(initial_value) );
                    break;
                case ConnPolicy::UNSYNC:
                    data_object.reset( new base::DataObjectUnSync<T>(initial_value) );
                    break;
                }
                return new ChannelDataElement<T>(data_object, policy);
            }
            else if (policy.type == ConnPolicy::BUFFER || policy.type == ConnPolicy::CIRCULAR_BUFFER)
            {
                base::BufferInterface<T>* buffer_object = 0;
                switch (policy.lock_policy)
                {
#ifndef OROBLD_OS_NO_ASM
                case ConnPolicy::LOCK_FREE:
                    buffer_object = new base::BufferLockFree<T>(policy.size, initial_value, policy.type == ConnPolicy::CIRCULAR_BUFFER);
                    break;
#else
                case ConnPolicy::LOCK_FREE:
                    RTT::log(Warning) << "lock free connection policy is unavailable on this system, defaulting to LOCKED" << RTT::endlog();
#endif
                case ConnPolicy::LOCKED:
                    buffer_object = new base::BufferLocked<T>(policy.size, initial_value, policy.type == ConnPolicy::CIRCULAR_BUFFER);
                    break;
                case ConnPolicy::UNSYNC:
                    buffer_object = new base::BufferUnSync<T>(policy.size, initial_value, policy.type == ConnPolicy::CIRCULAR_BUFFER);
                    break;
                }
                return new ChannelBufferElement<T>(typename base::BufferInterface<T>::shared_ptr(buffer_object), policy);
            }
            return NULL;
        }

        /** During the process of building a connection between two ports, this
         * method builds the input half (starting from the OutputPort).
         *
         * The ConnPolicy may require to install a buffer at the output side
         * of the channel.
         *
         * @param port The output port to which the connection is added.
         * @param policy The policy dictating which kind of buffer must be installed.
         * The transport and other parameters are ignored.
         *
         * @see buildChannelOutput
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildChannelInput(OutputPort<T>& port, ConnPolicy const& policy)
        {
            typename internal::ConnInputEndpoint<T>::shared_ptr endpoint = port.getConnEndpoint();
            typename base::ChannelElement<T>::shared_ptr buffer;

            if (policy.pull == ConnPolicy::PULL && policy.shared == ConnPolicy::PRIVATE) {
                buffer = buildDataStorage<T>(policy, port.getLastWrittenValue());
                if (!buffer) {
                    return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                }
                endpoint->addOutput(buffer, policy.mandatory);
                return buffer;

            } else if (policy.pull == ConnPolicy::PULL && policy.shared == ConnPolicy::SHARED) {
                // For shared pull connections, the buffer belongs to this output port.
                buffer = port.getBuffer();

                if (!buffer) {
                    if (endpoint->connected()) {
                        log(Error) << "You tried to create a shared pull connection for output port " << port.getName() << ", "
                                   << "but the port already has at least one incompatible connection." << endlog();
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }

                    buffer = buildDataStorage<T>(policy, port.getLastWrittenValue());
                    if (!buffer) {
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }
                    buffer->setOutput(endpoint);

                } else {
                    // check that the existing buffer type is compatible to the new ConnPolicy
                    assert(buffer->getConnPolicy());
                    ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                    if (
                        (buffer_policy.type != policy.type) ||
                        (buffer_policy.size != policy.size) ||
                        (buffer_policy.lock_policy != policy.lock_policy)
                       )
                    {
                        log(Error) << "You mixed incompatible connection policies for output port " << port.getName() << ": "
                                   << "The new connection requests a " << policy.toString() << " connection, "
                                   << "but the port already has a " << buffer_policy.toString() << " buffer." << endlog();
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }
                }
            }

            return endpoint;
        }

        /** During the process of building a connection between two ports, this
         * method builds the output part of the channel, that is the half that
         * is connected to the input port. The returned value is the connection
         * element that should be connected to the end of the input-half.
         *
         * The ConnPolicy may require to install a buffer at the output side
         * of the channel.
         *
         * @param port The input port to which the connection is added.
         * @param policy The policy dictating which kind of buffer must be installed.
         * The transport and other parameters are ignored.
         * @param initial_value The value to use to initialize the connection's storage buffer.
         *
         * @see buildChannelInput
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildChannelOutput(InputPort<T>& port, ConnPolicy const& policy, T const& initial_value = T() )
        {
            typename internal::ConnOutputEndpoint<T>::shared_ptr endpoint = port.getConnEndpoint();
            typename base::ChannelElement<T>::shared_ptr buffer = port.getBuffer();

            if (policy.pull == ConnPolicy::PUSH && policy.shared == ConnPolicy::PRIVATE) {
                // For private push connections, the buffer belongs to this input port.

                if (!buffer) {
                    buffer = buildDataStorage<T>(policy, initial_value);
                    if (!buffer) {
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }
                    endpoint->setOutput(buffer);

                } else {
                    // check that the existing buffer type is compatible to the new ConnPolicy
                    assert(buffer->getConnPolicy());
                    ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                    if (
                        (buffer_policy.type != policy.type) ||
                        (buffer_policy.size != policy.size) ||
                        (buffer_policy.lock_policy != policy.lock_policy)
                       )
                    {
                        log(Error) << "You mixed incompatible connection policies for input port " << port.getName() << ": "
                                   << "The new connection requests a " << policy.toString() << " connection, "
                                   << "but the port already has a " << buffer_policy.toString() << " buffer." << endlog();
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }
                }

            } else if (buffer) {
                // The new connection requires an unbuffered channel output, but this port already has an input buffer!
                assert(buffer->getConnPolicy());
                ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                log(Error) << "You mixed incompatible connection policies for input port " << port.getName() << ": "
                           << "The new connection requests a " << policy.toString() << " connection, "
                           << "but the port already has a " << buffer_policy.toString() << " buffer." << endlog();
                return typename internal::ConnOutputEndpoint<T>::shared_ptr();
            }

            return endpoint;
        }

        template <typename T>
        static SharedConnectionBase::shared_ptr buildOrCheckSharedConnection(OutputPort<T>& output_port, base::InputPortInterface& input_port, ConnPolicy const& policy)
        {
            // try to find an existing shared connection first
            SharedConnectionBase::shared_ptr shared_connection;
            shared_connection = output_port.getManager()->getSharedConnection();
            if (!shared_connection) {
                shared_connection = input_port.getManager()->getSharedConnection();
                if (!shared_connection && !policy.name_id.empty()) {
                    // lookup shared connection by the given name
                    shared_connection = SharedConnectionRepository::Instance()->get(policy.name_id);
                }

            } else {
                // For the case both, the output and the input port already have shared connections, check if it matches the one of the input port:
                SharedConnectionBase::shared_ptr input_ports_shared_connection = input_port.getManager()->getSharedConnection();
                if (shared_connection == input_ports_shared_connection) {
                    RTT::log(RTT::Info) << "Output port '" << output_port.getName() << "' and input port '" << input_port.getName() << "' are already connected to the same shared connection." << RTT::endlog();
                    // return SharedConnectionBase::shared_ptr();
                } else if (input_ports_shared_connection) {
                    RTT::log(RTT::Error) << "Output port '" << output_port.getName() << "' and input port '" << input_port.getName() << "' are already connected to different shared connections!" << RTT::endlog();
                    return SharedConnectionBase::shared_ptr();
                }
            }

            // create a new shared connection or check if the existing one matches
            if (!shared_connection) {
                shared_connection.reset(new SharedConnection<T>(buildDataStorage<T>(policy, output_port.getLastWrittenValue()), policy));
                policy.name_id = shared_connection->getName();

            // ... or check if the found connection is compatible
            } else {
                if (!policy.name_id.empty() && shared_connection->getName() != policy.name_id) {
                    RTT::log(RTT::Error) << "Either the output port '" << output_port.getName() << "' or the input port '" << input_port.getName() << "' or both "
                                         << "are already connected to shared connection '" << shared_connection->getName() << "' but you requested to connect to '" << policy.name_id << "'!" << RTT::endlog();
                    return SharedConnectionBase::shared_ptr();
                }

                if (
                    (shared_connection->getConnPolicy()->type != policy.type) ||
                    (shared_connection->getConnPolicy()->size != policy.size) ||
                    (shared_connection->getConnPolicy()->lock_policy != policy.lock_policy)
                   )
                {
                    log(Error) << "You mixed incompatible connection policies for shared connection '" << shared_connection->getName() << "': "
                               << "The new connection requests a " << policy.toString() << " connection, "
                               << "but the existing connection has a " << shared_connection->getConnPolicy()->toString() << " buffer." << endlog();
                    return SharedConnectionBase::shared_ptr();
                }
            }

            return shared_connection;
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
        static bool createConnection(OutputPort<T>& output_port, base::InputPortInterface& input_port, ConnPolicy const& policy)
        {
            if ( !output_port.isLocal() ) {
                log(Error) << "Need a local OutputPort to create connections." <<endlog();
                return false;
            }

            InputPort<T>* input_p = dynamic_cast<InputPort<T>*>(&input_port);

            // This is the input and output channel element of the output half
            base::ChannelElementBase::shared_ptr output_half;
            if (input_port.isLocal() && policy.transport == 0)
            {
                // Local connection
                if (!input_p)
                {
                    log(Error) << "Port " << input_port.getName() << " is not compatible with " << output_port.getName() << endlog();
                    return false;
                }

                // local ports, create buffer here.
                if (policy.pull == ConnPolicy::PUSH && policy.shared == ConnPolicy::SHARED) {
                    return createAndCheckSharedConnection(output_port, input_port, buildOrCheckSharedConnection<T>(output_port, input_port, policy), policy);
                }

                output_half = buildChannelOutput<T>(*input_p, policy, output_port.getLastWrittenValue());
            }
            else
            {
                // if the input is not local, this is a pure remote connection,
                // if the input *is* local, the user requested to use a different transport
                // than plain memory, rare case, but we accept it. The unit tests use this for example
                // to test the OOB transports.
                if ( !input_port.isLocal() ) {
                    output_half = createRemoteConnection( output_port, input_port, policy);
                } else
                    output_half = createOutOfBandConnection<T>( output_port, *input_p, policy);
            }

            if (!output_half)
                return false;

            // Since output is local, buildChannelInput is local as well.
            // This this the input channel element of the whole connection
            base::ChannelElementBase::shared_ptr channel_input;
            channel_input = buildChannelInput<T>(output_port, policy);

            if (!channel_input) {
                output_half->disconnect(true);
                return false;
            }

            // NOTE: channel_input and output_half are not yet connected!
            return createAndCheckConnection(output_port, input_port, channel_input, output_half, policy);
        }

        /**
         * Creates, attaches and checks an outbound stream to an Output port.
         *
         * @param output_port The port to connect the stream to.
         * @param policy The policy dictating which transport to use.
         * @return true if the stream could be created and connected to output_port.
         */
        template<class T>
        static bool createStream(OutputPort<T>& output_port, ConnPolicy const& policy)
        {
            StreamConnID *sid = new StreamConnID(policy.name_id);
            RTT::base::ChannelElementBase::shared_ptr chan = buildChannelInput( output_port, policy );
            if (!chan) return false;
            return createAndCheckStream(output_port, policy, chan, sid);
        }

        /**
         * Creates, attaches and checks an inbound stream to an Input port.
         *
         * @param input_port The port to connect the stream to.
         * @param policy The policy dictating which transport to use.
         * @return true if the stream could be created and connected to input_port.
         */
        template<class T>
        static bool createStream(InputPort<T>& input_port, ConnPolicy const& policy)
        {
            StreamConnID *sid = new StreamConnID(policy.name_id);
            RTT::base::ChannelElementBase::shared_ptr outhalf = buildChannelOutput( input_port, policy );
            if (!outhalf) return false;
            return createAndCheckStream(input_port, policy, outhalf, sid);
        }

    protected:
        static bool createAndCheckConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, base::ChannelElementBase::shared_ptr channel_input, base::ChannelElementBase::shared_ptr channel_output, ConnPolicy const& policy);

        static bool createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr channel_input, StreamConnID* conn_id);

        static bool createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr channel_output, StreamConnID* conn_id);

        static base::ChannelElementBase::shared_ptr createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, ConnPolicy const& policy);

        static bool createAndCheckSharedConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, SharedConnectionBase::shared_ptr shared_connection, ConnPolicy const& policy);

        /**
         * This code is for setting up an in-process out-of-band connection.
         * This means that both input and output port are present in the same process.
         * This function is used when the policy dictates a transport protocol, but both
         * ports are local.
         * @return a channel element chain with a channel output endpoint, but no channel input endpoint.
         */
        template<class T>
        static base::ChannelElementBase::shared_ptr createOutOfBandConnection(OutputPort<T>& output_port, InputPort<T>& input_port, ConnPolicy const& policy) {
            StreamConnID* conn_id = new StreamConnID(policy.name_id);
            RTT::base::ChannelElementBase::shared_ptr output_half = ConnFactory::buildChannelOutput<T>(input_port, policy, output_port.getLastWrittenValue());
            if (!output_half) return base::ChannelElementBase::shared_ptr();
            return createAndCheckOutOfBandConnection( output_port, input_port, policy, output_half, conn_id);
        }

        static base::ChannelElementBase::shared_ptr createAndCheckOutOfBandConnection( base::OutputPortInterface& output_port,
                                                                                       base::InputPortInterface& input_port,
                                                                                       ConnPolicy const& policy,
                                                                                       base::ChannelElementBase::shared_ptr output_half,
                                                                                       StreamConnID* conn_id);
    };

        typedef boost::shared_ptr<ConnFactory> ConnFactoryPtr;


    }
}

#endif

