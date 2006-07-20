/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  signal_base.hpp 

                        signal_base.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_SIGNAL_BASE_HPP
#define ORO_SIGNAL_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

/**
 * This is the most reliable real-time
 * connection list policy.
 */
#define ORO_SIGNAL_USE_LIST_LOCK_FREE

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
#include "../ListLockFree.hpp"
#include <boost/shared_ptr.hpp>
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
#include "../os/Mutex.hpp"
#include "../os/rt_list.hpp"
#else
#include "os/Mutex.hpp"
#include <list>
#endif
#endif

namespace sigslot
{
    namespace detail {
        class signal_base;

        /**
         * A connection 'memorises' the connection between an
         * event and an event handler function.
         * It manages connection and destruction of a signal/slot pair.
         * It is the central object which knows about one signal
         * and its slot (the latter in its derived classes).
         */
        class connection_base
        {
            friend void intrusive_ptr_add_ref(connection_base*);
            friend void intrusive_ptr_release(connection_base*);
            bool mconnected;
            signal_base* m_sig;

            /**
             * We require an internal refcount to ease self-addition
             * and removal of this connection.
             */
            int refcount;

            /**
             * Increase the reference count by one.
             */
            void ref();
            /**
             * Decrease the reference count by one and delete this on zero.
             */
            void deref();

            connection_base(const connection_base&);
        protected:
            virtual ~connection_base();
        public:
            connection_base(signal_base* sig);

            typedef boost::intrusive_ptr<connection_base> shared_ptr;

            inline bool connected() { return mconnected && m_sig; }
            bool connect();
            bool disconnect();
            void destroy();
        };

        void intrusive_ptr_add_ref( connection_base* p );
        void intrusive_ptr_release( connection_base* p );

        /**
         * The base signal class which stores connection objects.
         * It implements real-time management of connections, such that
         * connection/disconnetion of a handler is always thread-safe
         * and real-time.
         */
        class signal_base
        {
        public:
            typedef connection_base::shared_ptr        connection_t;
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            typedef RTT::ListLockFree<connection_t> connections_list;
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
            typedef RTT::OS::rt_list< connection_t >    connections_list;
#else
            typedef std::list< connection_t > connections_list;
#endif
            typedef connections_list::iterator iterator;
            typedef connections_list::iterator const_iterator;
#endif
        protected:
            friend class connection_base;

            void conn_setup( connection_t conn );

            void conn_connect( connection_t conn );

            void conn_disconnect( connection_t conn );

            void conn_destroy( connection_t conn );
        protected:
            connections_list mconnections;
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            // no mutexes involved
#else
            /**
             * Erase all empty list items after emit().
             */
            void cleanup();

            RTT::OS::MutexRecursive m;
            iterator itend;
#ifdef ORO_SIGNAL_USE_RT_LIST
            int disconcount;
#else
            int concount;
#endif
#endif
            bool emitting;
            signal_base();
        public:
            virtual ~signal_base();

            /**
             * Reserves memory for a number of connections.
             * Use this method to efficiently reserve memory for
             * possible connections. If not used, the event will
             * reserve memory in batch, depending upon demand.
             * This does not impair/influence real-time performance, only
             * memory efficiency.
             * @param conns The number of connections to reserve memory for.
             */
            void reserve(size_t conns);
            /**
             * Returns the arity (the number of arguments) of this signal.
             */
            virtual int arity() const = 0;
        };
    }// namespace detail

}
#endif
