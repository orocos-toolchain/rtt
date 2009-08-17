#ifndef ORO_OUTPUT_PORT_INTERFACE_HPP
#define ORO_OUTPUT_PORT_INTERFACE_HPP

#include "PortInterface.hpp"
#include "../internal/ListLockFree.hpp"
#include <boost/tuple/tuple.hpp>
#include "../os/Mutex.hpp"

namespace RTT
{ namespace base {

    class RTT_API OutputPortInterface : public PortInterface
    {
    protected:
        typedef boost::tuple<PortID*, ChannelElementBase::shared_ptr, internal::ConnPolicy> ChannelDescriptor;
        internal::ListLockFree< ChannelElementBase::shared_ptr > channels;
        internal::ListLockFree< ChannelDescriptor > connections;

        /** Helper method for disconnect(PortInterface*)
         *
         * This method removes the channel listed in \c descriptor from the list
         * of output channels if \c port has the same id that the one listed in
         * \c descriptor.
         *
         * @returns true if the descriptor matches, false otherwise
         */
        bool eraseIfMatchingPort(PortInterface const* port, ChannelDescriptor& descriptor);

        /** Helper method for disconnect()
         *
         * Unconditionally removes the given connection and return true
         */
        bool eraseConnection(OutputPortInterface::ChannelDescriptor& descriptor);

        /** Helper method for removeConnection(channel) */
        bool matchConnectionChannel(ChannelElementBase::shared_ptr channel, ChannelDescriptor const& descriptor) const;

        /** Helper method for port-to-port connection establishment */
        void addConnection(PortID* port_id, ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy);


        /** os::Mutex for when it is needed to resize the connections list */
        os::Mutex connection_resize_mtx;

    public:
        OutputPortInterface(std::string const& name);
        ~OutputPortInterface();

        /** Adds a new channel to the output list of this port. This is thread safe.
         */
        void addChannel(ChannelElementBase::shared_ptr channel);

        /** Removes the given output channel from the list of output channels of
         * this port. This is thread safe.
         */
        void removeChannel(ChannelElementBase::shared_ptr channel);

        /** If true, the port keeps track of the last value given to write().
         * The value can be accessed with InputPort<T>::getLastWrittenValue().
         *
         * @see keepLastWrittenValue(bool)
         */
        virtual bool keepsLastWrittenValue() const = 0;

        /** Set this flag to true so that the port keeps track of the last value
         * given to write(). The value can be accessed with
         * InputPort<T>::getLastWrittenValue().
         *
         * @see keepLastWrittenValue()
         */
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        /** Removes all channels from the list of outputs */
        virtual void disconnect();

        /** Returns true if there is at least one channel registered in this
         * port's list of outputs
         */
        virtual bool connected() const;

        /** Writes to the port the value contained by the given data source.
         * This is only valid for local ports.
         */
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

        /** Connects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( InputPortInterface& sink, internal::ConnPolicy const& policy ) = 0;

        /** Removes the channel that connects this port to \c port */
        void disconnect(PortInterface& port);

        /** Removes the connection associated with this channel, and the channel
         * as well
         */
        bool removeConnection(ChannelElementBase::shared_ptr channel);

        /** Connects this port with \a other, using the given policy Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        virtual bool connectTo(PortInterface& other, internal::ConnPolicy const& policy);
    };
}}

#endif
