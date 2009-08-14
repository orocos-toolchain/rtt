#ifndef ORO_EXECUTION_PORT_INTERFACE_HPP
#define ORO_EXECUTION_PORT_INTERFACE_HPP

#include <string>
#include "../internal/Channels.hpp"
#include "../internal/ConnPolicy.hpp"

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
    class RTT_API PortID
    {
    public:
        virtual ~PortID() {}
    };

    class RTT_API PortInterface
    {
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

}

#endif
