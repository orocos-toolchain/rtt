/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  Mutex.hpp 

                        Mutex.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <os/fosi.h>

namespace ORO_OS
{
	

    /**
     * An object oriented wrapper around the posix mutex.
     * A mutex can only be  unlock()'ed, by the thread which lock()'ed 
     * it. A trylock is a non blocking lock action which fails or succeeds.
     * @Warning Mutex instances should only be created in soft realtime,
     *          since the initialisation of a mutex can not be done in hard realtime.
     *
     * @see MutexLock
     */
    class Mutex 
    //: public pthread_mutex_t
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
                unlock();
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
                int res;
                if ( (res = rtos_mutex_trylock( &m )) == 0 )
                    return true;
                if ( res == EINVAL )
                    exit(1);
                return false;
            }

    private:
        rt_mutex_t m;
    };

}

#endif
