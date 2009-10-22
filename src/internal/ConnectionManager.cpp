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

        bool ConnectionManager::eraseIfMatchingPort(ConnID const* conn_id, ChannelDescriptor& descriptor)
        {
            if ( descriptor.get<0>() && conn_id->isSameID(*descriptor.get<0>()))
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
            boost::scoped_ptr<ConnID> conn_id( port.getPortID() );
            this->removeConnection(conn_id.get());
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


        void ConnectionManager::addConnection(ConnID* conn_id, ChannelElementBase::shared_ptr channel)
        {
            assert(conn_id);
            ChannelDescriptor descriptor = boost::make_tuple(conn_id, channel);
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
        }

        void ConnectionManager::removeConnection(ConnID* conn_id)
        {
            if (connections) {

                connections->delete_if( boost::bind(&ConnectionManager::eraseIfMatchingPort, this, conn_id, _1) );
                // reset current if it was deleted.
                checkDeletedConnections( cur_channel.get<1>() == 0 );
                return;
            } else {
                // single channel case
                if ( cur_channel.get<1>() )
                    eraseIfMatchingPort(conn_id, cur_channel);
            }

        }

        bool is_same_id(ConnID* conn_id, ConnectionManager::ChannelDescriptor const& channel)
        {
            return conn_id->isSameID( *channel.get<0>() );
        }

        ChannelElementBase::shared_ptr ConnectionManager::getConnection(const PortInterface& port)
        {
            boost::scoped_ptr<ConnID> conn_id( port.getPortID() );
            return connections->find_if( boost::bind(&is_same_id, conn_id.get(),_1) ).get<1>();
        }

    }

}
