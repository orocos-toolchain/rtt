/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PeriodicTask.hpp 

                        PeriodicTask.hpp -  description
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
 
#ifndef REALTIMETASK_HPP
#define REALTIMETASK_HPP

#include "RunnableInterface.hpp"

#include "Time.hpp"
#include "TaskInterface.hpp"
#include <os/Mutex.hpp>

#pragma interface

namespace ORO_CoreLib
{
    class TaskTimerInterface;
    class TimerThread;
    /**
     * @brief A PeriodicTask is the general implementation of a Task
     * which has (realtime,) periodic constraints.
     *
     * It will execute a RunnableInterface, or the equivalent methods in
     * it's own interface when none is given.
     * When initialize() returns false, it will abort start().
     * If the PeriodicTask is normally stop()'ed, finalize()
     * is called in the calling thread of stop().
     */
    class PeriodicTask
        : public TaskInterface
    {
    public:

        /**
         * @brief Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param period
         *        The periodicity of the PeriodicTask
         * @param thread
         *        The thread this Task will be run in.
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        PeriodicTask(Seconds period, TimerThread* thread, RunnableInterface* r=0 );

        /**
         * @brief Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param sec
         *        The periodicity of the PeriodicTask, seconds partition
         * @param nsec
         *        The periodicity of the PeriodicTask, nanoseconds partition
         * @param thread
         *        The thread this Task will be run in.
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        PeriodicTask(secs sec, nsecs nsec, TimerThread* thread, RunnableInterface* r=0 );

        /**
         * Stops and terminates a PeriodicTask
         */
        virtual ~PeriodicTask();
           
        /**
         * Run exclusively this RunnableInterface.
         *
         * @param r
         *        The RunnableInterface to run exclusively.
         * @return true if succeeded, false otherwise
         */
        virtual bool run( RunnableInterface* r );

        virtual bool start();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual Seconds getPeriod() const;

        virtual TaskThreadInterface* thread() const;

        virtual bool initialize();
        
        virtual void step();
        
        virtual void finalize();

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
        bool inError;

        /**
         * This tasks period in nanoseconds.
         */
        nsecs per_ns;

        /**
         * The thread which runs this task.
         */
        TimerThread* _thread;

        /**
         * The timer which steps this task.
         */
        TaskTimerInterface* _timer;
    };

}

#endif
