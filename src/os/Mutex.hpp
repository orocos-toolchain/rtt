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

namespace RTT
{ namespace OS {
    class MutexLock;
    class MutexTryLock;

    /**
     * @brief An interface to a Mutex.
     *
     * @see MutexLock, MutexTryLock, MutexRecursive
     */
	class MutexInterface 
	{
	public:
        virtual ~MutexInterface() {}
		virtual void lock() =0;
		virtual void unlock() =0;
		virtual bool trylock() = 0;
	};

    /**
     * @brief An object oriented wrapper around a non recursive mutex.
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed 
     * it. A trylock is a non blocking lock action which fails or succeeds.
     * @warning Mutex instances should only be created in soft realtime,
     *          since the initialisation of a mutex can not be done in hard realtime.
     *
     * @see MutexLock, MutexTryLock, MutexRecursive
     */
	class Mutex : public MutexInterface
    {
        friend class OS::MutexLock;
        friend class OS::MutexTryLock;
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
             * This is a blocking operation on the 
             * mutex itself. The mutex may not be locked
             * when you call the destructor.
             */
            virtual ~Mutex()
            {
                lock();
                unlock(); // remove this ? then we destroy a locked mutex.
                // race condition here...
                rtos_mutex_destroy( &m );
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

    };

    /**
     * @brief An object oriented wrapper around a recursive mutex.
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed 
     * it. A trylock is a non blocking lock action which fails or succeeds.
     *
     * @see MutexLock, MutexTryLock, Mutex
     */
    class MutexRecursive : public MutexInterface
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
       * Destroy a recursive Mutex.
       */
      ~MutexRecursive()
      {
	lock();
	unlock();
	// Race condition here...
	rtos_mutex_rec_destroy( &recm);
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

    protected:
      rt_rec_mutex_t recm;
    };

}}

#endif
