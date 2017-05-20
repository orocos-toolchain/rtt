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
         * @return The created endpoint.
         */
        virtual base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port) const = 0;
        /**
         * Creates the input endpoint (starting point) of a communication channel and adds it to an OutputPort.
         *
         * @param port The output port to connect the channel's input end to.
         * @return The created endpoint.
         */
        virtual base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port) const = 0;

        /** This method creates the connection element that will store data
         * inside the connection, based on the given policy
         * @todo: shouldn't this belong in the template type info ? This allows the type lib to
         * choose which locked/lockfree algorithms are implemented and leaves out 4x code generation
         * for each alternative in each compilation unit. Contra: needs T in typelib.
         * @todo: since setDataSample, initial_value is no longer needed.
         */
        template<typename T>
        static base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy, const T& initial_value = T())
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

                ChannelDataElement<T>* result = new ChannelDataElement<T>(data_object);
                return result;
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
                return new ChannelBufferElement<T>(typename base::BufferInterface<T>::shared_ptr(buffer_object));
            }
            return NULL;
        }

        /** During the process of building a connection between two ports, this
         * method builds the input half (starting from the OutputPort).
         *
         * The \c output_channel argument is the connection element that has been returned
         * by buildChannelOutput.
         *
         * @see buildChannelOutput
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildChannelInput(OutputPort<T>& port, ConnID* conn_id, base::ChannelElementBase::shared_ptr output_channel)
        {
            assert(conn_id);
            base::ChannelElementBase::shared_ptr endpoint = new ConnInputEndpoint<T>(&port, conn_id);
            if (output_channel)
                endpoint->setOutput(output_channel);
            return endpoint;
        }

        /**
         * Extended version of buildChannelInput that also installs
         * a buffer after the channel input endpoint, according to a \a policy.
         * @param port The output port to which the connection will be added by client code.
         * @param conn_id A unique connection id which identifies this connection
         * @param policy The policy dictating which kind of buffer must be installed.
         * The transport and other parameters are ignored.
         * @param output_channel Optional. If present, the buffer will be connected
         * to this element.
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildBufferedChannelInput(OutputPort<T>& port, ConnID* conn_id, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr output_channel)
        {
            assert(conn_id);
            base::ChannelElementBase::shared_ptr endpoint = new ConnInputEndpoint<T>(&port, conn_id);
            base::ChannelElementBase::shared_ptr data_object = buildDataStorage<T>(policy, port.getLastWrittenValue() );
            endpoint->setOutput(data_object);
            if (output_channel)
                data_object->setOutput(output_channel);
            return endpoint;
        }

        /** During the process of building a connection between two ports, this
         * method builds the output part of the channel, that is the half that
         * is connected to the input port. The returned value is the connection
         * element that should be connected to the end of the input-half.
         *
         * @see buildChannelInput
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildChannelOutput(InputPort<T>& port, ConnID* conn_id)
        {
            assert(conn_id);
            base::ChannelElementBase::shared_ptr endpoint = new ConnOutputEndpoint<T>(&port, conn_id);
            return endpoint;
        }

        /**
         * Extended version of buildChannelOutput that also installs
         * a buffer before the channel output endpoint, according to a \a policy.
         * @param port The input port to which the connection is added.
         * @param conn_id A unique connection id which identifies this connection
         * @param policy The policy dictating which kind of buffer must be installed.
         * The transport and other parameters are ignored.
         * @param initial_value The value to use to initialize the connection's storage buffer.
         */
        template<typename T>
        static base::ChannelElementBase::shared_ptr buildBufferedChannelOutput(InputPort<T>& port, ConnID* conn_id, ConnPolicy const& policy, T const& initial_value = T() )
        {
            assert(conn_id);
            base::ChannelElementBase::shared_ptr endpoint = new ConnOutputEndpoint<T>(&port, conn_id);
            base::ChannelElementBase::shared_ptr data_object = buildDataStorage<T>(policy, initial_value);
            data_object->setOutput(endpoint);
            return data_object;
        }

        static void fillConnId(ConnID *connId, base::OutputPortInterface& output_port, base::InputPortInterface& input_port);
        
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

            // This is the input channel element of the output half
            base::ChannelElementBase::shared_ptr output_half = 0;
            if (input_port.isLocal() && policy.transport == 0)
            {
                // Local connection
                if (!input_p)
                {
                    log(Error) << "Port " << input_port.getName() << " is not compatible with " << output_port.getName() << endlog();
                    return false;
                }
                ConnID *id = output_port.getPortID();
                fillConnId(id, output_port, input_port);
                
                // local ports, create buffer here.
                output_half = buildBufferedChannelOutput<T>(*input_p, id, policy, output_port.getLastWrittenValue());
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

            ConnID *id = input_port.getPortID();
            fillConnId(id, output_port, input_port);

            // Since output is local, buildChannelInput is local as well.
            // This this the input channel element of the whole connection
            base::ChannelElementBase::shared_ptr channel_input =
                buildChannelInput<T>(output_port, id, output_half);

            return createAndCheckConnection(output_port, input_port, channel_input, policy );
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
            RTT::base::ChannelElementBase::shared_ptr chan = buildChannelInput( output_port, sid, base::ChannelElementBase::shared_ptr() );
            return createAndCheckStream(output_port, policy, chan, sid);
        }

        /** @warning This helper function will be moved to the protected: scope in the next major release */
        static bool createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr chan, StreamConnID* conn_id);

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
            RTT::base::ChannelElementBase::shared_ptr outhalf = buildChannelOutput( input_port, sid );
            if ( createAndCheckStream(input_port, policy, outhalf, sid) )
                return true;
            input_port.removeConnection(sid);
            return false;
        }

    protected:
        static bool createAndCheckConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, base::ChannelElementBase::shared_ptr channel_input, ConnPolicy policy);

        static bool createAndCheckStream(base::InputPortInterface& input_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr outhalf, StreamConnID* conn_id);

        static base::ChannelElementBase::shared_ptr createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, ConnPolicy const& policy);

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
            RTT::base::ChannelElementBase::shared_ptr output_half = ConnFactory::buildChannelOutput<T>(input_port, conn_id);
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

