/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TaskTimerSequencer.hpp 

                        TaskTimerSequencer.hpp -  description
                           -------------------
    begin                : Fri February 11 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef ORO_TASKTIMERSEQUENCER_HPP
#define ORO_TASKTIMERSEQUENCER_HPP

#include "Time.hpp"
#include "os/Mutex.hpp"
#include "TaskTimerInterface.hpp"

#include <list>
#include <functional>

namespace ORO_CoreLib
{

    /**
     * An TaskTimerSequencer is an object that will step() a PeriodicTask every
     * n'th time it is tick()'ed such that when n tasks are present,
     * each task will be stepped on a different tick call of one period.
     * It will try to spread the stepping
     * depending on the task's periods and trigger period.
     * The former is the period between two consecutive step() calls,
     *  the latter, the elapsed time between two tick() calls.
     *
     * The implementation is based on <std::list>. This allows safe
     * add/remove of handlers during tick().
     *
     * @todo More efficient implementation if average_period == trigger_period.
     */
    class TaskTimerSequencer
        : public TaskTimerInterface
    {
    public:
        /**
         * Creates an TaskTimerSequencer which will have step() all tasks after
         * <average_period> / <trigger_period> times it is ticked.
         *
         * @param average_period The Average time between step()'ing each
         *        registered listener.
         * @param trigger_period The period between consecutive tick() calls.
         *        Defaults to average_period.
         */
        TaskTimerSequencer( Seconds average_period, Seconds trigger_period = 0 );

        /**
         * Creates an TaskTimerSequencer which will have step()'ed each task
         * after tick() was called <divider> times.
         *
         * @param divider The average number of times tick() is called before
         *        each task is step()'ed. ( So N Tasks will all be 
         *        step()'ed after tick() was called <divider> times. )
         */
        TaskTimerSequencer( unsigned int divider );

        void tick();

        bool addTask( PeriodicTask* task );

        bool removeTask( PeriodicTask* task );

        nsecs getPeriod() const;

        nsecs getTrigger() const;

        void setTrigger(nsecs ns);
        
    protected:

        struct ListItem
        {
            // The default constructor is called when the 
            // list is resized().
            ListItem() : task(0), priority(~0), turn(0) {}
            PeriodicTask* task;
            unsigned int priority;
            unsigned int turn;
        };

        struct CMP : public std::binary_function<ListItem, PeriodicTask*, bool>
        { 
            bool operator()(const ListItem& p, const PeriodicTask* task) const
            {
                return p.task == task;
            }
        };

        struct PrioritySort : public std::binary_function<ListItem, ListItem, bool>
        {
            bool operator()(const ListItem& f, const ListItem& s) const
            {
                return f.priority < s.priority;
            }
        };

        inline
        void update();

        nsecs avPeriod;

        nsecs trigPeriod;

        unsigned long relPeriod;

        /**
         * monotonic increasing number.
         */
        unsigned long total_added;

        unsigned long turn;

        std::list<ListItem> sortedList;

        std::list<ListItem>::iterator f_iter;

        bool reset_f_iter;

        ORO_OS::MutexRecursive mut;

        bool in_tick;
    };

}

#endif
