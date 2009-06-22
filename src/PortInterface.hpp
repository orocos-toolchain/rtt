#ifndef ORO_EXECUTION_PORT_INTERRFACE_HPP
#define ORO_EXECUTION_PORT_INTERRFACE_HPP

#include <string>
#include "Channels.hpp"
#include "Events.hpp"
#include "ListLockFree.hpp"

namespace RTT
{
    class ConnFactory;
    class TypeInfo;
    class TaskObject;
    class ChannelElementBase;

    /** This class is used in places where a permanent representation of a
     * reference to a port is needed, like in OutputPortInterface.
     *
     * It is usually returned by PortInterface::getPortID, and used by
     * PortInterface::isSameID(PortID const&)
     */
    class PortID
    {
    public:
        virtual ~PortID() {}
    };

    class PortInterface
    {
        struct PortID : public RTT::PortID
        {
            PortInterface const* ptr;
            PortID(PortInterface const* obj)
                : ptr(obj) {}
        };

        std::string name;

    protected:
        PortInterface(const std::string& name);

    public:
        virtual ~PortInterface() {}

        virtual RTT::PortID* getPortID() const;
        virtual bool isSameID(RTT::PortID const& id) const;

        /**
         * Get the name of this Port.
         */
        const std::string& getName() const { return name; }

        /**
         * Change the name of this unconnected Port.
         * One can only change the name when it is not yet connected.
         * @retval true if !this->connected(), the name has changed.
         * @retval false if this->connected(), the name has not been changed.
         */
        bool setName(const std::string& name);

        /** Returns true if this port is connected */
        virtual bool connected() const = 0;

        /** Returns the TypeInfo object for the port's type */
        virtual const TypeInfo* getTypeInfo() const = 0;

        /** Removes any connection that either go to or come from this port */
        virtual void disconnect() = 0;

        /** The ChannelFactory object that allows to build the ChannelElement chain
         * needed to build connections to or from this port
         */
        virtual ConnFactory* getConnFactory();

        /** Returns true if this port is located on this process, and false
         * otherwise
         */
        virtual bool isLocal() const;

        /**
         * Returns the protocol over which this port can be accessed.
         */
        virtual int serverProtocol() const;

        /**
         * Create a local clone of this port with the same name. If this port is
         * a local port, this is an object of the same type and same name. If
         * this object is a remote port, then it is a local port of the same
         * type and same name.
         */
        virtual PortInterface* clone() const = 0;

        /**
         * Create a local clone of this port with the same name. If this port is
         * a local port, this is an object of the inverse direction (read for
         * write and write for read), and same name. If this object is a remote
         * port, then it is a local port of the inverse direction and with the
         * same name.
         */
        virtual PortInterface* antiClone() const = 0;

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject();

        /** Connects this port with \a other, using the given policy Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        virtual bool connectTo(PortInterface& other, ConnPolicy const& policy) = 0;
    };

    class InputPortInterface : public PortInterface
    {
    public:
        typedef Event<void(PortInterface*)> NewDataOnPortEvent;

    protected:
        ChannelElementBase* channel;
        ConnPolicy          default_policy;
        NewDataOnPortEvent* new_data_on_port_event;

    public:
        InputPortInterface(std::string const& name, ConnPolicy const& default_policy = ConnPolicy());
        ~InputPortInterface();

        /** Sets the channel from which this port should get samples
         *
         * You should usually not use this directly. Use createConnection
         * instead.
         */
        virtual void setInputChannel(ChannelElementBase* channel_output);

        /** Clears the input channel
         *
         * You should usually not use this directly. Use createConnection
         * instead.
         */
        virtual void clearInputChannel();

        ConnPolicy getDefaultPolicy() const;

        /** Returns a DataSourceBase interface to read this port. The returned
         * data source is always the same object and will be destroyed when the
         * port is destroyed.
         */
        virtual DataSourceBase* getDataSource() = 0;

        /** Reads the port and updates the value hold by the given data source.
         * This is only valid for local ports.
         *
         * \a source has to be an assignable data source
         */
        virtual bool read(DataSourceBase::shared_ptr source);

        /** Removes any connection that either go to or come from this port */
        virtual void disconnect();

        /** Returns true if this port is connected */
        virtual bool connected() const;

        /** Clears the connection. After call to read() will return false after
         * clear() has been called
         */
        void clear();

        /** Returns the event object that gets emitted when new data is
         * available for this port. It gets deleted when the port is deleted.
         */
        NewDataOnPortEvent* getNewDataOnPortEvent();

        /** Connects this port with \a other, using the given policy Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        virtual bool connectTo(PortInterface& other, ConnPolicy const& policy);
    };

    class OutputPortInterface : public PortInterface
    {
    protected:
        typedef boost::tuple<RTT::PortID*, ChannelElementBase::shared_ptr, ConnPolicy> ChannelDescriptor;
        ListLockFree< ChannelElementBase::shared_ptr > channels;
        ListLockFree< ChannelDescriptor > connections;

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
        void addConnection(RTT::PortID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy);


        /** Mutex for when it is needed to resize the connections list */
        OS::Mutex connection_resize_mtx;

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
        bool createDataConnection( InputPortInterface& sink, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism
         */
        bool createBufferConnection( InputPortInterface& sink, int size, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using the as policy
         * the default policy of the sink port
         */
        bool createConnection( InputPortInterface& sink );

        /** Connects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( InputPortInterface& sink, ConnPolicy const& policy ) = 0;

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
        virtual bool connectTo(PortInterface& other, ConnPolicy const& policy);
    };
}

#endif
