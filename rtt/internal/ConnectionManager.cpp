/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnectionManager.cpp

                        ConnectionManager.cpp -  description
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
 * ConnectionManager.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: kaltan
 */

#include "ConnectionManager.hpp"
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include "../base/PortInterface.hpp"
#include "../os/MutexLock.hpp"
#include "../base/InputPortInterface.hpp"
#include "../Logger.hpp"
#include <cassert>

namespace RTT
{
    using namespace detail;

    namespace internal
    {

        ConnectionManager::ConnectionManager(PortInterface* port)
            : mport(port)
        {
        }

        ConnectionManager::~ConnectionManager()
        {
            this->disconnect();
        }

        bool ConnectionManager::disconnect(PortInterface* port)
        {
            boost::scoped_ptr<ConnID> conn_id( port->getPortID() );
            return this->removeConnection(conn_id.get(), /* disconnect = */ true);
        }

        ConnectionManager::Connections::iterator ConnectionManager::eraseConnection(const Connections::iterator& descriptor, bool disconnect)
        {
            base::ChannelElementBase::shared_ptr channel = descriptor->get<1>();

            // disconnect from a shared connection
            if (channel == shared_connection) {
                RTT::log(Debug) << "Port " << mport->getName() << " disconnected from shared connection " << shared_connection->getName() << RTT::endlog();
                shared_connection.reset();
            }
            Connections::iterator next = connections.erase(descriptor); // invalidates descriptor

            if (disconnect) {
                // disconnect needs to know if we're from Out->In (forward) or from In->Out
                bool is_forward = true;
                if ( dynamic_cast<InputPortInterface*>(mport) )
                    is_forward = false; // disconnect on input port = backward.

                mport->getEndpoint()->disconnect(channel, is_forward);
            }

            return next;
        }

        void ConnectionManager::disconnect()
        {
            RTT::os::MutexLock lock(connection_lock);
            for(Connections::iterator conn_it = connections.begin(); conn_it != connections.end(); ) {
                conn_it = eraseConnection(conn_it, true);
            }
        }

        bool ConnectionManager::connected() const
        { return !connections.empty(); }

        bool ConnectionManager::connectedTo(base::PortInterface* port)
        {
            RTT::os::MutexLock lock(connection_lock);
            boost::scoped_ptr<ConnID> conn_id( port->getPortID() );
            for(Connections::const_iterator conn_it = connections.begin(); conn_it != connections.end(); ++conn_it ) {
                if (conn_it->get<0>() && conn_id->isSameID(*conn_it->get<0>())) return true;
            }
            return false;
        }

        bool ConnectionManager::addConnection(ConnID* conn_id, ChannelElementBase::shared_ptr channel, ConnPolicy policy)
        { RTT::os::MutexLock lock(connection_lock);
            assert(conn_id);

            // check if the new connection is a shared connection
            {
                SharedConnectionBase::shared_ptr is_shared_connection = boost::dynamic_pointer_cast<SharedConnectionBase>(channel);
                if (is_shared_connection) shared_connection.swap(is_shared_connection);
            }

            // add ChannelDescriptor to the connections list
            ChannelDescriptor descriptor = boost::make_tuple(conn_id, channel, policy);
            connections.push_back(descriptor);

            return true;
        }

        bool ConnectionManager::removeConnection(ConnID* conn_id, bool disconnect /* = true */)
        {
            RTT::os::MutexLock lock(connection_lock);
            bool found = false;
            for(Connections::iterator conn_it = connections.begin(); conn_it != connections.end(); ) {
                if (conn_it->get<0>() && conn_id->isSameID(*conn_it->get<0>())) {
                    conn_it = eraseConnection(conn_it, disconnect);
                    found = true;
                } else {
                    conn_it++;
                }
            }
            return found;
        }

        bool ConnectionManager::removeConnection(base::ChannelElementBase* channel, bool disconnect /* = true */)
        {
            RTT::os::MutexLock lock(connection_lock);
            bool found = false;
            for(Connections::iterator conn_it = connections.begin(); conn_it != connections.end(); ) {
                if (conn_it->get<1>() && channel == conn_it->get<1>()) {
                    conn_it = eraseConnection(conn_it, disconnect);
                    found = true;
                } else {
                    conn_it++;
                }
            }
            return found;
        }

    }

}
