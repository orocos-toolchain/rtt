/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  ConnectionInterface.hpp 

                        ConnectionInterface.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_CONNECTION_INTERFACE_HPP

#include <boost/intrusive_ptr.hpp>
#include "os/fosi.h"
#include "DataSourceBase.hpp"
#ifdef OROPKG_CORBA
#include "tao/corba.h"
#endif

namespace RTT
{
    class ConnectionInterface;
}

void intrusive_ptr_add_ref( RTT::ConnectionInterface* p );
void intrusive_ptr_release( RTT::ConnectionInterface* p );

namespace RTT
{
    class PortInterface;

    /**
     * An Object which manages and maintains a communication connection
     * between Ports.
     *
     * @par Creating a Connection
     * When connections are created, they are initially not connected,
     * and hence, no port 'knows' of this connection object. In order
     * to establish the connection between ports, call connect() explicitly on the newly created
     * connection object. As long as that does not happen, it only keeps
     * track of readers and writers, but does not connect them.
     *
     * @par Connection Management
     * A connection object has two states: connected() or !connected().
     * In both states, any number of readers and writers may subscribe
     * to this connection. If the state is !connected(), all subscribers
     * will only be connected when the connect() method is called.
     * If the state is connected(), every newly added subscriber is
     * directly connected to the connection.
     *
     * It is possible that a PortInterface participates in multiple
     * 'dormant' (ie !connected()) connections while only one connection
     *  is connected() at the same time. If one tries to connect() a
     * connection in which one ore more subscribers are already participating
     * in the connection, the connect() method fails.
     * 
     * @par Reference counting
     * ConnectionInterface objects are reference counted. That means that
     * if no object holds a pointer to this object, it will be destroyed.
     * This can only happen if all ports disconnect from this connection.
     *
     * @par Thread Safety
     * Although this interface may be implemented thread-safe, the DataConnection
     * and BufferConnection classes have not been made thread-safe. This means that
     * multiple threads calling connect()/disconnect() on the same or related
     * connections may crash the system. Since connection setup is seen as
     * an application configuration step, executed by a single configurator, 
     * Locking was not added. If this policy does not match your application setup,
     * write your own ConnectionInterface subclass, or submit a bug report.
     *
     * @see PortInterface
     */
    class ConnectionInterface
    {
    protected:
        friend void ::intrusive_ptr_add_ref( ConnectionInterface* p );
        friend void ::intrusive_ptr_release( ConnectionInterface* p );
        atomic_t refcount;
    public:
        
        typedef boost::intrusive_ptr<ConnectionInterface> shared_ptr;

        ConnectionInterface();
        virtual ~ConnectionInterface();

        /**
         * Get a Data source representing the current value of this
         * connection.
         */
        virtual DataSourceBase::shared_ptr getDataSource() const = 0;

        /**
         * Connect all readers and writers.
         * If a reader or writer is already connected to another
         * connection, this method fails.
         * @return false if at least one reader or writer was already
         * participating in a connection or if connected()
         */
        virtual bool connect() = 0;

        /**
         * Get Connection status.
         */
        virtual bool connected() const = 0;

        /**
         * Disconnect all readers and writers.
         */
        virtual bool disconnect() = 0;

        /** 
         * Add a Port as possible reader (consumer) of the connection.
         * If this->connected(), also immediately connect \a r
         * to this connection.
         * 
         * @param r A port which wants to read data from this connection.
         * 
         * @return true if \a r has the correct type and could
         * be added
         */
        virtual bool addReader(PortInterface* r) = 0;

        /** 
         * Remove a Port as possible reader (consumer) of the connection.
         * If this->connected(), also immediately disconnect \a r
         * from this connection.
         * 
         * @param r A port which no longer wants to read data from this connection.
         * 
         * @return true if \a r was present.
         */
        virtual bool removeReader(PortInterface* r) = 0;

        /** 
         * Add a Port as possible writer (producer) of the connection.
         * If this->connected(), also immediately connect \a w
         * to this connection.
         * 
         * @param w A port which wants to write data into this connection.
         * 
         * @return true if \a w has the correct type and could
         * be added.
         */
        virtual bool addWriter(PortInterface* w) = 0;

        /** 
         * Remove a Port as possible writer (producer) of the connection.
         * If this->connected(), also immediately disconnect \a w
         * from this connection.
         * 
         * @param w A port which no longer wants to write data into this connection.
         * 
         * @return true if \a w was present.
         */
        virtual bool removeWriter(PortInterface* w) = 0;

#ifdef OROPKG_CORBA
        /**
         * Create a corba object which exports this connection to a Corba Object.
         */
        virtual CORBA::Object_ptr toChannel();
#endif
    };
}

#endif
