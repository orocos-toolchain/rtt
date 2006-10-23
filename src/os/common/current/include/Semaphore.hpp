/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:24 CEST 2005  Semaphore.hpp 

                        Semaphore.hpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
 

#ifndef OS_SEMAPHORE_HPP
#define OS_SEMAPHORE_HPP

#include "fosi.h"
#include <pkgconf/os.h>

namespace RTT
{ namespace OS {
    /**
     * An object oriented wrapper around a counting semaphore.
     * It works like a traffic light on which a thread can \a wait() until
     * the sempahore's value becomes positive, otherwise it blocks. Another
     * thread then needs to \a signal() the semaphore. One thread which is
     * waiting will then be awakened, if none is waiting, the first thread 
     * calling \a wait() will continue directly (and decrease the value by 1).
     * @warning Semaphore instances should only be created in soft realtime,
     *          since the initialisation of a semaphore can not be done in hard realtime.
     *
     */
    class Semaphore 
    {
    private:
        rt_sem_t sem;
    public:
        /**
         * Initialize a Semaphore with an initial \a count.
         */
        Semaphore(int count)
        {
            rtos_sem_init( &sem, count );
        }

        /**
         * Destroy a Semaphore.
         */
        ~Semaphore()
        {
            rtos_sem_destroy( &sem );
        }

        void wait()
        {
            rtos_sem_wait( &sem );
        }

        void signal()
        {
            rtos_sem_signal( &sem );
        }

        /**
         * Try to wait on this semaphore
         *
         * @return true when the locking succeeded ( this->value() was > 0), false otherwise ( this->value() was = 0 )
         */
        bool trywait()
        {
            if ( rtos_sem_trywait( &sem ) == 0 )
                return true;
            return false;
        }

        int value()
        {
            return rtos_sem_value( &sem );
        }
    };
}}

#endif
