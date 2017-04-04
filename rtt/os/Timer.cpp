/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  Timer.cpp

                        Timer.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

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


#include "Timer.hpp"
#include "MutexLock.hpp"
#include "../Activity.hpp"
#include "../Logger.hpp"
#include "../os/fosi.h"
#include <limits>

namespace RTT {
    using namespace base;
    using namespace os;

    bool Timer::initialize() {
        mdo_quit = false;
        // only start if non periodic.
        return this->getThread()->getPeriod() == 0;
    }
    void Timer::finalize() {}

    void Timer::step() {
        // no implementation for periodic execution.
    }

    void Timer::loop()
    {
        // This code is executed from mThread's thread
        while (!mdo_quit) {
            Time wake_up_time;
            TimerId next_timer_id = 0;

            // Select next timer.
            {// This scope is for MutexLock.
                // find wake_up_time
                // check timers queue.
                MutexLock locker(m);
                // We can't use infinite as the OS may internally use time_spec, which can not
                // represent as much in the future (until 2038) // XXX Year-2038 Bug
                wake_up_time = 1000000000LL * std::numeric_limits<int32_t>::max();
                for (TimerIds::iterator it = mtimers.begin(); it != mtimers.end(); ++it) {
                    if ( it->expires != 0 && it->expires < wake_up_time  ) {
                        wake_up_time = it->expires;
                        next_timer_id = it - mtimers.begin();
                    }
                }
            }// MutexLock

            // Wait
            int ret = 0;
            Time now = rtos_get_time_ns();
            if ( wake_up_time > now )
                ret = msem.waitUntil( wake_up_time ); // case of no timers or running timers
            else
                ret = -1; // case of timer overrun.

            // Timeout handling
            if (ret == -1) {
                // a timer expired
                // expires: reset/reprogram the timer that expired:
                {
                    MutexLock locker(m);
                    // detect corner case for resize:
                    if ( next_timer_id < int(mtimers.size()) ) {
                        // now clear or reprogram it.
                        TimerIds::iterator tim = mtimers.begin() + next_timer_id;
                        if ( tim->period ) {
                            // periodic timer
                            // if late by more than 4 periods, skip late updates
                            int maxDelayInPeriods = 4;
                            if (now - tim->expires > tim->period*maxDelayInPeriods) {
                                tim->expires += tim->period*((now - tim->expires) / tim->period);
                            }
                            tim->expires += tim->period;
                        } else {
                            // aperiodic timer
                            tim->expires = 0;
                        }
                    }
                }

                // Second: notify waiting threads
                {// This scope is for MutexLock.
                    MutexLock locker(m);
                    mtimers[next_timer_id].expired.broadcast();
                }// MutexLock

                // Third: send the timeout signal and allow (within the callback)
                // to reprogram the timer.
                // If we would expires call timeout(), the code above would overwrite
                // user settings.
                timeout( next_timer_id );
            }
        }
    }

    bool Timer::breakLoop()
    {
        mdo_quit = true;
        msem.signal();
        // kill all timers to abort all threads blocking in waitFor()
        for (TimerId i = 0; i < (int) mtimers.size(); ++i) {
            killTimer(i);
        }
        return true;
    }

    Timer::Timer(TimerId max_timers, int scheduler, int priority, const std::string& name)
        : mThread(0), msem(0), mdo_quit(false)
    {
        mtimers.resize(max_timers);
        if (scheduler != -1) {
            mThread = new Activity(scheduler, priority, 0.0, this, name);
            mThread->start();
        }
    }

    Timer::~Timer()
    {
        delete mThread;
    }


    void Timer::timeout(TimerId timer_id)
    {
        // User must implement this method.
    }

    void Timer::setMaxTimers(TimerId max)
    {
        MutexLock locker(m);
        mtimers.resize(max, TimerInfo() );
    }

    bool Timer::startTimer(TimerId timer_id, double period)
    {
        if ( timer_id < 0 || timer_id >= int(mtimers.size()) || period < 0.0)
        {
            log(Error) << "Invalid timer id or period" << endlog();
            return false;
        }

        Time due_time = rtos_get_time_ns() + Seconds_to_nsecs( period );

        {
            MutexLock locker(m);
            mtimers[timer_id].expires = due_time;
            mtimers[timer_id].period = Seconds_to_nsecs( period );
        }
        msem.signal();
        return true;
    }

    bool Timer::arm(TimerId timer_id, double wait_time)
    {
        if ( timer_id < 0 || timer_id >= int(mtimers.size()) || wait_time < 0.0)
        {
            log(Error) << "Invalid timer id or wait time" << endlog();
            return false;
        }

        Time now = rtos_get_time_ns();
        Time due_time = now + Seconds_to_nsecs( wait_time );

        {
            MutexLock locker(m);
            mtimers[timer_id].expires  = due_time;
            mtimers[timer_id].period = 0;
        }
        msem.signal();
        return true;
    }

    bool Timer::isArmed(TimerId timer_id) const
    {
        MutexLock locker(m);
        if (timer_id < 0 || timer_id >= int(mtimers.size()) )
        {
            log(Error) << "Invalid timer id" << endlog();
            return false;
        }
        return mtimers[timer_id].expires != 0;
    }

    double Timer::timeRemaining(TimerId timer_id) const
    {
        MutexLock locker(m);
        if (timer_id < 0 || timer_id >= int(mtimers.size()) )
        {
            log(Error) << "Invalid timer id" << endlog();
            return 0.0;
        }
        Time now = rtos_get_time_ns();
        Time result = mtimers[timer_id].expires - now;
        // detect corner cases.
        if ( result < 0 )
            return 0.0;
        return nsecs_to_Seconds( result );
    }

    bool Timer::killTimer(TimerId timer_id)
    {
        MutexLock locker(m);
        if (timer_id < 0 || timer_id >= int(mtimers.size()) )
        {
            log(Error) << "Invalid timer id" << endlog();
            return false;
        }
        mtimers[timer_id].expires = 0;
        mtimers[timer_id].period = 0;
        mtimers[timer_id].expired.broadcast();
        return true;
    }

    bool Timer::waitFor(TimerId timer_id)
    {
        MutexLock locker(m);
        if (timer_id < 0 || timer_id >= int(mtimers.size()) )
        {
            log(Error) << "Invalid timer id" << endlog();
            return false;
        }
        if (mtimers[timer_id].expires == 0) return false;

        return mtimers[timer_id].expired.wait(m);
    }

    bool Timer::waitForUntil(TimerId timer_id, nsecs abs_time)
    {
        MutexLock locker(m);
        if (timer_id < 0 || timer_id >= int(mtimers.size()) )
        {
            log(Error) << "Invalid timer id" << endlog();
            return false;
        }
        if (mtimers[timer_id].expires == 0) return false;

        return mtimers[timer_id].expired.wait_until(m, abs_time);
    }



}
