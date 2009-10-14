#ifndef ORO_OUTPUT_PORT_INTERFACE_HPP
#define ORO_OUTPUT_PORT_INTERFACE_HPP

#include "PortInterface.hpp"
#include "../internal/ConnectionManager.hpp"
#include "DataSourceBase.hpp"

namespace RTT
{ namespace base {

    /**
     * The base class of each OutputPort. It contains the connection management code, which is
     * independent of the actual data being transmitted.
     */
    class RTT_API OutputPortInterface : public PortInterface
    {
    protected:
        internal::ConnectionManager cmanager;

        /**
         * Upcall to OutputPort.
         */
        virtual bool connectionAdded(ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy) = 0;
    public:
        /**
         * Adds a new connection to this output port and initializes the connection if required by \a policy.
         * Use with care. Allows you to add any arbitrary connection to this output port. It is your responsibility
         * to do any further bookkeeping, such as informing the input that a new output has been added.
         */
        virtual bool addConnection(internal::ConnID* port_id, ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy);

        OutputPortInterface(std::string const& name);
        ~OutputPortInterface();

        /**
         * Returns true if this port records the last written value.
         */
        virtual bool keepsLastWrittenValue() const = 0;

        /**
         * Change the setting for keeping the last written value.
         * Setting this to false will clear up any unneeded storage.
         * If set, this port can initialize new connections with a data sample and
         * allows real-time data transport of dynamically sized objects
         * over its newly created connections.
         * @see OutputPort::OutputPort.
         */
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        virtual void disconnect();

        /** Returns true if there is at least one channel registered in this
         * port's list of outputs
         */
        virtual bool connected() const;

        virtual void write(DataSourceBase::shared_ptr source);

        /** Connects this write port to the given read port, using a single-data
         * policy with the given locking mechanism
         */
        bool createDataConnection( InputPortInterface& sink, int lock_policy = internal::ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism
         */
        bool createBufferConnection( InputPortInterface& sink, int size, int lock_policy = internal::ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using the as policy
         * the default policy of the sink port
         */
        bool createConnection( InputPortInterface& sink );

        virtual bool createConnection( InputPortInterface& sink, internal::ConnPolicy const& policy ) = 0;

        /** Removes the channel that connects this port to \c port */
        void disconnect(PortInterface& port);

        /** Removes the connection associated with this channel, and the channel
         * as well
         */
        virtual void removeConnection(internal::ConnID* cid);

        virtual bool connectTo(PortInterface& other, internal::ConnPolicy const& policy);

        virtual bool connectTo(PortInterface& other);
    };
}}

#endif
