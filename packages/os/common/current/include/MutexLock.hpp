/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  MutexLock.hpp 

                        MutexLock.hpp -  description
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
 
 
#ifndef MUTEXLOCK_HPP
#define MUTEXLOCK_HPP

#include <os/fosi.h>

#include "Mutex.hpp"

namespace ORO_OS
{
    /**
     * MutexLock is a scope based Monitor, protecting critical
     * sections with a Mutex object through locking and unlocking it.
     */

    class MutexLock
    {

        public:
            /**
             * Create a lock on a Mutex object.
             * 
             * @param mutex The Mutex to be locked.
             */
            MutexLock( Mutex &mutex )
            {
                //                pthread_mutex_lock(&mutex);
                _mutex = &mutex;

                _mutex->lock ()

                ;
            }

            /**
             * Remove a lock from a Mutex object
             */
            ~MutexLock()
            {
                _mutex->unlock();
                //                pthread_mutex_unlock(_mutex);
            }

        protected:
            Mutex *_mutex;

            MutexLock()
            {}

    }

    ;

    /**
     * A MutexTryLock tries to lock an Mutex object on construction
     * and unlocks it on destruction of the MutexTryLock.
     */

    class MutexTryLock
    {

        public:

            /**
             * Try to lock a Mutex object
             *
             * @param mutex The Mutex which should be attempted to be locked
             */
            MutexTryLock( Mutex &mutex )
                    : successful( false )
            {
                //if( pthread_mutex_trylock(&mutex) == 0) succesfull = true;
                _mutex = &mutex;
                successful = _mutex->trylock();
            }

            /**
             * Return if the locking of the Mutex was succesfull
             *
             * @return true when the Mutex is locked
             */
            bool isSuccessful()
            {
                return successful;
            }

            /**
             * Releases, if any, a lock on the previously try-locked Mutex
             */
            ~MutexTryLock()
            {
                if ( successful )
                    _mutex->unlock();

                //                    pthread_mutex_unlock(_mutex);
            }

        protected:
            /**
             * The Mutex to lock and unlock
             */
            Mutex *_mutex;

            MutexTryLock()
        {}

        private:

            /**
             * Stores the state of success
             */
            bool successful;

    };

}

#endif
