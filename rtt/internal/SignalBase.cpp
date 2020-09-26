/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  SignalBase.cxx

                        SignalBase.cxx -  description
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


#include "rtt/internal/SignalBase.hpp"
#include <boost/bind.hpp>

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
#else
#include "rtt/os/MutexLock.hpp"
#endif

namespace RTT {
    namespace internal {

        // ConnectionBase

        void intrusive_ptr_add_ref( ConnectionBase* p ) { p->ref(); }
        void intrusive_ptr_release( ConnectionBase* p ) { p->deref(); }

        void ConnectionBase::ref() { refcount.inc(); }
        void ConnectionBase::deref() { if ( refcount.dec_and_test() ) delete this; }

        ConnectionBase::ConnectionBase(SignalBase* sig)
            : mconnected(false), m_sig(sig)
            , refcount(0)
        {
        }

        ConnectionBase::~ConnectionBase() {
        }

        bool ConnectionBase::connect() {
            if( !m_sig ) return false;
            mconnected = true;
            return true;
        }
        bool ConnectionBase::disconnect() {
            if (!m_sig) return false;
            mconnected = false;
            return true;
        }
        void ConnectionBase::destroy() {
            if( !m_sig ) return;
            mconnected = false;
            SignalBase* copy = m_sig;
            m_sig = 0;
            copy->conn_destroy(this);
            // after this point this object may be destructed !
        }

        // SignalBase

        void SignalBase::conn_setup( connection_t conn ) {
            // allocate empty slot in list.
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections.grow(1);
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
            mconnections.rt_grow(1);
#else
            connection_t d(0);
            mconnections.push_back( d );
#endif
#endif
            this->conn_connect( conn );
        }

        void SignalBase::conn_connect( connection_t conn ) {
            assert( conn.get() && "virtually impossible ! only connection base should call this function !" );

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections.append( conn );
#else
            // derived class must make sure that list contained enough list items !
            //assert( itend != mconnections.end() );

            // connection (push_back) in emit() does not invalidate iterators, so this
            // function is straightforwardly implemented.
            os::MutexLock lock(m);
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
#endif
        }

        void SignalBase::conn_destroy( connection_t conn ) {
            this->conn_disconnect(conn);
            // increase number of connections destroyed.
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            // free memory
            mconnections.shrink(1);
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
            // free memory
            mconnections.rt_shrink(1);
#else
            ++concount;
#endif
#endif
        }

        void SignalBase::conn_disconnect( connection_t conn ) {
            assert( conn.get() && "virtually impossible ! only connection base should call this function !" );

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections.erase( conn );
#else
            iterator tgt;
            // avoid invalidating iterator of emit() upon self or cross removal of conn.
            os::MutexLock lock(m);
            if ( (tgt = std::find( mconnections.begin(),
                                   mconnections.end(),
                                   conn)) != mconnections.end() ) {
#ifdef ORO_SIGNAL_USE_RT_LIST
                if ( !emitting ) {
                    mconnections.erase( tgt ); // safe to remove we hold mutex + no self removal.
                    return;
                }
                // only done when in emit(). cleanup() is guaranteed to be called afterwards.
                ++disconcount; // used in cleanup() to detect self-disconnections.
#endif
                // cfr for loop in cleanup()
                connection_t d(0);
                *tgt = d; //clear out, no erase, keep all iterators valid !
            }
#endif
        }

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            // NOP
#else
        void SignalBase::cleanup() {
            // this is called from within emit().
            // mutex already locked !
#ifdef ORO_SIGNAL_USE_RT_LIST
            // this construct allows self+cross removal (cfr conn_disconnect) in emit().
            iterator it = mconnections.begin();
            iterator newit(it);
            const_iterator end = mconnections.end();
            for (; newit != end && disconcount > 0 ; it=newit ) {
                if (!*it) {
                    // it & newit become invalid after erase !
                    // do not change this construct unthoughtfully !
                    if ( it == mconnections.begin() ) {
                        mconnections.erase( it );
                        newit = mconnections.begin();
                    } else {
                        ++newit;
                        mconnections.erase( it );
                    }
                    --disconcount;
                } else
                    ++newit;
            }
#else
            while ( concount > 0 ) {
                mconnections.erase( --(mconnections.end()) );
                --concount;
            }
#endif
            // remove zeros. too expensive ?
            //itend = std::remove( mconnections.begin(), itend, 0);
        }
#endif

        SignalBase::SignalBase() :
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections(4) // this is a 'sane' starting point, this number will be grown if required.
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
            disconcount(0)
#else
            concount(0)
#endif
#endif
            ,emitting(false)
    {
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
        // NOP
#else
        itend = mconnections.end();
#endif
    }

        SignalBase::~SignalBase(){
            // call destroy on all connections.
            destroy();
        }

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
        static void disconnectImpl( const ConnectionBase::shared_ptr& c ) {
            c->disconnect();
        }
#endif

        void SignalBase::disconnect() {
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections.apply( boost::bind(&disconnectImpl, _1 ) );
#else
            // avoid invalidating iterator
            os::MutexLock lock(m);
            for( iterator tgt = mconnections.begin(); tgt != mconnections.end(); ++tgt)
                (*tgt)->disconnect();
#endif
        }

        void SignalBase::destroy() {
            while ( !mconnections.empty() ) {
                if ( mconnections.front() )
                    mconnections.front()->destroy(); // this calls-back conn_disconnect.
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
                // NOP
#else
#ifdef ORO_SIGNAL_USE_RT_LIST
                // NOP
#else
                mconnections.erase( mconnections.begin() );
#endif
#endif
            }
        }

        void SignalBase::reserve( size_t conns ) {
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            mconnections.reserve( conns );
#endif
        }

    }
}
