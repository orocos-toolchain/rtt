#include "corelib/impl/signal_base.hpp"
#include <os/MutexLock.hpp>

namespace sigslot {
    namespace detail {

        // connection_base

        void intrusive_ptr_add_ref( connection_base* p ) { p->ref(); }
        void intrusive_ptr_release( connection_base* p ) { p->deref(); }

        void connection_base::ref() { ++refcount; };
        void connection_base::deref() { if ( --refcount <= 0 ) delete this; };

        connection_base::connection_base(signal_base* sig)
            : mconnected(false), m_sig(sig), refcount(0)
        {
        }

        connection_base::~connection_base() {}

        bool connection_base::connect() {
            if( !m_sig ) return false;
            mconnected = true;
            m_sig->conn_connect(this);
            return true;
        }
        bool connection_base::disconnect() {
            if (!m_sig) return false;
            mconnected = false;
            m_sig->conn_disconnect(this);
            return true;
        }
        void connection_base::destroy() {
            if( !m_sig ) return;
            mconnected = false;
            signal_base* copy = m_sig;
            m_sig = 0; 
            copy->conn_destroy(this);
            // after this point this object may be destructed !
        }

        // signal_base

        void signal_base::conn_setup( connection_t conn ) {
            // allocate empty slot in list.
#ifdef ORO_SIGNAL_USE_RT_LIST
            mconnections.rt_grow(1);
#else
            connection_t d(0);
            mconnections.push_back( d );
#endif
        }

        void signal_base::conn_connect( connection_t conn ) {
            assert( conn && "virtually impossible ! only connection base should call this function !" );
            // derived class must make sure that list contained enough list items !
            //assert( itend != mconnections.end() );

            ORO_OS::MutexLock lock(m);

#ifdef ORO_SIGNAL_USE_RT_LIST
            mconnections.push_back( conn );
#else
            iterator tgt;
            connection_t empty;
            // find empty slot
            if ( (tgt = std::find( mconnections.begin(),
                                   mconnections.end(),
                                   empty )) != mconnections.end() ) {
                *tgt = conn;
            }
#endif
        }

        void signal_base::conn_destroy( connection_t conn ) {
            this->conn_disconnect(conn);
            // increase number of connections destroyed.
#ifdef ORO_SIGNAL_USE_RT_LIST
            // free memory
            mconnections.rt_shrink(1);
#else
            ++concount;
#endif
        }

        void signal_base::conn_disconnect( connection_t conn ) {
            assert( conn && "virtually impossible ! only connection base should call this function !" );
            iterator tgt;

            ORO_OS::MutexLock lock(m);
            if ( (tgt = std::find( mconnections.begin(),
                                   mconnections.end(),
                                   conn)) != mconnections.end() ) {
#ifdef ORO_SIGNAL_USE_RT_LIST
                mconnections.erase( tgt );
#else
                connection_t d(0);
                *tgt = d; // clear out, no erase, keep all iterators valid !
#endif
            }
        }

        void signal_base::cleanup() {
            // this is called from within emit().
            // mutex already locked !
#ifdef ORO_SIGNAL_USE_RT_LIST
#else
            while ( concount > 0 ) {
                mconnections.erase( --(mconnections.end()) );
                --concount;
            }
#endif            
            // remove zeros. too expensive ?
            //itend = std::remove( mconnections.begin(), itend, 0);
        }

    signal_base::signal_base()
        : concount(0)
    {
        itend = mconnections.end();
    }

        signal_base::~signal_base(){
            // call destroy on all connections.
            while ( !mconnections.empty() ) {
                if ( mconnections.front() )
                    mconnections.front()->destroy(); // this calls-back conn_disconnect.
#ifdef ORO_SIGNAL_USE_RT_LIST
#else
                mconnections.erase( mconnections.begin() );
#endif
            }
        }

    }
}
