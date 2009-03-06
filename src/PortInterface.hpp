#ifndef ORO_EXECUTION_PORT_INTERRFACE_HPP
#define ORO_EXECUTION_PORT_INTERRFACE_HPP

#include <string>
#include <Connections.hpp>
#include <list>
#include <Events.hpp>

namespace RTT
{
    class ConnFactory;
    class TypeInfo;
    class TaskObject;
    class ConnElementBase;

    class PortInterface
    {
        std::string name;

    protected:
        PortInterface(const std::string& name);

    public:
        virtual ~PortInterface() {}

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

        /** The ConnFactory object that allows to build the ConnElement chain
         * needed to build connections to or from this port
         */
        ConnFactory* getConnFactory();

        /** Returns true if this port is located on this process, and false
         * otherwise
         */
        virtual bool isLocal() const;

        /**
         * Returns the protocol over which this port can be accessed.
         */
        virtual int serverProtocol() const;

        /**
         * Create a clone of this port with the same name
         */
        virtual PortInterface* clone() const = 0;

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const = 0;

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject();
    };

    class ReadPortInterface : public PortInterface
    {
    public:
        typedef Event<void(PortInterface*)> NewDataOnPortEvent;

    protected:
        ConnElementBase* writer;
        ConnPolicy default_policy;
        NewDataOnPortEvent* new_data_on_port_event;

    public:
        ReadPortInterface(std::string const& name, ConnPolicy const& default_policy);
        ~ReadPortInterface();

        ConnPolicy getDefaultPolicy() const;

        /** Removes any connection that either go to or come from this port */
        virtual void disconnect();

        /** Returns true if this port is connected */
        virtual bool connected() const;

        /** Clears the connection. After call to read() will return false after
         * clear() has been called
         */
        void clear();

        NewDataOnPortEvent* getNewDataOnPortEvent();
    };

    class WritePortInterface : public PortInterface
    {
    protected:
        typedef boost::tuple<PortInterface*, ConnElementBase::shared_ptr, ConnPolicy> ConnDescriptor;
        std::list< ConnDescriptor > connections;

    public:
        WritePortInterface(std::string const& name);
        ~WritePortInterface();

        virtual bool keepsLastWrittenValue() const = 0;
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        void disconnect(PortInterface& port);

        virtual void disconnect();

        virtual bool connected() const;

        /** Connects this write port to the given read port, using a single-data
         * policy with the given locking mechanism */
        bool createDataConnection( ReadPortInterface& sink, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism */
        bool createBufferConnection( ReadPortInterface& sink, int size, int lock_policy = ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using the as policy
         * the default policy of the sink port
         */
        bool createConnection( ReadPortInterface& sink );

        /** Connects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( ReadPortInterface& sink, ConnPolicy const& policy ) = 0;
    };
}

#endif
