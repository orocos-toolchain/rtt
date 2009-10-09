/*
 * ConnectionManager.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: kaltan
 */

#include "ConnectionManager.hpp"
#include <boost/bind.hpp>
#include "../base/PortInterface.hpp"
#include "../os/MutexLock.hpp"
#include "../base/InputPortInterface.hpp"

namespace RTT
{
    using namespace detail;

    namespace internal
    {

        ConnectionManager::ConnectionManager(PortInterface* port)
            : connections(0), mport(port)
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
        {
            if (connections) {
                connections->apply(boost::bind(&clearChannel, _1));
                return;
            } else {
                // single connection case
                if ( cur_channel.get<1>() ) {
                    clearChannel(cur_channel);
                }
            }
        }

        bool ConnectionManager::eraseIfMatchingPort(PortInterface const* port, ChannelDescriptor& descriptor)
        {
            if ( descriptor.get<0>() && port->isSameID(*descriptor.get<0>()))
            {
                // disconnect needs to know if we're from Out->In (forward) or from In->Out
                bool is_forward = true;
                if ( dynamic_cast<InputPortInterface*>(mport) )
                    is_forward = false; // disconnect on input port = backward.

                descriptor.get<1>()->disconnect(is_forward);
                // reset cur_channel if it is for deletion.
                if ( descriptor.get<1>() == cur_channel.get<1>() ) {
                    cur_channel = ChannelDescriptor();
                }
                return true;
            }
            else return false;
        }

        void ConnectionManager::checkDeletedConnections(bool reset_current)
        {
            // check if to delete connections
            if ( connections != 0 && connections->empty() ) {
                delete connections;
                connections = 0;
                cur_channel = ChannelDescriptor();
                return;
            }
            // only here in case of single or non empty connection.
            if (reset_current) {
                if (connections)
                    cur_channel = connections->front();
                else {
                    cur_channel = ChannelDescriptor();
                }
            }
        }

        void ConnectionManager::disconnect(PortInterface& port)
        {
            if (connections) {
                connections->delete_if( boost::bind(&ConnectionManager::eraseIfMatchingPort, this, &port, _1) );
                // reset current if it was deleted.
                checkDeletedConnections( cur_channel.get<1>() == 0 );
                return;
            } else {
                // single channel case
                if ( cur_channel.get<1>() )
                    eraseIfMatchingPort(&port, cur_channel);
            }
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
            // disconnects all
            if (connections) {
                // multi channel case
                connections->delete_if( boost::bind(&ConnectionManager::eraseConnection, this, _1) );
                checkDeletedConnections( false ); // no need to try to reset current.
                return;
            } else {
                // single channel case
                if ( cur_channel.get<1>() ) {
                    eraseConnection(cur_channel);
                    cur_channel = ChannelDescriptor();
                }
            }
        }

        bool ConnectionManager::connected() const
        { return cur_channel.get<1>(); }


        bool ConnectionManager::addConnection(PortID* port_id, ChannelElementBase::shared_ptr channel, ConnPolicy const& policy)
        {
            ChannelDescriptor descriptor = boost::make_tuple(port_id, channel);
            if ( cur_channel.get<1>() ) {
                // cur_channel, already in use, check if connections needs to be created.
                if (!connections) {
                    connections = new List< ChannelDescriptor >(2);
                    connections->append( cur_channel );
                }

                if (!connections->append(descriptor))
                {
                    os::MutexLock locker(connection_resize_mtx);
                    connections->grow(1);
                    connections->append(descriptor);
                }
            } else {
                cur_channel = descriptor;
            }

            if ( mport->connectionAdded(channel, policy) )
                return true;
            // cleanup.
            removeConnection( channel );
            return false;
        }

        bool is_same_id(PortInterface const& port, ConnectionManager::ChannelDescriptor const& channel)
        {
            return port.isSameID( *channel.get<0>() );
        }

        ChannelElementBase::shared_ptr ConnectionManager::getConnection(const PortInterface& port)
        {
            return connections->find_if( boost::bind(&is_same_id, boost::ref(port),_1) ).get<1>();
        }

        bool ConnectionManager::matchAndRemoveConnectionChannel(ChannelElementBase::shared_ptr channel, ChannelDescriptor& descriptor) const
        {
            if(channel == descriptor.get<1>()) {
                return true;
            }
            return false;
        }

        bool ConnectionManager::removeConnection(ChannelElementBase::shared_ptr channel)
        {
            // this code removes the channel from the connections list, re-sets cur_channel if necessary
            // and deletes the connections list if it is empty.
            bool removed = false;
            bool reset = false;
            if (cur_channel.get<1>() == channel )
                reset = true;
            if (connections) {
                if ( connections->delete_if( bind(&ConnectionManager::matchAndRemoveConnectionChannel, this, channel, _1) ) ) {
                    removed = true;
                }
            }
            checkDeletedConnections(reset);
            return removed;

        }


    }

}
