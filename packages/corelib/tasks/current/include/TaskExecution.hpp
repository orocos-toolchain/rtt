/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  TaskExecution.h 

                       TaskExecution.h -  description
                          -------------------
   begin                : Wed April 17 2002
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


#ifndef TASKEXECUTION_HPP
#define TASKEXECUTION_HPP


#include "os/PeriodicThread.hpp"
#include "EventProcessor.hpp"

#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

#include <list>

namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;

    class TaskTimer;
    class RealTimeTask;

    /**
     * @brief A Thread executing a task is a
     * PeriodicThread with an EventProcessor.
     */
    struct TaskThreadInterface
        : public ORO_OS::PeriodicThread,
          public EventProcessor
    {
        TaskThreadInterface(int priority, const std::string& name, double periodicity)
            : PeriodicThread( priority, name, periodicity)
        {}
    };

    /**
     * @brief This Periodic Thread is meant for executing a RealTimeTask
     * object periodically.
     *
     * @see PeriodicThread
     */
    class TaskExecution
        : public TaskThreadInterface
    {

        /**
         * A structure to keep track of ownership
         * of timers.
         */
        struct TimerItem
        {
            TimerItem(TaskTimer* ev, bool _owner = false) : timer(ev), owner(_owner) {}
            TaskTimer* timer;
            bool owner;

            struct Locator : public std::binary_function<TimerItem, TaskTimer*, bool>
            {
                bool operator()(const TimerItem& p, const TaskTimer* ev) const
                {
                    return p.timer == ev;
                }
            };
        };

    public:

        /**
         * Destructor
         */
        virtual ~TaskExecution();

        /**
         * Add an Timer that will be ticked every execution period
         */
        void timerAdd( TaskTimer* );

        /**
         * Remove an Timer from being ticked every execution period
         */
        void timerRemove( TaskTimer* );
    protected:
        /**
         * Constructor. To be called from the friend classes.
         *
         * @param priority 
         *        The priority of this thread
         * @param periodicity
         *        The periodicity of this thread in seconds (e.g. 0.001 = 1000Hz )
         */
        TaskExecution(int priority, const std::string& name, double periodicity);

        virtual void step();

        /**
         * Add a RealTimeTask which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( RealTimeTask* t, const nsecs n );

        /**
         * Remove a RealTimeTask from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( RealTimeTask* t );
        
        /**
         * Internal method for keeping track of TaskTimer
         * ownership.
         */
        void doTimerAdd( TaskTimer* ev, bool myTimer);

        /**
         * A list containing all the TaskTimer instances
         *  we must tick
         *
         * @see TaskTimer
         */ 
        std::list<TimerItem> clocks;

        ORO_OS::MutexRecursive lock;

    };
} // namespace ORO_CoreLib

#endif
