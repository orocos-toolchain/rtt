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

namespace ORO_CoreLib
{
    /**
     * @brief Interface to start/stop and query a Task.
     *
     * A TaskInterface provides the control methods
     * for tasks. They can be started, stopped
     * and be queried for their state and period.
     *
     * It is complementary to the RunnableInterface, which
     * defines the methods for the functionality.
     */
    class TaskInterface
    {
    public:
        virtual ~TaskInterface() {}

        /**
         * Start the task.
         * 
         * @return true if the task is started, false otherwise
         */
        virtual bool start() = 0;

        /**
         * Stop the task
         *
         * @return true if the task is stopped, false otherwise
         */
        virtual bool stop() = 0;

        /**
         * Query for the tasks state.
         *
         * @return true if it is running, false otherwise
         */
        virtual bool isRunning() = 0;

        /**
         * Get the periodicity of this task in Seconds
         *
         * @return The execution period of this task
         */
        virtual Seconds periodGet() = 0;
    };

}

#endif
