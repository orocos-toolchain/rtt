#ifndef ORO_CONN_FACTORY_HPP
#define ORO_CONN_FACTORY_HPP

#include "Channels.hpp"
#include "ConnInputEndPoint.hpp"
#include "ConnOutputEndPoint.hpp"
#include "../base/PortInterface.hpp"

#include "../base/DataObject.hpp"
#include "../base/Buffer.hpp"

namespace RTT
{ namespace internal {

    /** This class provides the basic tools to create channels that represent
     * connections between two ports
     */
    class ConnFactory
    {
    public:

        /** This method is analoguous to the static ConnFactory::buildOutputHalf.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the types::TypeInfo object
         */
        virtual base::ChannelElementBase* buildOutputHalf(types::TypeInfo const* type_info,
                base::InputPortInterface& output, ConnPolicy const& policy) = 0;

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
    };

}}

#endif

