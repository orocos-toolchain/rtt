/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableTaskInterface.hpp 

                        TaskInterface.hpp -  description
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
 
#ifndef TASK_INTERFACE_HPP
#define TASK_INTERFACE_HPP

#include "Time.hpp"
#include "os/ThreadInterface.hpp"

namespace ORO_CoreLib
{
    class EventProcessor;

    /**
     * @brief Interface to start/stop and query a Task.
     *
     * A TaskInterface provides the control methods
     * for tasks. A task can be periodic, non periodic, event driven
     * or any task object which can be started, stopped
     * and be queried for their state and (optional) period.
     *
     * It is complementary to the RunnableInterface, which
     * defines the methods for the functionality that is executed.
     * @see RunnableInterface
     */
    class TaskInterface
    {
    public:
        virtual ~TaskInterface() {}

        /**
         * Start the task.
         * This will call RunnableInterface::initialize() and upon
         * success, effectively start the task, by running the
         * RunnableInterface::step() or RunnableInterface::loop() in
         * a thread.
         * @see isPeriodic()
         * 
         * @return true if the task is started, false otherwise
         */
        virtual bool start() = 0;

        /**
         * Stop the task
         * This will stop the task by removing it from the 'run-queue'
         * of a thread or call RunnableInterface::breakLoop().
         * If no errors occured, RunnableInterface::finalize() is called.
         * @see isPeriodic()
         *
         * @return true if the task is stopped, false otherwise
         */
        virtual bool stop() = 0;

        /**
         * Query for the tasks state.
         *
         * @return true if it is running, false otherwise
         */
        virtual bool isRunning() const = 0;

        /**
         * Get the periodicity of this task in Seconds
         *
         * @return The execution period of this task (zero if !this->isPeriodic() ).
         */
        virtual Seconds getPeriod() const = 0;

        /**
         * Inspect if this task is periodic. If so, it will call RunnableInterface::step().
         * If the task is not periodic, it will call RunnableInterface::loop().
         *
         * @return true if periodic.
         */
        virtual bool isPeriodic() const = 0;

        /**
         * Returns a pointer to the EventProcessor which will
         * process the asynchronous Events of this task. Will not be null.
         */
        virtual EventProcessor* getEventProcessor() const = 0;

        /**
         * Returns a pointer to the thread which will
         * run this task. Will not be null.
         */
        virtual ORO_OS::ThreadInterface* thread() = 0;
    };

}

#endif
