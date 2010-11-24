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

        /**
         * Helper function to clear a connection.
         * @param descriptor
         */
        void clearChannel(ConnectionManager::ChannelDescriptor& descriptor) {
            descriptor.get<1>()->clear();
        }

        void ConnectionManager::clear()
        { RTT::os::MutexLock lock(connection_lock);
            std::for_each(connections.begin(), connections.end(), &clearChannel);
        }

        bool ConnectionManager::findMatchingPort(ConnID const* conn_id, ChannelDescriptor const& descriptor)
        {
            return ( descriptor.get<0>() && conn_id->isSameID(*descriptor.get<0>()));
        }

        void ConnectionManager::updateCurrentChannel(bool reset_current)
        {
            if (connections.empty())
                cur_channel = ChannelDescriptor();
            else if (reset_current)
                cur_channel = connections.front();
        }

        bool ConnectionManager::disconnect(PortInterface* port)
        {
            boost::scoped_ptr<ConnID> conn_id( port->getPortID() );
            return this->removeConnection(conn_id.get());
        }

        bool ConnectionManager::eraseConnection(ConnectionManager::ChannelDescriptor& descriptor)
        {
            // disconnect needs to know if we're from Out->In (forward) or from In->Out
            bool is_forward = true;
            if ( dynamic_cast<InputPortInterface*>(mport) )
                is_forward = false; // disconnect on input port = backward.

            descriptor.get<1>()->disconnect( is_forward );
            return true;
        }

        void ConnectionManager::disconnect()
        {
            std::list<ChannelDescriptor> all_connections;
            { RTT::os::MutexLock lock(connection_lock);
                all_connections.splice(all_connections.end(), connections);
                cur_channel = ChannelDescriptor();
            }
            std::for_each(all_connections.begin(), all_connections.end(),
                    boost::bind(&ConnectionManager::eraseConnection, this, _1));
        }

        bool ConnectionManager::connected() const
        { return !connections.empty(); }


        void ConnectionManager::addConnection(ConnID* conn_id, ChannelElementBase::shared_ptr channel)
        { RTT::os::MutexLock lock(connection_lock);
            assert(conn_id);
            ChannelDescriptor descriptor = boost::make_tuple(conn_id, channel);
            if (connections.empty())
                cur_channel = descriptor;
            connections.push_back(descriptor);
        }

        bool ConnectionManager::removeConnection(ConnID* conn_id)
        {
            ChannelDescriptor descriptor;
            { RTT::os::MutexLock lock(connection_lock);
                std::list<ChannelDescriptor>::iterator conn_it =
                    std::find_if(connections.begin(), connections.end(), boost::bind(&ConnectionManager::findMatchingPort, this, conn_id, _1));
                if (conn_it == connections.end())
                    return false;
                descriptor = *conn_it;
                connections.erase(conn_it);
                updateCurrentChannel( cur_channel.get<1>() == descriptor.get<1>() );
            }

            // disconnect needs to know if we're from Out->In (forward) or from In->Out
            bool is_forward = true;
            if ( dynamic_cast<InputPortInterface*>(mport) )
                is_forward = false; // disconnect on input port = backward.

            descriptor.get<1>()->disconnect(is_forward);
            return true;
        }

        bool is_same_id(ConnID* conn_id, ConnectionManager::ChannelDescriptor const& channel)
        {
            return conn_id->isSameID( *channel.get<0>() );
        }

    }

}
