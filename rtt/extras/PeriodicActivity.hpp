/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PeriodicActivity.hpp

                        PeriodicActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef ORO_PERIODIC_ACTIVITY_HPP
#define ORO_PERIODIC_ACTIVITY_HPP

#include "../base/RunnableInterface.hpp"

#include "../Time.hpp"
#include "../base/ActivityInterface.hpp"
#include "TimerThread.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace extras {


    /**
     * @brief A PeriodicActivity is the general implementation of a Activity
     * which has (realtime) periodic constraints.
     *
     * A PeriodicActivity is executed in a TimerThread. Multiple
     * PeriodicActivities having the same priority and periodicity will be executed
     * in the same TimerThread one after the other.
     *
     * It will execute a base::RunnableInterface, or the equivalent methods in
     * it's own interface when none is given.
     * When initialize() returns false, it will abort start().
     * If the PeriodicActivity is stop()'ed, finalize()
     * is called in the calling thread of stop().
     * @ingroup CoreLibActivities
     */
    class RTT_API PeriodicActivity
        : public base::ActivityInterface
    {
    public:

        /**
         * @brief Create a Periodic Activity with a given priority and period. The default
         * scheduler for PeriodicActivity objects is ORO_SCHED_RT.
         *
         * @param priority
         *        The priority of this activity. A lookup will be done to locate a suitable
         *        TimerThread with the same priority. If it exists and it has the same \a period,
         *        this activity will be executed in that thread. Otherwise, a new TimerThread is created.
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(int priority, Seconds period, base::RunnableInterface* r=0 );

        /**
         * @brief Create a Periodic Activity with a given scheduler type, priority and period.
         *
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority
         *        The priority of this activity. A lookup will be done to locate a suitable
         *        TimerThread with the same priority. If it exists and it has the same \a period,
         *        this activity will be executed in that thread. Otherwise, a new TimerThread is created.
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(int scheduler, int priority, Seconds period, base::RunnableInterface* r=0 );

        /**
         * @brief Create a Periodic Activity with a given scheduler type, priority and period.
         *
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority
         *        The priority of this activity. A lookup will be done to locate a suitable
         *        TimerThread with the same priority. If it exists and it has the same \a period,
         *        this activity will be executed in that thread. Otherwise, a new TimerThread is created.
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param cpu_affinity
         *        The prefered cpu to run on (a mask)
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(int scheduler, int priority, Seconds period, unsigned cpu_affinity, base::RunnableInterface* r=0 );


        /**
         * @brief Create a Periodic Activity executing in a given thread.
         *
         * @param thread
         *        The thread this Activity will be run in. Both period and priority of this activity
         *        will be the same as \a thread.
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(TimerThreadPtr thread, base::RunnableInterface* r=0);

        /**
         * @brief Create a Periodic Activity with a given period and thread.
         *
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param thread
         *        The thread this Activity will be run in. The given \a period must be a
         *        multiple of the thread's period.
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(Seconds period, TimerThreadPtr thread, base::RunnableInterface* r=0 );

        /**
         * @brief Create a Periodic Activity with a given period and thread.
         *
         * @param sec
         *        The periodicity of the PeriodicActivity, seconds partition
         * @param nsec
         *        The periodicity of the PeriodicActivity, nanoseconds partition
         * @param thread
         *        The thread this Activity will be run in. The given period (\a sec, \a nsec) must be a
         *        multiple of the thread's period.
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(secs sec, nsecs nsec, TimerThreadPtr thread, base::RunnableInterface* r=0 );

        /**
         * Stops and terminates a PeriodicActivity
         */
        virtual ~PeriodicActivity();

        virtual bool start();

        virtual bool execute();

        virtual bool trigger();

        virtual bool timeout();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool isActive() const;

        virtual bool isPeriodic() const;

        virtual Seconds getPeriod() const;

        virtual bool setPeriod(Seconds s);

        virtual unsigned getCpuAffinity() const;

        virtual bool setCpuAffinity(unsigned cpu);

        virtual os::ThreadInterface* thread();

        /**
         * @see base::RunnableInterface::initialize()
         */
        virtual bool initialize();

        /**
         * @see base::RunnableInterface::step()
         */
        virtual void step();

        /**
         * @see base::RunnableInterface::work()
         */
        virtual void work(base::RunnableInterface::WorkReason reason);

        /**
         * @see base::RunnableInterface::finalize()
         */
        virtual void finalize();

    protected:
        void init();

        /**
         * State info.
         */
        bool running;

        /**
         * State info.
         */
        bool active;

        /**
         * The thread which runs this activity.
         */
        TimerThreadPtr thread_;
    };

}}

#endif
