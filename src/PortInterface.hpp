#ifndef ORO_EXECUTION_PORT_INTERRFACE_HPP
#define ORO_EXECUTION_PORT_INTERRFACE_HPP

#include <string>
#include "Channels.hpp"
#include "Events.hpp"
#include "ListLockFree.hpp"

namespace RTT
{
    class ChannelFactory;
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
        virtual ChannelFactory* getConnFactory();

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

        /** Channelects this port with \a other, using the given policy Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        bool connectTo(PortInterface& other, ConnPolicy const& policy);
    };

    class InputPortInterface : public PortInterface
    {
    public:
        typedef Event<void(PortInterface*)> NewDataOnPortEvent;

    protected:
        ChannelElementBase* writer;
        ConnPolicy default_policy;
        NewDataOnPortEvent* new_data_on_port_event;

    public:
        InputPortInterface(std::string const& name, ConnPolicy const& default_policy = ConnPolicy());
        ~InputPortInterface();

        ConnPolicy getDefaultPolicy() const;

        /** Returns a DataSourceBase interface to read this port. The returned
         * data source is always the same object and will be destroyed when the
         * port is destroyed.
         */
        virtual DataSourceBase* getDataSource() = 0;

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
    };

    class OutputPortInterface : public PortInterface
    {
    protected:
        typedef boost::tuple<RTT::PortID*, ChannelElementBase::shared_ptr, ConnPolicy> ChannelDescriptor;
        ListLockFree< ChannelDescriptor > connections;

        /** Helper method for disconnect(PortInterface*) */
        bool eraseMatchingConnection(PortInterface const* port, ChannelDescriptor& descriptor);
        /** Helper method for disconnect() */
        bool eraseConnection(ChannelDescriptor& descriptor);

        /** Mutex for when it is needed to resize the connections list */
        OS::Mutex connection_resize_mtx;
        /** Helper method for OutputPort<T>::createConnection */
        void addConnection(ChannelDescriptor const& descriptor);

    public:
        OutputPortInterface(std::string const& name);
        ~OutputPortInterface();

        virtual bool keepsLastWrittenValue() const = 0;
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        void disconnect(PortInterface& port);

        virtual void disconnect();

        virtual bool connected() const;

        /** Channelects this write port to the given read port, using a single-data
         * policy with the given locking mechanism */
        bool createDataConnection( InputPortInterface& sink, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Channelects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism */
        bool createBufferConnection( InputPortInterface& sink, int size, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Channelects this write port to the given read port, using the as policy
         * the default policy of the sink port
         */
        bool createConnection( InputPortInterface& sink );

        /** Channelects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( InputPortInterface& sink, ConnPolicy const& policy ) = 0;
    };
}

#endif
