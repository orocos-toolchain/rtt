/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TaskTimerInterface.hpp 

                        TaskTimerInterface.hpp -  description
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
 
 
#ifndef TASKTIMERINTERFACE_HPP
#define TASKTIMERINTERFACE_HPP

#include "Time.hpp"

namespace ORO_CoreLib
{
    class PeriodicTask;
    /**
     * An TaskTimerInterface is an object that will step() a PeriodicTask every
     * n'th time it is tick()'ed such that when n tasks are present,
     * each task will be stepped on a different tick call of one period.
     * It will try to spread the stepping
     * depending on the task's periods and tick period.
     * The former is the period between two consecutive step() calls,
     *  the latter, the elapsed time between two tick() calls.
     *
     */
    class TaskTimerInterface
    {
    public:
        virtual ~TaskTimerInterface() {}

        /**
         * The maximum number of tasks each timer will handle, thus,
         * the max number of tasks of the same priority.
         */
        static const unsigned int MAX_TASKS = 64;

        /**
         * Add a task which must be step()'ed.
         */
        virtual bool addTask( PeriodicTask* t ) = 0;

        /**
         * Remove a task which must no longer be step()'ed.
         */
        virtual bool removeTask( PeriodicTask* t ) = 0;

        /**
         * Execute a PeriodicTask, according to the average period.
         */
        virtual void tick() = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the tasks are step()'ed.
         */
        virtual nsecs getPeriod() const = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the timer is tick()'ed.
         */
        virtual nsecs getTrigger() const = 0;

        /**
         * Sets the (average) period at which this timer will be ticked.
         */
        virtual void setTrigger(nsecs ns) = 0;
        
    };

}

#endif
