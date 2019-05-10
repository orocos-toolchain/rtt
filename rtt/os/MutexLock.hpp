/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  MutexLock.hpp

                        MutexLock.hpp -  description
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

#ifndef OS_MUTEXLOCK_HPP
#define OS_MUTEXLOCK_HPP

#include "fosi.h"

#include "Mutex.hpp"

namespace RTT
{ namespace os {
    /**
     * @brief MutexLock is a scope based Monitor, protecting critical
     * sections with a Mutex object through locking and unlocking it.
     */
    class RTT_API MutexLock
    {

        public:
            /**
             * Create a lock on a Mutex object.
             *
             * @param mutex The Mutex to be locked.
             */
            MutexLock( MutexInterface &mutex )
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
            MutexInterface *_mutex;

            MutexLock()
            {}

    };

    /**
     * @brief A MutexTryLock tries to lock an Mutex object on construction
     * and if successful, unlocks it on destruction of the MutexTryLock.
     * Use isSuccessful() to check whether the attempt succeeded.
     */
    class RTT_API MutexTryLock
    {

        public:

            /**
             * Try to lock a Mutex object
             *
             * @param mutex The Mutex which should be attempted to be locked
             */
            MutexTryLock( MutexInterface &mutex )
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
            MutexInterface *_mutex;

            MutexTryLock()
            {}

        private:

            /**
             * Stores the state of success
             */
            bool successful;

    };

     /**
      * @brief A MutexTimedLock locks a Mutex object on construction
      * and if successful, unlocks it on destruction of the MutexTimedLock.
      * If the lock can not be acquired within the user provided time limit,
      * the attempt is aborted.
      * Use isSuccessful() to check whether the attempt succeeded.
      */
     class RTT_API MutexTimedLock
     {

         public:

             /**
              * Lock a Mutex object, but don't block longer than a specified
              * timeout.
              *
              * @param mutex The Mutex which should be attempted to be locked
              * @param timeout The maximum time to wait in seconds.
              */
             MutexTimedLock( MutexInterface &mutex, Seconds timeout )
                     : _mutex( &mutex), successful( mutex.timedlock(timeout) )
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
             ~MutexTimedLock()
             {
                 if ( successful )
                     _mutex->unlock();
             }

         protected:
             /**
              * The Mutex to lock and unlock
              */
             MutexInterface *_mutex;

             MutexTimedLock()
             {}

         private:

             /**
              * Stores the state of success
              */
             bool successful;

     };

     /**
      * @brief SharedMutexLock is a scope based Monitor, protecting critical
      * sections with a SharedMutex object through locking and unlocking it.
      */
     class RTT_API SharedMutexLock
     {

         public:
             /**
              * Create a shared lock on a SharedMutex object.
              *
              * @param mutex The Mutex to be locked.
              */
             SharedMutexLock( MutexInterface &mutex )
             {
                 _mutex = &mutex;
                 _mutex->lock_shared ();
             }

             /**
              * Remove a lock from a SharedMutex object
              */
             ~SharedMutexLock()
             {
                 _mutex->unlock_shared();
             }

         protected:
             MutexInterface *_mutex;

             SharedMutexLock()
             {}
     };
}}

#endif
