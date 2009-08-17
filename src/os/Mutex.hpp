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
#include "../Time.hpp"

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
	    friend class MutexLock;
	    friend class MutexTryLock;
	    friend class MutexTimedLock;
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

    protected:
        rt_rec_mutex_t recm;
    };

}}

#endif
