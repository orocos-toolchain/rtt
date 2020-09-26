/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  SignalBase.hpp

                        SignalBase.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_SIGNAL_BASE_HPP
#define ORO_SIGNAL_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

#include "rtt/rtt-config.h"

#if defined(OROBLD_OS_NO_ASM)
#define ORO_SIGNAL_USE_RT_LIST
#else
#define ORO_SIGNAL_USE_LIST_LOCK_FREE
#endif

#include "rtt/os/Atomic.hpp"
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
#include "rtt/internal/ListLockFree.hpp"
#include <boost/shared_ptr.hpp>
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
#include "rtt/os/Mutex.hpp"
#include "rtt/os/rt_list.hpp"
#else
#include "rtt/os/Mutex.hpp"
#include <list>
#endif
#endif

namespace RTT
{
    namespace internal {

        class SignalBase;

        /**
         * A connection 'memorises' the connection between an
         * event and an event handler function.
         * It manages connection and destruction of a signal/slot pair.
         * It is the central object which knows about one signal
         * and its slot (the latter in its derived classes).
         */
        class RTT_API ConnectionBase
        {
        protected:
            RTT_API friend void intrusive_ptr_add_ref(ConnectionBase*);
            RTT_API friend void intrusive_ptr_release(ConnectionBase*);
            bool mconnected;
            SignalBase* m_sig;

            /**
             * We require an internal refcount to ease self-addition
             * and removal of this connection.
             */
            os::AtomicInt refcount;

            /**
             * Increase the reference count by one.
             */
            void ref();
            /**
             * Decrease the reference count by one and delete this on zero.
             */
            void deref();

        protected:
            virtual ~ConnectionBase();
        public:
            ConnectionBase(SignalBase* sig);

            typedef boost::intrusive_ptr<ConnectionBase> shared_ptr;

            inline bool connected() { return mconnected && m_sig; }
            bool connect();
            bool disconnect();
            void destroy();
        private:
            ConnectionBase(const ConnectionBase& );
            const ConnectionBase& operator=( const ConnectionBase& );
        };

        RTT_API void intrusive_ptr_add_ref( ConnectionBase* p );
        RTT_API void intrusive_ptr_release( ConnectionBase* p );

        /**
         * The base signal class which stores connection objects.
         * It implements real-time management of connections, such that
         * connection/disconnetion of a handler is always thread-safe
         * and real-time.
         */
        class RTT_API SignalBase
        {
        public:
            typedef ConnectionBase::shared_ptr        connection_t;
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            typedef ListLockFree<connection_t> connections_list;
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
            typedef RTT::os::rt_list< connection_t >    connections_list;
#else
            typedef std::list< connection_t > connections_list;
#endif
            typedef connections_list::iterator iterator;
            typedef connections_list::iterator const_iterator;
#endif
        protected:
            friend class ConnectionBase;

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

            RTT::os::MutexRecursive m;
            iterator itend;
#ifdef ORO_SIGNAL_USE_RT_LIST
            int disconcount;
#else
            int concount;
#endif
#endif
            bool emitting;
            SignalBase();
        public:
            /**
             * Deletes this Signal and will destroy all
             * connections and handles of this object. It is
             * not necessary to call disconnect() or disconnect() prior to
             * destroying a SignalBase.
             */
            virtual ~SignalBase();

            /**
             * Disconnects all connections.
             * All Handles will be disconnected, but able to reconnect.
             */
            void disconnect();

            /**
             * Destroys all connections.
             * This will destroy all connections of this Event.
             * All Handles will be disconnected and unable to reconnect.
             */
            void destroy();

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
        private:
            SignalBase(const SignalBase& );
            const SignalBase& operator=( const SignalBase& );

        };
    }// namespace detail

}
#endif
