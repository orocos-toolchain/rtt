/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  SpinLocker.hpp 

                        SpinLocker.hpp -  description
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
 
// XXX: Spinlocks are not tested yet!!!

#ifndef SPINLOCKER_HPP
#define SPINLOCKER_HPP

#include "SpinLock.hpp"

#ifdef HAVE_SPINLOCK

namespace ORO_OS
{

    /**
     * An object of this class locks a SpinLock object when it is created, and 
     * unlocks it when it is destroyed. This is very simple to use: in the 
     * beginning of a function you make a SpinLocker object, and the SpinLock 
     * keeps locked until the function ends.
     * 
     * NEVER use this SpinLocker in interrupt handlers, use SpinlockerIRQ instead
     * (slower, because it saves interrupt flags).
     *
     * Documentation about spinlocks:
     *
     * /usr/src/linux/Documentation/spinlocks.txt
     */

    class SpinLocker
    {

        protected:

            spinlock_t *pt_lock;

    protected:
        /**
         * Do not create a SpinLocker without a lock.
         */
            SpinLocker()
            {} // forbidden

        public:

            /**
             * Locks the \a lock object.
             */

            SpinLocker( spinlock_t &lock )
            {

                spin_lock( &lock )

                ;
                pt_lock = &lock ;
            }

            /**
             * Unlocks the locked spinlock.
             */
            ~SpinLocker()
            {
                spin_unlock( pt_lock );
            }


    }; // class SpinLocker
}
#else

#include "MutexLock.hpp"

namespace ORO_OS {
    typedef MutexLock SpinLocker;
}

#endif

#endif // SPINLOCKER_H
