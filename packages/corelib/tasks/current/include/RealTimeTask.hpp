/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RealTimeTask.hpp 

                        RealTimeTask.hpp -  description
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
    class TaskTimer;
    /**
     * @brief A RealTimeTask is the general implementation of a Task
     * which has realtime, periodic constraints.
     *
     * It will execute a RunnableInterface, or the equivalent methods in
     * it's own interface when none is given.
     * When initialize() returns false, it will finalize() it in the
     * CompletionProcessor. If the RealTimeTask is normally stop()'ed, finalize()
     * is called in the calling thread of stop().
     */
    class RealTimeTask
        : public TaskInterface
    {
	protected:
        friend class TaskTimer;
    public:

        /**
         * @brief Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param period
         *        The periodicity of the RealTimeTask
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        RealTimeTask(Seconds period, RunnableInterface* r=0 );

        /**
         * @brief Create a RealTime Task with a given period which runs
         * a RunnableInterface.
         *
         * @param sec
         *        The periodicity of the RealTimeTask, seconds partition
         * @param nsec
         *        The periodicity of the RealTimeTask, nanoseconds partition
         * @param r
         *        The optional RunnableInterface to run exclusively within this Task
         */
        RealTimeTask(secs sec, nsecs nsec, RunnableInterface* r=0 );

        /**
         * Stops and terminates a RealTimeTask
         */
        virtual ~RealTimeTask();
           
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

        virtual Seconds periodGet() const;

    protected:
        /**
         * Adds the task to the corresponding thread
         */
        virtual bool taskAdd()=0;

        /**
         * Removes this task from the corresponding thread
         */
        virtual void taskRemove()=0; 

        virtual bool initialize() { return true; }
        
        virtual void step() {}
        
        virtual void finalize() {}

        //virtual TaskInterface* taskGet( ) const { return this; }; // deprecated

        /**
         * Calls the runners or own step function
         */
        void doStep();

        /**
         * Does an unconditional stop.
         */
        void doStop();

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
         * Used when two threads try to stop simultanously.
         */
        ORO_OS::Mutex stop_lock;
    };

}

#endif
