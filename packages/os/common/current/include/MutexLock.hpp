/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  MutexLock.hpp 

                        MutexLock.hpp -  description
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
 
#ifndef MUTEXLOCK_HPP
#define MUTEXLOCK_HPP

#include <os/fosi.h>

#include "Mutex.hpp"

namespace ORO_OS
{
    /**
     * @brief MutexLock is a scope based Monitor, protecting critical
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
                _mutex = &mutex;
                _mutex->lock ();
            }

            /**
             * Remove a lock from a Mutex object
             */
            ~MutexLock()
            {
                _mutex->unlock();
            }

        protected:
            Mutex *_mutex;

            MutexLock()
            {}

    };

    /**
     * @brief A MutexTryLock tries to lock an Mutex object on construction
     * and if successful, unlocks it on destruction of the MutexTryLock.
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
                    : _mutex( &mutex), successful( mutex.trylock() )
            {
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
