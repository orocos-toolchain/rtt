/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  SpinLocker.hpp 

                        SpinLocker.hpp -  description
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
 
 
// XXX: Spinlocks are not tested yet!!!

#ifndef SPINLOCKER_HPP
#define SPINLOCKER_HPP

#include "SpinLock.hpp"

namespace ORO_OS
{

#ifdef HAVE_SPINLOCK

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
             * Locks the <lock> object.
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


    }

    ; // class SpinLocker

#else

    typedef MutexLock SpinLocker;

#endif

} // namespace orocos

#endif // SPINLOCKER_H
