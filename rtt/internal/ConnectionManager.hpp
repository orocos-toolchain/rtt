/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnectionManager.hpp

                        ConnectionManager.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


/*
 * ConnectionManager.hpp
 *
 *  Created on: Oct 9, 2009
 *      Author: kaltan
 */

#ifndef CONNECTIONMANAGER_HPP_
#define CONNECTIONMANAGER_HPP_


#include "ConnID.hpp"
#include "List.hpp"
#include "SharedConnection.hpp"
#include "../ConnPolicy.hpp"
#include "../os/Mutex.hpp"
#include "../base/rtt-base-fwd.hpp"
#include "../base/ChannelElementBase.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <rtt/os/Mutex.hpp>
#include <rtt/os/MutexLock.hpp>
#include <list>


namespace RTT
{

    namespace internal
    {
        /**
         * Manages connections between ports.
         * This class is used for input and output ports
         * in order to manage their connections.
         * TODO: use the mutex lock !!!
         */
        class RTT_API ConnectionManager
        {
        public:
            /**
             * A connection is described by an opaque ConnID object,
             * the first element of the connection and the policy of the connection.
             * The policy is only given for read-only access, modifying it will
             * not have any effect on the connection.
             */
            typedef boost::tuple<boost::shared_ptr<ConnID>, base::ChannelElementBase::shared_ptr, ConnPolicy> ChannelDescriptor;

            typedef std::list<ChannelDescriptor> Connections;

            /**
             * Creates a connection manager to manage the connections of \a port.
             * @param port The port whose connections to manage.
             *
             */
            ConnectionManager(base::PortInterface* port);
            ~ConnectionManager();

            /** Helper method for port-to-port connection establishment.
             * This is the last step in adding a connection to an output port and
             * also validates if the connection is sound.
             * @return false if the connection failed to work, true otherwise.
             */
            bool addConnection(ConnID* port_id, base::ChannelElementBase::shared_ptr channel, ConnPolicy policy);

            bool removeConnection(ConnID* port_id, bool disconnect = true);
            bool removeConnection(base::ChannelElementBase* channel, bool disconnect = true);

            /**
             * Disconnect all connections.
             */
            void disconnect();

            /** Returns true if there is at least one connection registered in this
             * port's list of outputs
             */
            bool connected() const;

            /** Returns true if there exists a connection to the given port
             */
            bool connectedTo(base::PortInterface* port);


            /** Removes the connection that connects this port to \c port */
            bool disconnect(base::PortInterface* port);

            /**
             * Returns true if this manager manages only one connection.
             * @return
             */
            bool isSingleConnection() const { return connections.size() == 1; }

            /**
             * Returns a list of all connections managed by this object.
             */
            Connections getConnections() const {
                return connections;
            }

            /**
             * Returns a pointer to the shared connection element this port may be connected to.
             */
            internal::SharedConnectionBase::shared_ptr getSharedConnection() const {
                return shared_connection;
            }

            /**
             * Locks the mutex protecting the channel element list.
             * */
            void lock() const {
                connection_lock.lock();
            };

            /**
             * Unlocks the mutex protecting the channel element list.
             * */
            void unlock() const {
                connection_lock.unlock();
            }
        protected:

            /** Helper method for disconnect()
             *
             * Unconditionally removes the given connection and returns the next connection in the list or connections.end()
             */
            Connections::iterator eraseConnection(const Connections::iterator& descriptor, bool disconnect);

            /**
             * The port for which we manage connections.
             */
            base::PortInterface* mport;

            /**
             * A list of all our connections. Only non-null if two or more connections
             * were added.
             */
            std::list< ChannelDescriptor > connections;

            /**
             * A pointer to the shared connection this port may be connected to.
             */
            internal::SharedConnectionBase::shared_ptr shared_connection;

            /**
             * Lock that should be taken before the list of connections is
             * accessed or modified
             */
            mutable RTT::os::Mutex connection_lock;
        };

    }

}

#endif /* CONNECTIONMANAGER_HPP_ */
