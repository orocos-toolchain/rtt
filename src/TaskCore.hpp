/***************************************************************************
  tag: FMTC  do nov 2 13:06:07 CET 2006  TaskCore.hpp 

                        TaskCore.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_TASK_CORE_HPP
#define ORO_TASK_CORE_HPP

#include "OperationInterface.hpp"
#include "ExecutionEngine.hpp"
#include <string>

namespace RTT
{
    /**
     * The minimal Orocos task. It has a name and an ExecutionEngine
     * to process events,commands, programs, statemachines and run a user function. It
     * is useful for in embedded systems, where communication between
     * tasks may be hard coded and hence, no interface browsing or
     * task browsing is required (for every task).
     */
    class TaskCore
    {
        // non copyable
        TaskCore( TaskCore& );
    protected:
        std::string    mtask_name;

        /**
         * The execution engine which calls update() and processes
         * our commands, events etc.
         */
        ExecutionEngine* ee;
    public:
        /**
         * Create a TaskCore visible with \a name.
         * It's ExecutionEngine will be newly constructed
         * processing its commands, events,
         * programs and state machines.
         */
        TaskCore( const std::string& name );

        /**
         * Create a TaskCore visible with \a name. Its commands
         * programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         */
        TaskCore(const std::string& name, ExecutionEngine* parent );

        virtual ~TaskCore();

        /**
         * Function where the user must insert his 'startup' code.
         * This function is called by the ExecutionEngine before it
         * starts its processors. If it returns \a false, the startup
         * of the TaskCore is aborted.  The default implementation is an
         * empty function which returns \a true.
         */
        virtual bool startup();

        /**
         * Function where the user must insert his 'application' code.
         * When the ExecutionEngine's Activity is a PeriodicActivity, this
         * function is called by the ExecutionEngine in each periodic
         * step after all command, event,... processors. When it's Task is a
         * NonPeriodicActivity, this function is called after an Event or
         * Command is received and executed.  It should not loop
         * forever, since no commands or events are processed when
         * this function executes.  The default implementation is an
         * empty function.
         */
        virtual void update();

        /**
         * Function where the user must insert his 'shutdown' code.
         * This function is called by the ExecutionEngine after it
         * stops its processors.  The default implementation is an
         * empty function.
         */
        virtual void shutdown();

        /**
         * Get the name of this TaskCore.
         */
        const std::string& getName() const
        {
            return mtask_name;
        }

        /**
         * Change the name of this TaskCore.
         */
        void setName(const std::string& n)
        {
            mtask_name = n;
        }

        /**
         * Use this method to re-set the execution engine
         * of this task core. 
         * @param engine The new execution engine which will execute
         * this TaskCore or null if a new execution engine must be
         * created (the old is deleted in that case).
         * @post The TaskCore is being run by \a engine or a new
         * execution engine.
         */
        void setExecutionEngine(ExecutionEngine* engine);

        /**
         * Get a const pointer to the ExecutionEngine of this Task.
         */
        const ExecutionEngine* engine() const
        {
            return ee;
        }

        /**
         * Get a pointer to the ExecutionEngine of this Task.
         */
        ExecutionEngine* engine()
        {
            return ee;
        }
    };
}

#endif
