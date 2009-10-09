/*
 * ConnectionManager.hpp
 *
 *  Created on: Oct 9, 2009
 *      Author: kaltan
 */

#ifndef CONNECTIONMANAGER_HPP_
#define CONNECTIONMANAGER_HPP_


#include "../base/rtt-base-fwd.hpp"
#include "../internal/ConnPolicy.hpp"
#include "../base/ChannelElementBase.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "../internal/List.hpp"
#include "../os/Mutex.hpp"


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
        class ConnectionManager
        {
        public:
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
            bool addConnection(base::PortID* port_id, base::ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy);

            /**
             * Returns the connection to a given port.
             * The lookup is done by PortID.
             * @param port The port to use for lookup.
             * @return A connection to \a port or nill if no such connection exists.
             */
            base::ChannelElementBase::shared_ptr getConnection(const base::PortInterface& port);

            /**
             * Disconnect all connections.
             */
            void disconnect();

            /** Returns true if there is at least one channel registered in this
             * port's list of outputs
             */
            bool connected() const;

            /** Removes the channel that connects this port to \c port */
            void disconnect(base::PortInterface& port);

            /** Removes the connection associated with this channel, and the channel
             * as well
             */
            bool removeConnection(base::ChannelElementBase::shared_ptr channel);

            template<typename Pred>
            bool delete_if(Pred pred) {
                if (connections)
                    return connections->delete_if(pred);
                if ( cur_channel.get<1>() )
                    if ( pred( cur_channel ) ) {
                        // delete
                        cur_channel.get<1>().reset();
                        return true;
                    }
                return false;
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
                if ( cur_channel.get<1>() )
                    if ( pred( cur_channel ) )
                        return;
                bool found;
                // The boost reference to pred is required
                //boost::bind(&ConnectionManager::select_helper<Pred>, this, boost::ref(pred), boost::ref(found), _1)(cur_channel);
                if (connections)
                    connections->apply(boost::bind(&ConnectionManager::select_helper<Pred>, this, boost::ref(pred), boost::ref(found), _1));
            }

            /**
             * Returns true if this manager manages only one connection.
             * @return
             */
            bool isSingleConnection() const { return connections == 0 && cur_channel.get<1>(); }

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

            typedef boost::tuple<boost::shared_ptr<base::PortID>, base::ChannelElementBase::shared_ptr> ChannelDescriptor;
        protected:

            /**
             * Helper method for select_if
             * @param pred The user's predicate
             * @param descriptor the channel to check using the predicate.
             * @param found will be set to true if the selection process should stop because
             * a match was found.
             */
            template<typename Pred>
            void select_helper(Pred pred, bool& found, const ChannelDescriptor& descriptor) {
                if ( !found && pred(descriptor) ) {
                    cur_channel = descriptor;
                    found = true;
                }
            }

            void checkDeletedConnections(bool reset_current);

            /** Helper method for disconnect(PortInterface*)
             *
             * This method removes the channel listed in \c descriptor from the list
             * of output channels if \c port has the same id that the one listed in
             * \c descriptor.
             *
             * @returns true if the descriptor matches, false otherwise
             */
            bool eraseIfMatchingPort(base::PortInterface const* port, ChannelDescriptor& descriptor);

            /** Helper method for disconnect()
             *
             * Unconditionally removes the given connection and return true
             */
            bool eraseConnection(ChannelDescriptor& descriptor);

            /** Helper method for removeConnection(channel) */
            bool matchAndRemoveConnectionChannel(base::ChannelElementBase::shared_ptr channel, ChannelDescriptor& descriptor) const;

            /** os::Mutex for when it is needed to resize the connections list */
            os::Mutex connection_resize_mtx;

            base::PortInterface* mport;

            List< ChannelDescriptor >* connections;

            /**
             * Optimisation in case only one channel is to be managed.
             */
            ChannelDescriptor cur_channel;
        };

    }

}

#endif /* CONNECTIONMANAGER_HPP_ */
