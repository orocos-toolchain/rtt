/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  Mutex.hpp 

                        Mutex.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <os/fosi.h>

namespace ORO_OS
{
    /**
     * @brief An object oriented wrapper around the posix mutex.
     *
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed 
     * it. A trylock is a non blocking lock action which fails or succeeds.
     * @warning Mutex instances should only be created in soft realtime,
     *          since the initialisation of a mutex can not be done in hard realtime.
     *
     * @see MutexLock, MutexTryLock
     */
    class Mutex 
    {
        friend class MutexLock;

        friend class MutexTryLock;

        friend class ConditionVariableTimed;
        
        public:
            /**
             * Initialize a Mutex.
             */
            Mutex()
            {
                rtos_mutex_init( &m, 0 );
            }

            /**
             * Destroy a Mutex.
             * This is a blocking operation on the 
             * mutex itself. The mutex may not be locked
             * when you call the destructor.
             */
            ~Mutex()
            {
                lock();
                unlock(); // remove this ? then we destroy a locked mutex.
                // race condition here...
                rtos_mutex_destroy( &m );
            }

            void lock ()
            {
                rtos_mutex_lock( &m );
            }

            void unlock()
            {
                rtos_mutex_unlock( &m );
            }

            /**
             * Try to lock this mutex
             *
             * @return true when the locking succeeded, false otherwise
             */
            bool trylock()
            {
                if ( rtos_mutex_trylock( &m ) == 0 )
                    return true;
                return false;
            }

    private:
        rt_mutex_t m;
    };

}

#endif
