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
#include "../ConnPolicy.hpp"
#include "../os/Mutex.hpp"
#include "../base/rtt-base-fwd.hpp"
#include "../base/ChannelElementBase.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>

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
         * in order to manage their channels.
         * TODO: use the mutex lock !!!
         */
        class RTT_API ConnectionManager
        {
        public:
            typedef boost::tuple<boost::shared_ptr<ConnID>, base::ChannelElementBase::shared_ptr> ChannelDescriptor;

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
            void addConnection(ConnID* port_id, base::ChannelElementBase::shared_ptr channel_input);

            void removeConnection(ConnID* port_id);

            /**
             * Disconnect all connections.
             */
            void disconnect();

            /** Returns true if there is at least one channel registered in this
             * port's list of outputs
             */
            bool connected() const;

            /** Removes the channel that connects this port to \c port */
            void disconnect(base::PortInterface* port);

            template<typename Pred>
            bool delete_if(Pred pred) {
                RTT::os::MutexLock lock(connection_lock);
                bool result = false;
                std::list<ChannelDescriptor>::iterator it = connections.begin();
                while (it != connections.end())
                {
                    if (pred(*it))
                    {
                        result = true;
                        it = connections.erase(it);
                    }
                    else ++it;
                }
                return result;
            }

            /**
             * Selects a connection as the current channel
             * if pred(connection) is true. It will first check
             * the current channel ( getCurrentChannel() ), if that
             * does not satisfy pred, iterate over \b all connections.
             * If none satisfy pred, the current channel remains unchanged.
             * @param pred
             */
            template<typename Pred>
            void select_if(Pred pred) {
                RTT::os::MutexLock lock(connection_lock);
                cur_channel = find_if(pred).second;
            }

            template<typename Pred>
            std::pair<bool, ChannelDescriptor> find_if(Pred pred) {
                ChannelDescriptor channel = cur_channel;
                if ( channel.get<1>() )
                    if ( pred( channel ) )
                        return std::make_pair(true, channel);

                // The boost reference to pred is required
                //boost::bind(&ConnectionManager::select_helper<Pred>, this, boost::ref(pred), boost::ref(found), _1)(cur_channel);
#ifdef MSVC
                std::list<ChannelDescriptor>::iterator result =
                    std::find_if(connections.begin(), connections.end(), pred);
#else
                std::list<ChannelDescriptor>::iterator result =
                    std::find_if(connections.begin(), connections.end(), pred);
#endif
                if (result == connections.end())
                    return std::make_pair(false, ChannelDescriptor());
                else
                    return std::make_pair(true, *result);
            }

            /**
             * Returns true if this manager manages only one connection.
             * @return
             */
            bool isSingleConnection() const { return connections.size() == 1; }

            /**
             * Returns the first added channel or if select_if was called, the selected channel.
             * @see select_if to change the current channel.
             * @return
             */
            base::ChannelElementBase* getCurrentChannel() const {
                return cur_channel.get<1>().get();
            }

            /**
             * Clears all connections.
             */
            void clear();

        protected:

            void updateCurrentChannel(bool reset_current);

            /** Helper method for disconnect(PortInterface*)
             *
             * This method removes the channel listed in \c descriptor from the list
             * of output channels if \c port has the same id that the one listed in
             * \c descriptor.
             *
             * @returns true if the descriptor matches, false otherwise
             */
            bool findMatchingPort(ConnID const* conn_id, ChannelDescriptor const& descriptor);

            /** Helper method for disconnect()
             *
             * Unconditionally removes the given connection and return true
             */
            bool eraseConnection(ChannelDescriptor& descriptor);

            /** os::Mutex for when it is needed to resize the connections list */
            os::Mutex connection_resize_mtx;

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
             * Optimisation in case only one channel is to be managed.
             */
            ChannelDescriptor cur_channel;

            /**
             * Lock that should be taken before the list of connections is
             * accessed or modified
             */
            RTT::os::Mutex connection_lock;
        };

    }

}

#endif /* CONNECTIONMANAGER_HPP_ */
