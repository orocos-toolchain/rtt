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
#include "../types/TypeMarshaller.hpp"

#include "../base/DataObject.hpp"
#include "../base/DataObjectUnSync.hpp"
#include "../base/Buffer.hpp"
#include "../base/BufferUnSync.hpp"

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
     * connections between two ports
     */
    class RTT_API ConnFactory
    {
    public:

        /** This method is analoguous to the static ConnFactory::buildChannelOutput.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the types::TypeInfo object
         */
        virtual base::ChannelElementBase* buildRemoteChannelOutput(
                base::OutputPortInterface& output_port,
                types::TypeInfo const* type_info,
                base::InputPortInterface& input, const ConnPolicy& policy) = 0;

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
                case ConnPolicy::LOCKED:
                    data_object.reset( new base::DataObjectLocked<T>(initial_value) );
                    break;
#ifndef OROBLD_OS_NO_ASM
                case ConnPolicy::LOCK_FREE:
                    data_object.reset( new base::DataObjectLockFree<T>(initial_value) );
                    break;
#endif
                case ConnPolicy::UNSYNC:
                    data_object.reset( new base::DataObjectUnSync<T>(initial_value) );
                    break;
                }

                ChannelDataElement<T>* result = new ChannelDataElement<T>(data_object);
                return result;
            }
            else if (policy.type == ConnPolicy::BUFFER)
            {
                base::BufferInterface<T>* buffer_object = 0;
                switch (policy.lock_policy)
                {
                case ConnPolicy::LOCKED:
                    buffer_object = new base::BufferLocked<T>(policy.size, initial_value);
                    break;
#ifndef OROBLD_OS_NO_ASM
                case ConnPolicy::LOCK_FREE:
                    buffer_object = new base::BufferLockFree<T>(policy.size, initial_value);
                    break;
#endif
                case ConnPolicy::UNSYNC:
                    buffer_object = new base::BufferUnSync<T>(policy.size, initial_value);
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
        static base::ChannelElementBase* buildChannelInput(OutputPort<T>& port, ConnID* conn_id, base::ChannelElementBase::shared_ptr output_channel)
        {
            assert(conn_id);
            base::ChannelElementBase* endpoint = new ConnInputEndpoint<T>(&port, conn_id);
            if (output_channel)
                endpoint->setOutput(output_channel);
            return endpoint;
        }

        template<typename T>
        static base::ChannelElementBase* buildBufferedChannelInput(OutputPort<T>& port, ConnID* conn_id, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr output_channel)
        {
            assert(conn_id);
            base::ChannelElementBase* endpoint = new ConnInputEndpoint<T>(&port, conn_id);
            base::ChannelElementBase* data_object = buildDataStorage<T>(policy, port.getLastWrittenValue() );
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
        static base::ChannelElementBase* buildChannelOutput(InputPort<T>& port, ConnID* conn_id)
        {
            assert(conn_id);
            base::ChannelElementBase* endpoint = new ConnOutputEndpoint<T>(&port, conn_id);
            return endpoint;
        }

        template<typename T>
        static base::ChannelElementBase* buildBufferedChannelOutput(InputPort<T>& port, ConnID* conn_id, ConnPolicy const& policy, T const& initial_value = T() )
        {
            assert(conn_id);
            base::ChannelElementBase* endpoint = new ConnOutputEndpoint<T>(&port, conn_id);
            base::ChannelElementBase* data_object = buildDataStorage<T>(policy, initial_value);
            data_object->setOutput(endpoint);
            return data_object;
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
                // local ports, create buffer here.
                output_half = ConnFactory::buildBufferedChannelOutput<T>(*input_p, output_port.getPortID(), policy, output_port.getLastWrittenValue());
            }
            else
            {
                // if the input is not local, this is a pure remote connection,
                // if the input *is* local, the user requested to use a different transport
                // than plain memory, rare case, but we accept it. The unit tests use this for example
                // to test the OOB transports.
                if ( !input_port.isLocal() ) {
                    output_half = ConnFactory::createRemoteConnection( output_port, input_port, policy);
                } else
                    output_half = ConnFactory::createOutOfBandConnection<T>( output_port, *input_p, policy);
            }

            if (!output_half)
                return false;

            // Since output is local, buildChannelInput is local as well.
            // This this the input channel element of the whole connection
            base::ChannelElementBase::shared_ptr channel_input =
                ConnFactory::buildChannelInput<T>(output_port, input_port.getPortID(), output_half);

            return ConnFactory::createAndCheckConnection(output_port, input_port, channel_input, policy );
        }

        template<class T>
        static bool createStream(OutputPort<T>& output_port, ConnPolicy const& policy)
        {
            StreamConnID *sid = new StreamConnID(policy.name_id);
            RTT::base::ChannelElementBase::shared_ptr chan = buildChannelInput( output_port, sid, base::ChannelElementBase::shared_ptr() );
            return createAndCheckStream(output_port, policy, chan, sid);
        }

        static bool createAndCheckStream(base::OutputPortInterface& output_port, ConnPolicy const& policy, base::ChannelElementBase::shared_ptr chan, StreamConnID* conn_id);

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

}}

#endif

