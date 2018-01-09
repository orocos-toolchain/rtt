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

#include "../rtt-config.h"

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

        /**
         * Tries to find an existing or creates a new shared connection object for the given output port, input port and connection policy.
         * If an existing shared connection has been found, also checks if it matches the given policy and output an error otherwise.
         * The input and output port arguments are optional and can be NULL pointers.
         *
         * @param output_port The output port to connect the channel's input end to (optional).
         * @param input_port The output port to connect the channel's output end to (optional).
         * @param policy Describes the kind of storage requested by the user
         * @return a SharedConnectionBase element.
         */
        virtual internal::SharedConnectionBase::shared_ptr buildSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy) const = 0;

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
                    data_object.reset( new base::DataObjectLockFree<T>(initial_value, policy) );
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
                typename base::BufferInterface<T>::shared_ptr buffer_object;
                switch (policy.lock_policy)
                {
#ifndef OROBLD_OS_NO_ASM
                case ConnPolicy::LOCK_FREE:
                    buffer_object.reset(new base::BufferLockFree<T>(policy.size, initial_value, policy));
                    break;
#else
                case ConnPolicy::LOCK_FREE:
                    RTT::log(Warning) << "lock free connection policy is unavailable on this system, defaulting to LOCKED" << RTT::endlog();
#endif
                case ConnPolicy::LOCKED:
                    buffer_object.reset(new base::BufferLocked<T>(policy.size, initial_value, policy));
                    break;
                case ConnPolicy::UNSYNC:
                    buffer_object.reset(new base::BufferUnSync<T>(policy.size, initial_value, policy));
                    break;
                }
                return new ChannelBufferElement<T>(buffer_object, policy);
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
        static base::ChannelElementBase::shared_ptr buildChannelInput(OutputPort<T>& port, ConnPolicy const& policy, bool force_unbuffered = false)
        {
            typename internal::ConnInputEndpoint<T>::shared_ptr endpoint = port.getEndpoint();
            typename base::ChannelElement<T>::shared_ptr buffer = port.getSharedBuffer();

            // Note: PerInputPort implies PUSH and PerOutputPort implies PULL
            bool pull = policy.pull;
            if (policy.buffer_policy == PerInputPort) pull = ConnPolicy::PUSH;
            if (policy.buffer_policy == PerOutputPort) pull = ConnPolicy::PULL;

            if (pull == ConnPolicy::PULL && !force_unbuffered) {
                if (!buffer) {
                    buffer = buildDataStorage<T>(policy, port.getLastWrittenValue());
                    if (!buffer) return typename internal::ConnOutputEndpoint<T>::shared_ptr();

                    if (policy.buffer_policy == PerOutputPort) {
                        // For PerOutputPort connections, the buffer is installed BEFORE the input endpoint!
                        if (endpoint->connected()) {
                            log(Error) << "You tried to create a shared output buffer connection for output port " << port.getName() << ", "
                                       << "but the port already has at least one incompatible outgoing connection." << endlog();
                            return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                        }
                        return buffer->connectTo(endpoint) ? endpoint : typename internal::ConnInputEndpoint<T>::shared_ptr();
                    } else {
                        return endpoint->connectTo(buffer, policy.mandatory) ? buffer : typename internal::ConnInputEndpoint<T>::shared_ptr();
                    }

                } else if (policy.buffer_policy == PerOutputPort) {
                    // check that the existing buffer type is compatible to the new ConnPolicy
                    assert(buffer->getConnPolicy());
                    ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                    if (
                        (buffer_policy.type != policy.type) ||
                        (buffer_policy.size != policy.size) ||
                        (buffer_policy.lock_policy != policy.lock_policy)
                       )
                    {
                        log(Error) << "You mixed incompatible connection policies for the shared output buffer of port " << port.getName() << ": "
                                   << "The new connection requests a " << policy << " connection, "
                                   << "but the port already has a " << buffer_policy << " buffer." << endlog();
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }

                    return endpoint;
                }
            }

            if (buffer) {
                // The new connection requires an unbuffered channel input or a per-connection buffer, but this port already has a shared output buffer!
                assert(buffer->getConnPolicy());
                ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                log(Error) << "You mixed incompatible connection policies for output port " << port.getName() << ": "
                           << "The new connection requests a " << policy << " connection, "
                           << "but the port already has a " << buffer_policy << " buffer." << endlog();
                return typename internal::ConnOutputEndpoint<T>::shared_ptr();
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
            typename internal::ConnOutputEndpoint<T>::shared_ptr endpoint = port.getEndpoint();
            typename base::ChannelElement<T>::shared_ptr buffer = port.getSharedBuffer();

            // Note: PerInputPort implies PUSH and PerOutputPort implies PULL
            bool pull = policy.pull;
            if (policy.buffer_policy == PerInputPort) pull = ConnPolicy::PUSH;
            if (policy.buffer_policy == PerOutputPort) pull = ConnPolicy::PULL;

            if (pull == ConnPolicy::PUSH) {
                if (!buffer) {
                    buffer = buildDataStorage<T>(policy, initial_value);
                    if (!buffer) return typename internal::ConnOutputEndpoint<T>::shared_ptr();

                    if (policy.buffer_policy == PerInputPort) {
                        // For PerInputPort connections, the buffer is installed AFTER the output endpoint!
                        if (endpoint->connected()) {
                            log(Error) << "You tried to create a shared input buffer connection for input port " << port.getName() << ", "
                                       << "but the port already has at least one incompatible incoming connection." << endlog();
                            return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                        }
                        return endpoint->connectTo(buffer) ? endpoint : typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    } else {
                        return buffer->connectTo(endpoint) ? buffer : typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }

                } else if (policy.buffer_policy == PerInputPort) {
                    // check that the existing buffer type is compatible to the new ConnPolicy
                    assert(buffer->getConnPolicy());
                    ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                    if (
                        (buffer_policy.type != policy.type) ||
                        (buffer_policy.size != policy.size) ||
                        (buffer_policy.lock_policy != policy.lock_policy)
                       )
                    {
                        log(Error) << "You mixed incompatible connection policies for the shared input buffer of port " << port.getName() << ": "
                                   << "The new connection requests a " << policy << " connection, "
                                   << "but the port already has a " << buffer_policy << " buffer." << endlog();
                        return typename internal::ConnOutputEndpoint<T>::shared_ptr();
                    }

                    return endpoint;
                }
            }

            if (buffer) {
                // The new connection requires an unbuffered channel output or a per-connection buffer, but this port already has a shared input buffer!
                assert(buffer->getConnPolicy());
                ConnPolicy buffer_policy = *(buffer->getConnPolicy());

                log(Error) << "You mixed incompatible connection policies for input port " << port.getName() << ": "
                           << "The new connection requests a " << policy << " connection, "
                           << "but the port already has a " << buffer_policy << " buffer." << endlog();
                return typename internal::ConnOutputEndpoint<T>::shared_ptr();
            }

            return endpoint;
        }

        /**
         * Tries to find an existing or creates a new shared connection object for the given output port, input port and connection policy.
         * If an existing shared connection has been found, also checks if it matches the given policy and output an error otherwise.
         * The input and output port arguments are optional and can be NULL pointers.
         *
         * @param output_port The output port to connect the channel's input end to (optional).
         * @param input_port The output port to connect the channel's output end to (optional).
         * @param policy Describes the kind of storage requested by the user
         * @param shared_connection A reference to a SharedConnectionBase pointer that will point to the found instance or an empty pointer, if
         *                          either no connection has been found or the found connection was invalid for the given ports and policy.
         * @return true if an exiting SharedConnectionBase instance could be found.
         */
        static bool findSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy, SharedConnectionBase::shared_ptr &shared_connection);

        /**
         * Tries to find an existing or creates a new shared connection object for the given output port, input port and connection policy.
         * If an existing shared connection has been found, also checks if it matches the given policy and output an error otherwise.
         * The input and output port arguments are optional and can be NULL pointers.
         *
         * @param output_port The output port to connect the channel's input end to (optional).
         * @param input_port The output port to connect the channel's output end to (optional).
         * @param policy Describes the kind of storage requested by the user
         * @return a SharedConnectionBase element.
         */
        template <typename T>
        static SharedConnectionBase::shared_ptr buildSharedConnection(OutputPort<T> *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy)
        {
            // try to find an existing shared connection first
            SharedConnectionBase::shared_ptr shared_connection;

            // abort if an incompatible connection has been found or one of the ports is already connected to another shared connection
            if (findSharedConnection(output_port, input_port, policy, shared_connection) && !shared_connection) {
                return SharedConnectionBase::shared_ptr();
            }

            // for remote input ports, and if we can derive the type from the output port, build the shared buffer at the remote side and only generate a proxy here:
            if (input_port && !input_port->isLocal()) {
                if (!output_port) {
                    log(Error) << "Cannot create a shared connection for a remote input port or a non-standard transport without knowing the local output port." << endlog();
                    return SharedConnectionBase::shared_ptr();
                }

                // If no existing shared connection has been found, ask the remote side to build a shared channel output and create a local proxy.
                if (!shared_connection) {
                    base::ChannelElementBase::shared_ptr output_half = buildRemoteChannelOutput( *output_port, *input_port, policy);
                    if (!output_half) {
                        log(Error) << "Could not create a shared remote connection for input port '" << input_port->getName() << "'." << endlog();
                        return SharedConnectionBase::shared_ptr();
                    }

                    shared_connection = new SharedRemoteConnection<T>(policy);
                    shared_connection->connectTo(output_half, policy.mandatory);

                // ...or only ask the remote side to connect the additional input port to an existing remote shared connection
                } else {
//                    typename SharedRemoteConnection<T>::shared_ptr shared_remote_connection = boost::dynamic_pointer_cast<SharedRemoteConnection<T> >(shared_connection);

//                    if (!shared_remote_connection) {
//                        log(Error) << "Cannot create a shared connection for a remote input port because the local output port is already connected to a local shared connection." << endlog();
//                        return SharedConnectionBase::shared_ptr();
//                    }

//                    if (!input_port->createConnection(shared_remote_connection, policy)) {
                    if (!input_port->createConnection(shared_connection, policy)) {
                        log(Error) << "The remote side refused to connect the input port '" << input_port->getName() << "' to the existing shared connection '" << shared_connection->getName() << "'." << endlog();
                        return SharedConnectionBase::shared_ptr();
                    }
                }
            }

            // create a new shared connection instance
            if (!shared_connection) {
                RTT::OutputPort<T> *output_p = dynamic_cast<RTT::OutputPort<T> *>(output_port);
                typename base::ChannelElement<T>::shared_ptr buffer = buildDataStorage<T>(policy, (output_p ? output_p->getLastWrittenValue() : T()));
                if (!buffer) return SharedConnectionBase::shared_ptr();
                shared_connection.reset(new SharedConnection<T>(buffer.get(), policy));
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

            if (output_port.connectedTo(&input_port)) {
                log(Info) << "OutputPort " << output_port.getName() << " is already connected to " << input_port.getName() << ", ignoring new connection." << endlog();
                return true;
            }

            InputPort<T>* input_p = dynamic_cast<InputPort<T>*>(&input_port);

            // Shared push connection? => forward to createAndCheckSharedConnection()
            if (policy.buffer_policy == Shared) {
                return createAndCheckSharedConnection(&output_port, &input_port, buildSharedConnection<T>(&output_port, &input_port, policy), policy);
            }

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
                output_half = buildChannelOutput<T>(*input_p, policy, output_port.getLastWrittenValue());
            }
            else
            {
                // if the input is not local, this is a pure remote connection,
                // if the input *is* local, the user requested to use a different transport
                // than plain memory, rare case, but we accept it. The unit tests use this for example
                // to test the OOB transports.
                if ( !input_port.isLocal() ) {
                    output_half = buildRemoteChannelOutput( output_port, input_port, policy);
                } else if (input_p) {
                    return createOutOfBandConnection<T>( output_port, *input_p, policy);
                } else {
                    log(Error) << "Port " << input_port.getName() << " is not compatible with " << output_port.getName() << endlog();
                    return false;
                }
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
            // Stream channel inputs are always unbuffered (push). It's the transport that has to add a buffer element if required.
            RTT::base::ChannelElementBase::shared_ptr chan = buildChannelInput( output_port, policy, /* force_unbuffered = */ true );
            if (!chan) return false;
            return bool(createAndCheckStream(output_port, policy, chan, sid));
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
            return bool(createAndCheckStream(input_port, policy, outhalf, sid));
        }

        static bool createAndCheckSharedConnection(base::OutputPortInterface* output_port, base::InputPortInterface* input_port, SharedConnectionBase::shared_ptr shared_connection, ConnPolicy const& policy);

    protected:
        static bool createAndCheckConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, base::ChannelElementBase::shared_ptr channel_input, base::ChannelElementBase::shared_ptr channel_output, ConnPolicy const& policy);

        static base::ChannelElementBase::shared_ptr createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr channel_input, StreamConnID* conn_id);

        static base::ChannelElementBase::shared_ptr createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr channel_output, StreamConnID* conn_id);

        static base::ChannelElementBase::shared_ptr buildRemoteChannelOutput(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, ConnPolicy const& policy);

        /**
         * This code is for setting up an in-process out-of-band connection.
         * This means that both input and output port are present in the same process.
         * This function is used when the policy dictates a transport protocol, but both
         * ports are local.
         * @return true if the out-of-band connection was successfully setup.
         */
        template<class T>
        static bool createOutOfBandConnection(OutputPort<T>& output_port, InputPort<T>& input_port, ConnPolicy const& policy) {
            // constructs an out-of-band channel:
            // output_port -> channel_input -> stream_input -> (out-of-band transport of data) -> stream_output -> channel_output -> input_port
            //                                           |-- (direct connection for coordination) --^
            //

            // Stream channel inputs are always unbuffered (push). It's the transport that has to add a buffer element if required.
            RTT::base::ChannelElementBase::shared_ptr channel_input = buildChannelInput( output_port, policy, /* force_unbuffered = */ true );
            if (!channel_input) return false;

            RTT::base::ChannelElementBase::shared_ptr stream_input = createAndCheckStream(output_port, policy, channel_input, new StreamConnID(policy.name_id));
            if (!stream_input) return false;

            RTT::base::ChannelElementBase::shared_ptr channel_output = ConnFactory::buildChannelOutput<T>(input_port, policy, output_port.getLastWrittenValue());
            if (!channel_output) return false;

            RTT::base::ChannelElementBase::shared_ptr stream_output = createAndCheckStream(input_port, policy, channel_output, new StreamConnID(policy.name_id));
            if (!stream_output) return false;

            return stream_input->getOutputEndPoint()->connectTo(stream_output->getInputEndPoint(), policy.mandatory);
        }

    };

    typedef boost::shared_ptr<ConnFactory> ConnFactoryPtr;

}}

#endif

