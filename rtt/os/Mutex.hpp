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
#include "CAS.hpp"
#include "Semaphore.hpp"
#include "Time.hpp"

#include <cassert>

#ifdef ORO_OS_USE_BOOST_THREAD
// BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG is defined in rtt-config.h
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
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
		virtual bool timedlock(Seconds) = 0;
        virtual void lock_shared() {}
        virtual void unlock_shared() {}
        virtual bool trylock_shared() { return false; }
        virtual bool timedlock_shared(Seconds) { return false; }
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
            if ( rtos_mutex_trylock_for( &m, Seconds_to_nsecs(s) ) == 0 )
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
            if ( rtos_mutex_rec_trylock_for( &recm, Seconds_to_nsecs(s) ) == 0 )
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
     * @brief An object oriented wrapper around a shared mutex
     * (multiple readers allowed, but only one writer with exclusive access).
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed
     * it. A trylock is a non blocking lock action which fails or succeeds.
     *
     * Implementation is motivated by
     * http://preshing.com/20150316/semaphores-are-surprisingly-versatile/#a-lightweight-semaphore-with-partial-spinning.
     *
     * @see MutexLock, MutexTryLock, SharedMutexLock, SharedMutexTryLock, Mutex
     */
    class RTT_API SharedMutex : public MutexInterface
    {
    protected:
        typedef unsigned int Status;
        oro_atomic_t status;
        Semaphore read_semaphore;
        Semaphore write_semaphore;

        static const Status one_reader       = (1 << 0);
        static const Status one_wait_to_read = (1 << 10);
        static const Status one_writer       = (1 << 20);
        static const Status status_mask      = (1 << 10) - 1;

        static inline unsigned int readers(Status status) {
            return (status & status_mask);
        }
        static inline unsigned int waitToRead(Status status) {
            return ((status >> 10) & status_mask);
        }
        static inline unsigned int writers(Status status) {
            return ((status >> 20) & status_mask);
        }

    public:
        /**
        * Initialize a shared Mutex.
        */
        SharedMutex()
            : read_semaphore(0)
            , write_semaphore(0)
        {
            ORO_ATOMIC_SETUP( &status, 0 );
        }

        /**
        * Destroy a shared Mutex.
        */
        virtual ~SharedMutex()
        {
            ORO_ATOMIC_CLEANUP( &status );
        }

        virtual void lock ()
        {
            Status old_status, new_status;
            do {
                new_status = old_status = (Status) oro_atomic_read( &status );
                new_status += one_writer;
                assert(writers(new_status) > 0);  // overflow?
            } while(!CAS(&status, (int) old_status, (int) new_status));

            if (readers(old_status) > 0 || writers(old_status) > 0) {
                write_semaphore.wait();
            }
        }

        virtual void unlock()
        {
            Status old_status, new_status;
            unsigned int wait_to_read = 0;
            do {
                new_status = old_status = (Status) oro_atomic_read( &status );
                assert(readers(old_status) == 0);
                assert(writers(old_status) > 0);
                new_status -= one_writer;
                wait_to_read = waitToRead(old_status);
                if (wait_to_read > 0) {
                    // all waiting threads can be unblocked
                    new_status = (new_status & (status_mask << 20)) | wait_to_read;
                }
            } while(!CAS(&status, (int) old_status, (int) new_status));

            // Signal threads that have been waiting for a read lock
            if (wait_to_read > 0) {
                for(unsigned int i = 0; i < wait_to_read; ++i) read_semaphore.signal();
            } else
            // ... or signal a thread that has been waiting for a write lock
            if (writers(old_status) > 1) {
                write_semaphore.signal();
            }
        }

        /**
        * Try to lock this mutex exclusively
        *
        * @note not implemented for this type of mutex
        * @return false
        */
        virtual bool trylock()
        {
            return false;
        }

        /**
        * Lock this mutex exclusively, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        *
        * @note not implemented for this type of mutex
        * @return false
        */
        virtual bool timedlock(Seconds s)
        {
            (void) s;
            return false;
        }

        /**
         * Obtain shared ownership of this mutex.
         */
        void lock_shared ()
        {
            Status old_status, new_status;
            do {
                new_status = old_status = (Status) oro_atomic_read( &status );
                if (writers(old_status) > 0) {
                    new_status += one_wait_to_read;
                    assert(waitToRead(new_status) > 0);  // overflow?
                } else {
                    new_status += one_reader;
                    assert(readers(new_status) > 0);  // overflow?
                }
            } while(!CAS(&status, (int) old_status, (int) new_status));

            // wait until all writers are finished
            if (writers(old_status) > 0) {
                read_semaphore.wait();
            }
        }

        /**
         * Release shared ownership of this mutex.
         */
        virtual void unlock_shared()
        {
            Status old_status, new_status;
            do {
                new_status = old_status = (Status) oro_atomic_read( &status );
                assert(readers(old_status) > 0);
                new_status -= one_reader;
            } while(!CAS(&status, (int) old_status, (int) new_status));

            // one writer thread can be unblocked if there are no more readers
            if (readers(old_status) == 1 && writers(old_status) > 0) {
                write_semaphore.signal();
            }
        }

        /**
        * Attempt to obtain shared ownership of this mutex
        *
        * @note not implemented for this type of mutex
        * @return false
        */
        virtual bool trylock_shared()
        {
            return false;
        }

        /**
        * Attempt to obtain shared ownership of this mutex, but don't wait longer for the lock
        * than the specified timeout.
        *
        * @param  s The maximum time to wait before aqcuiring the lock.
        *
        * @note not implemented for this type of mutex
        * @return false
        */
        virtual bool timedlock_shared(Seconds s)
        {
            (void) s;
            return false;
        }
    };

}}

#endif
