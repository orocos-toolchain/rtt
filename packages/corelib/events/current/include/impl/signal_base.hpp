#ifndef ORO_SIGNAL_BASE_HPP
#define ORO_SIGNAL_BASE_HPP

#include <list>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

namespace sigslot
{
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
            void ref() { ++refcount; };
            /**
             * Decrease the reference count by one and delete this on zero.
             */
            void deref() { if ( --refcount <= 0 ) delete this; };

            connection_base(const connection_base&);
        public:
            connection_base(signal_base* sig)
                : mconnected(false), m_sig(sig), refcount(0)
            {
            }

            typedef boost::intrusive_ptr<connection_base> shared_ptr;

            bool connected() { return mconnected && m_sig; }
            bool connect();
            bool disconnect();
            void destroy();
        };

        void intrusive_ptr_add_ref( connection_base* p ) { p->ref(); }
        void intrusive_ptr_release( connection_base* p ) { p->deref(); }

        /**
         * The base signal class which stores connection objects.
         */
        class signal_base
        {
        public:
            typedef connection_base::shared_ptr        connection_t;
            typedef std::list< connection_t >    connections_list;
            typedef connections_list::iterator iterator;
            typedef connections_list::iterator const_iterator;
        private:
            friend class connection_base;

            void conn_connect( connection_t conn ) {
                assert( conn && "virtually impossible ! only connection base should call this function !" );
                mconnections.push_back( conn );
            }

            void conn_disconnect( connection_t conn ) {
                assert( conn && "virtually impossible ! only connection base should call this function !" );

                iterator tgt;

                if ( (tgt = std::find( mconnections.begin(),
                                       mconnections.end(),
                                       conn)) != mconnections.end() ) {
                    //mconnections.erase( tgt );
                    connection_t d(0);
                    *tgt = d; // clear out, no erase, keep all iterators valid !
                }
            }

        protected:
            /**
             * Erase all empty list items.
             */
            void cleanup() {
                iterator it = mconnections.begin();
                const_iterator itend = mconnections.end();
                while( it != itend ) {
                    iterator nxt_it = it;
                    ++nxt_it;
                    if ( !(*it) )
                        mconnections.erase( it );
                    it = nxt_it;
                }
            }

            connections_list mconnections;
        public:
            virtual ~signal_base(){
                // call destroy on all connections.
                while ( !mconnections.empty() ) {
                    if ( mconnections.front() )
                        mconnections.front()->destroy(); // this calls-back conn_disconnect.
                    mconnections.erase( mconnections.begin() );
                }
            }

        };

        bool connection_base::connect() { if( !m_sig ) return false; mconnected = true; m_sig->conn_connect(this); return true; }
        bool connection_base::disconnect() { if (!m_sig) return false; mconnected = false; m_sig->conn_disconnect(this); return true; }
        void connection_base::destroy() {
            if( !m_sig ) return;
            mconnected = false;
            signal_base* copy = m_sig;
            m_sig = 0; 
            copy->conn_disconnect(this);
            // after this point this object may be destructed !
        }


}
#endif
