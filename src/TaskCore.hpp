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
    public:
        /**
         * Describes the different states a component can have.
         * When a TaskContext is being constructed, it is in the
         * \a Init state. After the construction ends, the
         * component arrives in the \a PreOperational (additional
         * configuration required) or the \a Stopped (ready to run)
         * state. Invoking \a start() will make a transition to the
         * \a Running state and \a stop() back to the \a Stopped state.
         *
         * In order to check if these transitions are allowed, hook functions
         * are executed, which can be filled in by the component builder.
         * - A transition from \a PreOperational to \a Stopped is checked
         * by calling the \a configureHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a PreOperational.
         * - A transition from \a Stopped to \a Running is checked by calling
         * the \a startHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a Stopped.
         * - A transition from \a Running to \a Stopped is always allowed
         * and the \a stopHook() method is called to inform the component of
         * this transtion.
         * - A transition from \a Stopped to \a PreOperational is always allowed
         * and the \a cleanupHook() method is called to inform the component of
         * this transtion.
         *
         */
        enum TaskState { Init,           //! The state during component construction.
                         PreOperational, //! The state indicating additional configuration is required.
                         Stopped,        //! The state indicating the component is ready to run.
                         Running         //! The state indicating the component is running.
        }; 
        
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
         * Returns the current state of the TaskContext.
         */
        TaskState getTaskState() const;

        /**
         * @name Script Methods 
         *
         * The standard script methods of a TaskContext are for
         * configuration and starting and stopping its
         * ExecutionEngine.  @{
         */

        /**
         * This method instructs the component to (re-)read configuration data
         * and try to enter the \a Stopped state. This can only succeed
         * if the component is not running and \a configureHook() returns true.
         */
        virtual bool configure();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a configure() is called. The default
         * implementation is an empty function which returns \a true.
         *
         * @retval true to indicate that configuration succeeded and
         * the Stopped state may be entered.
         * @retval false to indicate that configuration failed and the
         * Preoperational state is entered.
         */
        virtual bool configureHook();

        /**
         * This method starts the execution engine of this component.
         * This function calls \a startHook(), which must return \a true in order to
         * allow this component to run.
         * You can override this method to do something else or in addition
         * to starting the ExecutionEngine.
         * @return false if the engine was not assigned to an ActivityInterface
         * or if startHook() returned false or it was already started.
         */
        virtual bool start();
        
        /**
         * This method stops the execution engine of this component.
         * You can override this method to do something else or in addition
         * to stopping the engine. This function calls cleanupHook() as well.
         * @return false if the engine was not running.
         */
        virtual bool stop();

        /**
         * This method instructs a stopped component to enter the
         * pre-operational state again. It calls cleanupHook().
         * @return true if the component was in the stopped state.
         */
        virtual bool cleanup();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a cleanup() is called. The default
         * implementation is an empty function.
         */
        virtual void cleanupHook();
  
        /**
         * Inspect if the component is in the Running state.
         */
        virtual bool isRunning() const;

        /**
         * Inspect if the component is configured, i.e. in
         * the Stopped or Running state.
         */
        virtual bool isConfigured() const;

        /**
         * Invoke this method to \a execute
         * the ExecutionEngine and the update() method. This method maps to
         * the 'update()' method in the scripting language.
         * @retval false if this->engine()->getActivity()->execute() == false
         * @retval true otherwise.
         */
        virtual bool doUpdate();

        /**
         * Invoke this method to \a trigger the thread of this TaskContext to execute
         * its ExecutionEngine and the update() method. This method maps to
         * the 'trigger()' method in the scripting language.
         * @retval false if this->engine()->getActivity()->trigger() == false
         * @retval true otherwise.
         */
        virtual bool doTrigger();
        /**
         *@}
         */

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a start() is called. The default implementation is an
         * empty function which returns \a true.
         * @retval true to indicate that the component may run and
         * the Running state may be entered.
         * @retval false to indicate that the component may not run and the
         * Stopped state is entered.
         */
        virtual bool startHook();

        /**
         * Function where the user must insert his 'startup' code.
         * This function is called by the ExecutionEngine before it
         * starts its processors. If it returns \a false, the startup
         * of the TaskCore is aborted.  The default implementation is an
         * empty function which returns \a true.
         * @deprecated Use the startHook() function instead.
         */
        virtual bool startup();

        /**
         * Function where the user must insert his 'application' code.
         * When the ExecutionEngine's Activity is a PeriodicActivity, this
         * function is called by the ExecutionEngine in each periodic
         * step after all command, event,... are processed. When it is executed by a
         * NonPeriodicActivity, this function is called after an Event or
         * Command is received and executed.  It should not loop
         * forever, since no commands or events are processed when
         * this function executes.  The default implementation is an
         * empty function.
         */
        virtual void updateHook();

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
         * @deprecated Use the updateHook() function instead.
         */
        virtual void update();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a stop() is called. The default
         * implementation is an empty function.
         */
        virtual void stopHook();

        /**
         * Function where the user must insert his 'shutdown' code.
         * This function is called by the ExecutionEngine after it
         * stops its processors.  The default implementation is an
         * empty function.
         * @deprecated Use the stopHook() function instead.
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

    protected:
        std::string    mtask_name;

        /**
         * The execution engine which calls update() and processes
         * our commands, events etc.
         */
        ExecutionEngine* ee;

        TaskState mTaskState;
    private:
        // non copyable
        TaskCore( TaskCore& );
    };
}

#endif
