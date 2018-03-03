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

/// @todo Replace boost::system::system_error by std::system_error for C++11 and newer.
#include <boost/system/system_error.hpp>
#include <cassert>

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
     * @see MutexLock, MutexTryLock, MutexRecursive
     */
	class RTT_API MutexInterface
	{
	public:
        virtual ~MutexInterface() {}
		virtual void lock() =0;
		virtual void unlock() =0;
		virtual bool trylock() = 0;
		virtual bool timedlock(Seconds s) = 0;
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
            int ret = rtos_mutex_init( &m);
            if ( ret == 0 )
                return;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to initialize an instance of RTT::os::Mutex");
#endif
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
              int ret = rtos_mutex_destroy( &m );
              if ( ret == 0 )
                  return;
              assert(false && "Failed to destroy an instance of RTT::os::Mutex");
	        }
	    }

	    virtual void lock ()
	    {
	        int ret = rtos_mutex_lock( &m );
          if ( ret == 0 )
              return;
#ifndef ORO_EMBEDDED
          throw boost::system::system_error(-ret, boost::system::system_category(),
              "Failed to lock an instance of RTT::os::Mutex");
#else
          assert(false && "Failed to lock an instance of RTT::os::Mutex");
#endif
	    }

	    virtual void unlock()
	    {
	        int ret = rtos_mutex_unlock( &m );
          if ( ret == 0 )
              return;
#ifndef ORO_EMBEDDED
          throw boost::system::system_error(-ret, boost::system::system_category(),
              "Failed to unlock an instance of RTT::os::Mutex");
#else
          assert(false && "Failed to unlock an instance of RTT::os::Mutex");
#endif
	    }

	    /**
	    * Try to lock this mutex
	    *
	    * @return true when the locking succeeded, false otherwise
	    */
	    virtual bool trylock()
	    {
            int ret = rtos_mutex_trylock( &m );
            if ( ret == 0 )
                return true;
            if ( ret == -EBUSY || ret == -EWOULDBLOCK || ret == -ETIMEDOUT || ret == -EINTR )
                return false;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to lock an instance of RTT::os::Mutex");
#else
            assert(false && "Failed to lock an instance of RTT::os::Mutex");
            return false;
#endif
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
            int ret = rtos_mutex_lock_until( &m, rtos_get_time_ns() + Seconds_to_nsecs(s) );
            if ( ret == 0 )
                return true;
            if ( ret == -EBUSY || ret == -EWOULDBLOCK || ret == -ETIMEDOUT || ret == -EINTR )
                return false;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to lock an instance of RTT::os::Mutex");
#else
            assert(false && "Failed to lock an instance of RTT::os::Mutex");
            return false;
#endif
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
            int ret = rtos_mutex_rec_init( &recm );
            if ( ret == 0 )
                return;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to initialize an instance of RTT::os::MutexRecursive");
#endif
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
                // The calling thread might have locked the mutex more than once.
                while(rtos_mutex_rec_unlock( &recm ) == 0);
                int ret = rtos_mutex_rec_destroy( &recm );
                if ( ret == 0 )
                    return;
                assert(false && "Failed to destroy an instance of RTT::os::MutexRecursive");
            }
        }

        void lock ()
        {
            int ret = rtos_mutex_rec_lock( &recm );
            if ( ret == 0 )
                return;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to lock an instance of RTT::os::MutexRecursive");
#else
            assert(false && "Failed to lock an instance of RTT::os::MutexRecursive");
#endif
        }

        virtual void unlock()
        {
            int ret = rtos_mutex_rec_unlock( &recm );
            if ( ret == 0 )
                return;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to unlock an instance of RTT::os::MutexRecursive");
#else
            assert(false && "Failed to unlock an instance of RTT::os::MutexRecursive");
#endif
        }

        /**
        * Try to lock this mutex
        *
        * @return true when the locking succeeded, false otherwise
        */
        virtual bool trylock()
        {
            int ret = rtos_mutex_rec_trylock( &recm );
            if ( ret == 0 )
                return true;
            if ( ret == -EBUSY || ret == -EWOULDBLOCK || ret == -ETIMEDOUT || ret == -EINTR )
                return false;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to lock an instance of RTT::os::MutexRecursive");
#else
            assert(false && "Failed to lock an instance of RTT::os::MutexRecursive");
            return false;
#endif
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
            int ret = rtos_mutex_rec_lock_until( &recm, rtos_get_time_ns() + Seconds_to_nsecs(s) );
            if ( ret == 0 )
                return true;
            if ( ret == -EBUSY || ret == -EWOULDBLOCK || ret == -ETIMEDOUT || ret == -EINTR )
                return false;
#ifndef ORO_EMBEDDED
            throw boost::system::system_error(-ret, boost::system::system_category(),
                "Failed to lock an instance of RTT::os::MutexRecursive");
#else
            assert(false && "Failed to lock an instance of RTT::os::MutexRecursive");
            return false;
#endif
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

}}

#endif
