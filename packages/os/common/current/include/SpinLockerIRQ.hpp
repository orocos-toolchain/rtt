/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  SpinLockerIRQ.hpp 

                        SpinLockerIRQ.hpp -  description
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

#ifndef SPINLOCKERIRQ_HPP
#define SPINLOCKERIRQ_HPP

#include "SpinLock.hpp"

#ifdef HAVE_SPINLOCK

namespace ORO_OS
{

    /**
     * An object of this class locks a SpinLock object when it is created, and 
     * unlocks it when it is destroyed. This is very simple to use: in the 
     * beginning of a function you make a SpinLockerIRQ object, and the SpinLock 
     * keeps locked until the function ends.
     * 
     * The SpinLockerIRQ object is safe to use in interrupt handlers, because it 
     * saves the interrupt flags. If you never use the lock in an interrupt handler, 
     * it is more efficient to use a SpinLocker object.
     *
     * Documentation about spinlocks:
     *
     * /usr/src/linux/Documentation/spinlocks.txt
     */

    class SpinLockerIRQ
    {

        protected:

            spinlock_t *pt_lock;
            unsigned long flags;

        protected:
            SpinLockerIRQ()
            {} // forbidden

        public:

            /**
             * Locks the <lock> object.
             */

            SpinLockerIRQ( spinlock_t &lock )
            {

                spin_lock_irqsave( &lock , flags )

                ;
                pt_lock = &lock ;
            }

            /**
             * Unlocks the locked spinlock.
             */
            ~SpinLockerIRQ()
            {
                spin_unlock_irqrestore( pt_lock, flags );
            }


    };
}
#else

namespace ORO_OS {
    typedef MutexLock SpinLockerIRQ;
}

#endif

#endif // SPINLOCKERIRQ_H
