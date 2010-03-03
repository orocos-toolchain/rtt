/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  threads.hpp 

                        threads.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef OS_THREADS_HPP
#define OS_THREADS_HPP

#include "Atomic.hpp"
#include "../rtt-config.h"

namespace RTT
{ namespace os {

    /**
     * The number of threads in addition to the main() thread.
     * The main thread is thread zero (0). \a threads is
     * increased by one for each PeriodicThread or SingleThread
     * object created and is never decreased.
     */
    extern AtomicInt RTT_API threads;

    /**
     * An integer denoting the lowest priority of the
     * selected OS.
     */
    extern const int RTT_API LowestPriority;

    /**
     * An integer denoting the highest priority of the
     * selected OS.
     */
    extern const int RTT_API HighestPriority;

    /**
     * An interval to be added 'n' times to LowestPriority
     * to get an 'n' times higher priority.
     */
    extern const int RTT_API IncreasePriority;

    /**
     * Check if the scheduler is a valid type in the current process
     * and adapt to a valid value.
     * @return true if \a sched_type made sense, false if a
     * correction was applied.
     */
    bool RTT_API CheckScheduler(int& sched_type);

    /**
     * Check if the scheduler type and priority match and
     * adapt to a valid value. If \a sched_type is unknown
     * a default is choosen. If \a priority is invalid within
     * \a sched_type, a default is choosen.
     * @return true if the arguments made sense, false if a
     * correction was applied.
     */ 
    bool RTT_API CheckPriority(int& sched_type, int& priority);

}}
#endif
