/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  Timer.hpp

                        Timer.hpp -  description
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


#ifndef ORO_RTT_TIMER_HPP
#define ORO_RTT_TIMER_HPP

#include "../Time.hpp"
#include "../os/TimeService.hpp"
#include "os/Mutex.hpp"
#include "os/Semaphore.hpp"
#include "os/ThreadInterface.hpp"
#include "../base/RunnableInterface.hpp"
#include <vector>
#include <utility>

namespace RTT
{
    /**
     * This class allows to define a timer object which can be
     * armed in single shot and periodic mode. In Order to use this
     * class, derive your class from Timer and implement the timeout()
     * method.
     * The resolution of this class depends completely on the timer
     * resolution of the underlying operating system.
     *
     * If you do not attach an activity, the Timer will create a thread
     * of its own and start it. That thread will be stopped and cleaned up
     * when the Timer is destroyed.
     */
    class RTT_API Timer
        : protected RunnableInterface
    {
    public:
        /**
         * A positive numeric ID representing a timer.
         */
        typedef int TimerId;
    protected:
        TimeService* mTimeserv;
        OS::ThreadInterface* mThread;
        OS::Semaphore msem;
        mutable OS::Mutex m;
        typedef TimeService::nsecs Time;
        /**
         * Index in vector is the timer id.
         * 1st Time is the absolute time upon which the timer expires.
         * 2nd Time is the optional period of the timer.
         */
        typedef std::vector<std::pair<Time, Time> > TimerIds;
        TimerIds mtimers;
        bool mdo_quit;

        bool initialize();
        void finalize();
        void step();

        // The main 'event' loop.
        void loop();

        bool breakLoop();

    public:
        /**
         * Create a timer object which can hold \a max_timers timers.
         * A Timer must be executed in a non periodic thread (or the main thread)
         * or it will refuse to start.
         * If \a scheduler is set to -1 (default) no thread is created and you need
         * to attach a thread yourself to this Timer.
         * @param max_timers The initial amount of timers this Timer can monitor.
         * Keep as low as possible. See also setMaxTimers().
         * @param scheduler The Orocos scheduler type for this timer. ORO_SCHED_OTHER or ORO_SCHED_RT or
         * -1 to attach your own thread.
         * @param priority The priority within the \a scheduler of this timer.
         */
        Timer(TimerId max_timers, int scheduler = -1, int priority = 0);

        ~Timer();

        /**
         * This function is called each time an armed or periodic timer expires.
         * The user must implement this method to catch the time outs.
         * @param timer_id The number of the timer that expired.
         */
        virtual void timeout(TimerId timer_id);

        /**
         * Change the maximum number of timers in this object.
         * Any added timer with id >= \a max will be removed.
         */
        void setMaxTimers(TimerId max);

        /**
         * Start a periodic timer which starts first over \a period
         * seconds and then every \a period seconds.
         * @param timer_id The number of the timer, starting from zero.
         * @param period The period when the timer should expire.
         * This is a floating point number.
         * @see killTimer to disable it again.
         */
        bool startTimer(TimerId timer_id, Seconds period);

        /**
         * Arm a timer to fire once over \a wait_time seconds.
         * @param timer_id The number of the timer, starting from zero.
         * @param wait_time The time in seconds from now, when the
         * timer should expire. This is a floating point number.
         * @see killTimer to disable it before it fires.
         */
        bool arm(TimerId timer_id, Seconds wait_time);

        /**
         * Returns the remaining time before this timer elapses.
         * @retval 0.0 if the timer is not armed or has already elapsed.
         * @return the remaining time in seconds.
         */
        TimeService::Seconds timeRemaining(TimerId timer_id) const;

        /**
         * Check if a given timer id is armed.
         * @param timer_id The number of the timer, starting from zero.
         */
        bool isArmed(TimerId timer_id) const;

        /**
         * Disable an armed timer.
         * @param timer_id The number of the timer, starting from zero.
         */
        bool killTimer(TimerId timer_id);

    };
}

#endif
