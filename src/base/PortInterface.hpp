#ifndef ORO_EXECUTION_PORT_INTERFACE_HPP
#define ORO_EXECUTION_PORT_INTERFACE_HPP

#include <string>
#include "../internal/rtt-internal-fwd.hpp"
#include "../ConnPolicy.hpp"
#include "../internal/ConnID.hpp"
#include "ChannelElementBase.hpp"
#include "../types/rtt-types-fwd.hpp"
#include "../interface/rtt-interface-fwd.hpp"

namespace RTT
{ namespace base {

    /**
     * The base class of every data flow port.
     */
    class RTT_API PortInterface
    {
        std::string name;
        interface::DataFlowInterface* iface;
    protected:
        PortInterface(const std::string& name);

    public:
        virtual ~PortInterface() {}

        /**
         * Returns the identity of this port in a ConnID object.
         */
        virtual internal::ConnID* getPortID() const;

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

        /** Returns the types::TypeInfo object for the port's type */
        virtual const types::TypeInfo* getTypeInfo() const = 0;

        /** Removes any connection that either go to or come from this port */
        virtual void disconnect() = 0;

        /** The ChannelFactory object that allows to build the ChannelElement chain
         * needed to build connections to or from this port
         */
        virtual internal::ConnFactory* getConnFactory();

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
        virtual internal::TaskObject* createPortObject();

        /** Connects this port with \a other, using the given policy. Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        virtual bool connectTo(PortInterface& other, ConnPolicy const& policy) = 0;

        /** Connects this port with \a other, using the default policy of the input. Unlike
         * OutputPortInterface::createConnection, \a other can be the write port
         * and \c this the read port.
         *
         * @returns true on success, false on failure
         */
        virtual bool connectTo(PortInterface& other) = 0;

        /**
         * Creates a data stream from or to this port using connection-less transports.
         * Typically, policy.transport and policy.name_id must be properly filled in
         * such that the data stream can be set up and input and output port can find each other.
         * You need to call this method on two ports (input and output) using the same transport
         * and (probably) same name_id.
         * @param policy The connection policy describing how the stream must be set up.
         */
        virtual bool createStream(ConnPolicy const& policy) = 0;

        /**
         * Adds a user created connection to this port.
         * This is an advanced method, prefer to use connectTo and createStream.
         */
        virtual bool addConnection(internal::ConnID* cid, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy = ConnPolicy() ) = 0;

        /**
         * Removes a user created connection from this port.
         * This is an advanced method, prefer to use disconnect()
         * or a method from a subclass of PortInterface.
         */
        virtual void removeConnection(internal::ConnID* cid) = 0;

        /**
         * Once a port is added to a DataFlowInterface, it gets
         * a pointer to that interface.
         * This allows advanced ports to track back to which component
         * they belong.
         */
        void setInterface(interface::DataFlowInterface* iface);
        /**
         * Returns the DataFlowInterface this port belongs to or null if it was not added
         * to such an interface.
         */
        interface::DataFlowInterface* getInterface() const;
};

}}

#endif
