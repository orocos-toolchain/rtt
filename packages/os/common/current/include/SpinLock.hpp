/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  SpinLock.hpp 

                        SpinLock.hpp -  description
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

#ifndef SPINLOCK_HPP
#define SPINLOCK_HPP

#ifdef HAVE_SPINLOCK

#ifdef __cplusplus
#define new _new
    extern "C"
    {
#include <linux/spinlock.h>
#include <asm/system.h>

    }
#undef new
#endif // __cplusplus

namespace ORO_OS
{

    /**
     * This class implements a spinlock, which is a very efficient single-holder 
     * lock: if cou can't get the spinlock, you keep trying (spinning) until you 
     * can. It is more efficient than a mutex when you have to lock something for
     * a short time.
     *
     * Documentation about kernel locking:
     *
     * http://kernelnewbies.org/documents/kdoc/kernel-locking/lklockingguide.html
     */

    class SpinLock : public spinlock_t
    {

        public:
            /**
             * Constructs a SpinLock.
             */
            SpinLock()
            {
            }

            ~SpinLock()
            {
            }

    };

}

#else

#include "Mutex.hpp"

namespace ORO_OS {
    typedef Mutex SpinLock;
}
#endif // HAVE_SPINLOCK

#endif // SPINLOCK_H
