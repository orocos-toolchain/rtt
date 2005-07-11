#ifndef ORO_SIGNAL_BASE_HPP
#define ORO_SIGNAL_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>
#include <os/Mutex.hpp>

#define ORO_SIGNAL_USE_RT_LIST

#ifdef ORO_SIGNAL_USE_RT_LIST
#include <os/rt_list.hpp>
#else
#include <list>
#endif

namespace sigslot
{
    namespace detail {
        class signal_base;

        /**
         * A connection may be connected or destroyed.
         * It is the central object which knows about one signal
         * and its slot (the latter in its derived classes).
         */
        class connection_base
        {
            friend void intrusive_ptr_add_ref(connection_base*);
            friend void intrusive_ptr_release(connection_base*);
            bool mconnected;
            signal_base* m_sig;
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
         */
        class signal_base
        {
        public:
            typedef connection_base::shared_ptr        connection_t;
#ifdef ORO_SIGNAL_USE_RT_LIST
            typedef ORO_OS::rt_list< connection_t >    connections_list;
#else
            typedef std::list< connection_t > connections_list;
#endif
            typedef connections_list::iterator iterator;
            typedef connections_list::iterator const_iterator;
        protected:
            friend class connection_base;

            void conn_setup( connection_t conn );

            void conn_connect( connection_t conn );

            void conn_disconnect( connection_t conn );

            void conn_destroy( connection_t conn );
        protected:
            /**
             * Erase all empty list items after emit().
             */
            void cleanup();

            ORO_OS::MutexRecursive m;
            connections_list mconnections;
            iterator itend;
#ifdef ORO_SIGNAL_USE_RT_LIST
            int disconcount;
#else
            int concount;
#endif
            bool emitting;
            signal_base();
        public:
            virtual ~signal_base();
        };
    }// namespace detail

}
#endif
