/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  TaskTimerLockFree.hpp 

                        TaskTimerLockFree.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef TASKLOCKFREETIMER_HPP
#define TASKLOCKFREETIMER_HPP

#include "TaskTimerInterface.hpp"
#include <vector>
#include "ListLockFree.hpp"

namespace ORO_CoreLib
{
    namespace detail {

    /**
     * A lock-free TaskTimer which step()s all tasks in 'one shot' in the
     * first tick given of each task period. It does discard ticks
     * if there are multiple ticks per task period, hence not spreading
     * task step()s over ticks. If a task in this timer tries to start
     * or stop another task in this timer, the effect will only be in the next tick.
     */
    class TaskTimerLockFree
        : public TaskTimerInterface
    {
        typedef ListLockFree<PeriodicTask*> TaskList ;
        nsecs trig_per;
        nsecs task_per;
        int   turn;
        TaskList tasks;
    public:
        TaskTimerLockFree( nsecs task_period );

        virtual ~TaskTimerLockFree();

        virtual void start();

        virtual void stop();

        virtual bool addTask( PeriodicTask* t );

        virtual bool removeTask( PeriodicTask* t );

        virtual void tick();

        virtual nsecs getPeriod() const { return task_per; }

        virtual nsecs getTrigger() const { return trig_per; }

        virtual void setTrigger(nsecs ns) { trig_per = ns; }

    };
    }
}

#endif
