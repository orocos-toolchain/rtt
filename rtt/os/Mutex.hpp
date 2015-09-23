/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  Mutex.hpp

                        Mutex.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#ifndef OS_MUTEX_HPP
#define OS_MUTEX_HPP

#include "fosi.h"
#include "../rtt-config.h"
#include "rtt-os-fwd.hpp"
#include "Time.hpp"
#ifdef ORO_OS_USE_BOOST_THREAD
// BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG is defined in rtt-config.h
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#endif

namespace RTT
{ namespace os {

    /**
     * @brief An interface to a Mutex.
     *
     * @see MutexLock, MutexTryLock, MutexRecursive, SharedMutexLock
     */
	class RTT_API MutexInterface
	{
	public:
        virtual ~MutexInterface() {}
		virtual void lock() =0;
		virtual void unlock() =0;
		virtual bool trylock() = 0;
		virtual bool timedlock(Seconds s) = 0;

        virtual void lock_shared() {}
        virtual void unlock_shared() {}
        virtual bool trylock_shared() { return false; }
        virtual bool timedlock_shared(Seconds s) { return false; }

        virtual bool lock_upgrade() { return false; }
        virtual void unlock_upgrade() {}
        virtual bool trylock_upgrade() { return false; }
        virtual bool timedlock_upgrade(Seconds s) { return false; }
	};


    /**
     * @brief An object oriented wrapper around a non recursive mutex.
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed
     * it. A trylock is a non blocking lock action which fails or succeeds.
     * @warning Mutex instances should only be created in soft realtime,
     *          since the initialisation of a mutex can not be done in hard realtime.
     *
     * @see MutexLock, MutexTryLock, MutexTimedLock, MutexRecursive
     */
	class RTT_API Mutex : public MutexInterface
    {
	    friend class Condition;
#ifndef ORO_OS_USE_BOOST_THREAD
	protected:
	    rt_mutex_t m;
	public:
	    /**
	    * Initialize a Mutex.
	    */
	    Mutex()
	    {
	        rtos_mutex_init( &m);
	    }

	    /**
	    * Destroy a Mutex.
	    * If the Mutex is still locked, the RTOS
	    * will not be asked to clean up its resources.
	    */
	    virtual ~Mutex()
	    {
	        if ( trylock() ) {
	            unlock();
	            rtos_mutex_destroy( &m );
	        }
	    }

	    virtual void lock ()
	    {
	        rtos_mutex_lock( &m );
	    }

	    virtual void unlock()
	    {
	        rtos_mutex_unlock( &m );
	    }

	    /**
	    * Try to lock this mutex
	    *
	    * @return true when the locking succeeded, false otherwise
	    */
	    virtual bool trylock()
	    {
	        if ( rtos_mutex_trylock( &m ) == 0 )
	            return true;
	        return false;
	    }

        /**
        * Lock this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            if ( rtos_mutex_lock_until( &m, rtos_get_time_ns() + Seconds_to_nsecs(s) ) == 0 )
                return true;
            return false;
        }
#else
    protected:
        boost::timed_mutex m;
    public:
        /**
        * Initialize a Mutex.
        */
        Mutex()
        {
        }

        /**
        * Destroy a Mutex.
        * If the Mutex is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        virtual ~Mutex()
        {
        }

        virtual void lock ()
        {
            m.lock();
        }

        virtual void unlock()
        {
            m.unlock();
        }

        /**
        * Try to lock this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            return m.try_lock();
        }

        /**
        * Lock this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            return m.timed_lock( boost::posix_time::microseconds(Seconds_to_nsecs(s)/1000) );
        }
#endif

    };

    /**
     * @brief An object oriented wrapper around a recursive mutex.
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed
     * it. A trylock is a non blocking lock action which fails or succeeds.
     *
     * @see MutexLock, MutexTryLock, Mutex
     */
    class RTT_API MutexRecursive : public MutexInterface
    {
#ifndef ORO_OS_USE_BOOST_THREAD
    protected:
        rt_rec_mutex_t recm;
    public:
        /**
         * Initialize a recursive Mutex.
         */
        MutexRecursive()
        {
            rtos_mutex_rec_init( &recm );
        }

        /**
        * Destroy a MutexRecursive.
        * If the MutexRecursive is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        virtual ~MutexRecursive()
        {
            if ( trylock() ) {
                unlock();
                rtos_mutex_rec_destroy( &recm );
            }
        }

        void lock ()
        {
            rtos_mutex_rec_lock( &recm );
        }

        virtual void unlock()
        {
            rtos_mutex_rec_unlock( &recm );
        }

        /**
        * Try to lock this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            if ( rtos_mutex_rec_trylock( &recm ) == 0 )
                return true;
            return false;
        }

        /**
        * Lock this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            if ( rtos_mutex_rec_lock_until( &recm, rtos_get_time_ns() + Seconds_to_nsecs(s) ) == 0 )
                return true;
            return false;
        }
#else
    protected:
        boost::recursive_timed_mutex recm;
    public:
        /**
         * Initialize a recursive Mutex.
         */
        MutexRecursive()
        {
        }

        /**
        * Destroy a MutexRecursive.
        * If the MutexRecursive is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        virtual ~MutexRecursive()
        {
        }

        void lock ()
        {
            recm.lock();
        }

        virtual void unlock()
        {
            recm.unlock();
        }

        /**
        * Try to lock this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            return recm.try_lock();
        }

        /**
        * Lock this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            return recm.timed_lock( boost::posix_time::microseconds( Seconds_to_nsecs(s)/1000 ) );
        }
#endif
    };

    /**
     * @brief An object oriented wrapper around a shared mutex (multiple readers allowed).
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed
     * it. A trylock is a non blocking lock action which fails or succeeds.
     *
     * @see MutexLock, MutexTryLock, SharedMutexLock, SharedMutexTryLock, Mutex
     */
    class RTT_API SharedMutex : public MutexInterface
    {
#ifndef ORO_OS_USE_BOOST_THREAD
    protected:
        rt_mutex_t m;
        rt_cond_t shared_cond;
        rt_cond_t exclusive_cond;
        unsigned shared_count;
        bool exclusive;
    public:
        /**
        * Initialize a shared Mutex.
        */
        SharedMutex()
            : shared_count(0)
            , exclusive(false)
        {
            rtos_mutex_init( &m );
            rtos_cond_init( &shared_cond );
            rtos_cond_init( &exclusive_cond );
        }

        /**
        * Destroy a shared Mutex.
        * If the Mutex is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        virtual ~SharedMutex()
        {
            if ( trylock() ) {
                unlock();
                rtos_mutex_destroy( &m );
                rtos_cond_destroy( &shared_cond );
                rtos_cond_destroy( &exclusive_cond );
            }
        }

        virtual void lock ()
        {
            rtos_mutex_lock( &m );
            while (shared_count || exclusive) {
                rtos_cond_wait(&exclusive_cond, &m);
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
        }

        virtual void unlock()
        {
            rtos_mutex_lock( &m );
            assert(shared_count == 0);
            assert(exclusive);
            exclusive = false;
            rtos_cond_broadcast( &exclusive_cond );
            rtos_cond_broadcast( &shared_cond );
            rtos_mutex_unlock( &m );
        }

        /**
        * Try to lock this mutex exclusively
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            rtos_mutex_lock( &m );
            if (shared_count || exclusive) {
                rtos_mutex_unlock( &m );
                return false;
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
        * Lock this mutex exclusively, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            RTT::nsecs abs_time = rtos_get_time_ns() + Seconds_to_nsecs(s);
            rtos_mutex_lock( &m );
            while (shared_count || exclusive) {
                if ( rtos_cond_timedwait( &exclusive_cond, &m, abs_time ) != 0 ) {
                    rtos_mutex_unlock( &m );
                    return false;
                }
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
         * Obtain shared ownership of this mutex.
         */
        void lock_shared ()
        {
            rtos_mutex_lock( &m );
            while (exclusive) {
                rtos_cond_wait(&shared_cond, &m);
            }
            shared_count++;
            rtos_mutex_unlock( &m );
        }

        /**
         * Release shared ownership of this mutex.
         */
        virtual void unlock_shared()
        {
            rtos_mutex_lock( &m );
            assert(shared_count > 0);
            assert(!exclusive);
            if (shared_count > 0) shared_count--;
            rtos_cond_broadcast( &exclusive_cond );
            rtos_mutex_unlock( &m );
        }

        /**
        * Attempt to obtain shared ownership of this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock_shared()
        {
            rtos_mutex_lock( &m );
            if (exclusive) {
                rtos_mutex_unlock( &m );
                return false;
            }
            shared_count++;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
        * Attempt to obtain shared ownership of this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock_shared(Seconds s)
        {
            RTT::nsecs abs_time = rtos_get_time_ns() + Seconds_to_nsecs(s);
            rtos_mutex_lock( &m );
            while (exclusive) {
                if ( rtos_cond_timedwait( &shared_cond, &m, abs_time ) != 0 ) {
                    rtos_mutex_unlock( &m );
                    return false;
                }
            }
            shared_count++;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
         * Obtain upgrade ownership of this mutex.
         */
        virtual bool lock_upgrade ()
        {
            rtos_mutex_lock( &m );
            while ((shared_count > 1) || exclusive) {
                rtos_cond_wait(&exclusive_cond, &m);
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
         * Release upgrade ownership of this mutex.
         */
        virtual void unlock_upgrade()
        {
            rtos_mutex_lock( &m );
            assert(shared_count == 1);
            assert(exclusive);
            exclusive = false;
            rtos_cond_broadcast( &exclusive_cond );
            rtos_cond_broadcast( &shared_cond );
            rtos_mutex_unlock( &m );
        }

        /**
        * Attempt to obtain upgrade ownership of this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock_upgrade()
        {
            rtos_mutex_lock( &m );
            if ((shared_count > 1) || exclusive) {
                rtos_mutex_unlock( &m );
                return false;
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
            return true;
        }

        /**
        * Attempt to obtain upgrade ownership of this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock_upgrade(Seconds s)
        {
            RTT::nsecs abs_time = rtos_get_time_ns() + Seconds_to_nsecs(s);
            rtos_mutex_lock( &m );
            while ((shared_count > 1) || exclusive) {
                if ( rtos_cond_timedwait( &shared_cond, &m, abs_time ) != 0 ) {
                    rtos_mutex_unlock( &m );
                    return false;
                }
            }
            exclusive = true;
            rtos_mutex_unlock( &m );
            return true;
        }

#else
    protected:
        boost::shared_mutex sharedm;
    public:
        /**
         * Initialize a shared Mutex.
         */
        SharedMutex()
        {
        }

        /**
        * Destroy a SharedMutex.
        * If the SharedMutex is still locked, the RTOS
        * will not be asked to clean up its resources.
        */
        virtual ~SharedMutex()
        {
        }

        void lock ()
        {
            sharedm.lock();
        }

        virtual void unlock()
        {
            sharedm.unlock();
        }

        /**
        * Try to lock this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            return sharedm.try_lock();
        }

        /**
        * Lock this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock(Seconds s)
        {
            return recm.timed_lock( boost::posix_time::microseconds( Seconds_to_nsecs(s)/1000 ) );
        }

        /**
         * Obtain shared ownership of this mutex.
         */
        void lock_shared ()
        {
            sharedm.lock_shared();
        }

        /**
         * Release shared ownership of this mutex.
         */
        virtual void unlock_shared()
        {
            sharedm.unlock_shared();
        }

        /**
        * Attempt to obtain shared ownership of this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock_shared()
        {
            return sharedm.try_lock_shared();
        }

        /**
        * Attempt to obtain shared ownership of this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock_shared(Seconds s)
        {
            return recm.timed_lock_shared( boost::posix_time::microseconds( Seconds_to_nsecs(s)/1000 ) );
        }

        /**
         * Obtain upgrade ownership of this mutex.
         */
        virtual bool lock_upgrade ()
        {
            try {
                recm.lock_upgrade();
            } catch(boost::lock_error&) {
                return false;
            }
            return true;
        }

        /**
         * Release upgrade ownership of this mutex.
         */
        virtual void unlock_upgrade()
        {
            recm.unlock_upgrade();
        }

        /**
        * Attempt to obtain upgrade ownership of this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock_upgrade()
        {
            return recm.try_lock_upgrade();
        }

        /**
        * Attempt to obtain upgrade ownership of this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool timedlock_upgrade(Seconds s)
        {
            return recm.try_lock_upgrade_for( boost::posix_time::microseconds( Seconds_to_nsecs(s)/1000 ) );
        }
#endif
    };

}}

#endif
