/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  ConnectionInterface.hpp 

                        ConnectionInterface.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_EXECUTION_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_CONNECTION_INTERFACE_HPP

#include <boost/intrusive_ptr.hpp>
#include "os/fosi.h"
#include "DataSourceBase.hpp"
#include "BufferBase.hpp"
#include <vector>

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
        oro_atomic_t refcount;

        typedef std::vector<PortInterface*> PList;
        PList ports;
        bool mconnected;
    public:
        typedef boost::intrusive_ptr<ConnectionInterface> shared_ptr;

        ConnectionInterface();
        virtual ~ConnectionInterface();

        /**
         * Get a Data source representing the current value of this
         * connection. This function always returns a meaningful data source.
         */
        virtual DataSourceBase::shared_ptr getDataSource() const = 0;

        /**
         * Returns the buffer and its associated status of this connection,
         * if this connection is buffered. Otherwise, this returns the null
         * pointer.
         */
        virtual BufferBase::shared_ptr getBuffer() const = 0;

        /**
         * Connect all readers and writers.
         * If a reader or writer is already connected to another
         * connection, this method fails.
         * @return false if at least one reader or writer was already
         * participating in a connection or if connected()
         */
        virtual bool connect();

        /**
         * Get Connection status.
         */
        virtual bool connected() const;

        /**
         * Disconnect all readers and writers.
         */
        virtual bool disconnect();

        /** 
         * Add a Port to the connection.
         * If this->connected(), also immediately connect \a p
         * to this connection.
         * 
         * @param p A port which wants to read or write data from this connection.
         * 
         * @return true if \a p has the correct type and could
         * be added
         */
        virtual bool addPort(PortInterface* p);

        /** 
         * Remove a Port from the connection.
         * If this->connected(), also immediately disconnect \a p
         * from this connection.
         * 
         * @param p A port which no longer wants to read or write data from this connection.
         * 
         * @return true if \a p was present.
         */
        virtual bool removePort(PortInterface* p);

        /**
         * Get the data type of this port.
         */
        virtual const TypeInfo* getTypeInfo() const = 0;

        /**
         * Returns the protocol over which this connection can be accessed.
         */
        virtual int serverProtocol() const {
            return 0;
        }

    };
}

#endif
