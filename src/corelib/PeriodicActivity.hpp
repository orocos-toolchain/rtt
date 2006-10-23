/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PeriodicActivity.hpp 

                        PeriodicActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef ORO_PERIODIC_ACTIVITY_HPP
#define ORO_PERIODIC_ACTIVITY_HPP

#include "RunnableInterface.hpp"

#include "Time.hpp"
#include "ActivityInterface.hpp"
#include "TimerThread.hpp"
#include "os/Mutex.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    namespace detail {
        class TimerInterface;
    }

    /**
     * @brief A PeriodicActivity is the general implementation of a Activity
     * which has (realtime) periodic constraints.
     *
     * A PeriodicActivity is executed in a TimerThread. Multiple
     * PeriodicActivities executing in the same TimerThread will be
     * executed one after the other.  If, by user choice, the
     * PeriodicActivity has a period which is a multiple (n) of the
     * thread's period, it may be executed every n'th execution period
     * of that thread.
     * 
     * It will execute a RunnableInterface, or the equivalent methods in
     * it's own interface when none is given.
     * When initialize() returns false, it will abort start().
     * If the PeriodicActivity is stop()'ed, finalize()
     * is called in the calling thread of stop().
     * @ingroup CoreLibActivities
     */
    class PeriodicActivity
        : public ActivityInterface
    {
    public:

        /**
         * @brief Create a Periodic Activity with a given priority and period.
         *
         * @param priority
         *        The priority of this activity. A lookup will be done to locate a suitable
         *        TimerThread with the same priority. If it exists and it has the same \a period,
         *        this activity will be executed in that thread. Otherwise, a new TimerThread is created.
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param r
         *        The optional RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(int priority, Seconds period, RunnableInterface* r=0 );

        /**
         * @brief Create a Periodic Activity executing in a given thread.
         *
         * @param thread
         *        The thread this Activity will be run in. Both period and priority of this activity
         *        will be the same as \a thread. 
         * @param r
         *        The optional RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(TimerThreadPtr thread, RunnableInterface* r=0);

        /**
         * @brief Create a Periodic Activity with a given period and thread.
         *
         * @param period
         *        The periodicity of the PeriodicActivity
         * @param thread
         *        The thread this Activity will be run in. The given \a period must be a
         *        multiple of the thread's period.
         * @param r
         *        The optional RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(Seconds period, TimerThreadPtr thread, RunnableInterface* r=0 );

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
         *        The optional RunnableInterface to run exclusively within this Activity
         */
        PeriodicActivity(secs sec, nsecs nsec, TimerThreadPtr thread, RunnableInterface* r=0 );

        /**
         * Stops and terminates a PeriodicActivity
         */
        virtual ~PeriodicActivity();
           
        /**
         * Run exclusively this RunnableInterface.
         *
         * @param r
         *        The RunnableInterface to run exclusively.
         * @return true if succeeded, false otherwise
         */
        bool run( RunnableInterface* r );

        virtual bool start();

        virtual bool execute();

        virtual bool trigger();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool isActive() const;

        virtual bool isPeriodic() const;

        virtual Seconds getPeriod() const;

        virtual OS::ThreadInterface* thread();

        /**
         * @see RunnableInterface::initialize()
         */
        virtual bool initialize();
        
        /**
         * @see RunnableInterface::step()
         */
        virtual void step();
        
        /**
         * @see RunnableInterface::finalize()
         */
        virtual void finalize();

        /**
         * Called by the TimerInterface.
         */
        void doStep();
    protected:
        void init();

        /**
         * When runner != 0 it will be executed instead of
         * this instances initialize(), step() and finalize() functions
         */
        RunnableInterface* runner;

        /**
         * State info.
         */
        bool running;

        /**
         * State info.
         */
        bool active;

        /**
         * This activity's period in nanoseconds.
         */
        nsecs per_ns;

        /**
         * The thread which runs this activity.
         */
        TimerThreadPtr thread_;

        /**
         * The timer which steps this activity.
         */
        detail::TimerInterface* timer_;
    };

}

#endif
