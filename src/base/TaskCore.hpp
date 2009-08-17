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

#include "../interface/OperationInterface.hpp"
#include <string>

namespace RTT
{ namespace base {

    /**
     * The minimal Orocos task. It has a name and an ExecutionEngine
     * to process events,commands, programs, statemachines and run a user function. It
     * is useful for in embedded systems, where communication between
     * tasks may be hard coded and hence, no interface browsing or
     * task browsing is required (for every task).
     */
    class RTT_API TaskCore
    {
    public:
        /**
         * Describes the different states a component can have.  When
         * a TaskContext is being constructed, it is in the \a Init
         * state. After the construction ends, the component arrives
         * in the \a PreOperational (additional configuration
         * required) or the \a Stopped (ready to run) state. Invoking
         * \a start() will make a transition to the \a Running state
         * and \a stop() back to the \a Stopped state. The \a Running
         * state executes the \a updateHook() and processes events and
         * commands. Invoking \a activate() will make a transition to
         * the \a Active state in which only events and commands are
         * processed. Within the \a Active state: \a start() will
         * continue to the \a Running state, and a \a stop() will go
         * back to the \a Stopped state. Finally, there is an \a
         * FatalError state, in which the component can enter by
         * calling the protected method \a fatalError(). In this
         * state, the ExecutionEngine is stopped and \a updateHook()
         * is no longer called.  The public \a resetError() method allows one to
         * leave the \a FatalError state and enter the \a Stopped
         * state, or if the error is unrecoverable, the \a
         * PreOperational state.
         *
         * Next to the fatal error, two run-time error levels are
         * available in the \a Running state as well. These levels
         * allow 'automatic' recovery by the component in case the
         * problem is temporal. When the task is \a Running, it may
         * call the protected \a warning() method in order to signal a
         * possible problem. In this \a RunTimeWarning substate, the \a
         * updateHook() is still called. One can leave this substate
         * by calling the protected \a recovered() method. In case of
         * more severe problems, one may call the protected method \a
         * error() when the component is \a Running. The component
         * will enter the \a RunTimeError state and will cause the \a
         * errorHook() to be called instead of \a updateHook(). When
         * \a recovered() is called, this run-time error state is left
         * and the nominal \a Running state is entered again.
         *
         * In order to keep track of these run-time errors, a counter
         * is provided for each sub-state indicating how many times it
         * was entered. See getWarningCount() and getErrorCount().
         *
         * In order to check if these transitions are allowed, hook functions
         * are executed, which can be filled in by the component builder.
         * - A transition from \a PreOperational to \a Stopped is checked
         * by calling the \a configureHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a PreOperational.
         * - A transition from \a Stopped to \a Running is checked by calling
         * the \a startHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a Stopped.
         * - A transition from \a Stopped to \a Active is checked by calling
         * the \a activateHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a Stopped.
         * - A transition from \a Active to \a Running is checked by calling
         * the \a startHook() method. If this method returns \a true,
         * the transition is made, otherwise, the state remains \a Active.
         * - A transition from \a Running to \a Stopped is always allowed
         * and the \a stopHook() method is called to inform the component of
         * this transtion.
         * - A transition from \a Active to \a Stopped is always allowed
         * and the \a stopHook() method is called to inform the component of
         * this transtion.
         * - A transition from \a Stopped to \a PreOperational is always allowed
         * and the \a cleanupHook() method is called to inform the component of
         * this transtion.
         * - A transition from the \a FatalError state to the \a Stopped state
         * is checked by calling the \a resetHook(): If it returns true, the \a
         * Stopped state is entered, if it returns false, the \a PreOperational
         * state is entered, and the user is as such forced to call \a configure()
         * in order to enter the \a Stopped state again.
         * -
         *
         */
        enum TaskState { Init,           //! The state during component construction.
                         PreOperational, //! The state indicating additional configuration is required.
                         FatalError,     //! The state indicating the component encountered a fatal error and is unable to execute.
                         Stopped,        //! The state indicating the component is ready to run.
                         Active,         //! The state indicating the component is processing events and commands.
                         Running,        //! The state indicating the component is running [green].
                         RunTimeWarning, //! The state indicating that a run-time warning has occured [yellow] and needs attention.
                         RunTimeError    //! The state indicating that a run-time error has occured [red] and needs attention.
        };

        /**
         * Create a TaskCore.
         * It's ExecutionEngine will be newly constructed with private
         * processing of commands, events, programs and state machines.
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        TaskCore( const std::string& name, TaskState initial_state = Stopped  );

        /**
         * Create a TaskCore.
         * Its commands programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        TaskCore(const std::string& name, ExecutionEngine* parent, TaskState initial_state = Stopped );

        virtual ~TaskCore();

        /**
         * Returns the current state of the TaskContext.
         */
        TaskState getTaskState() const;

        /**
         * Returns the number of times the RunTimeWarning state
         * was \b entered.
         */
        int getWarningCount() const;

        /**
         * Returns the number of times the RunTimeError state
         * was \b entered.
         */
        int getErrorCount() const;

        /**
         * @name Script Methods
         *
         * These TaskCore functions are exported in a TaskContext as script
         * methods and are for configuration, starting and stopping
         * its ExecutionEngine.  @{
         */

        /**
         * This method instructs the component to (re-)read configuration data
         * and try to enter the \a Stopped state. This can only succeed
         * if the component is not running and \a configureHook() returns true.
         */
        virtual bool configure();

        /**
         * This method starts the execution of this component in order to
         * process events and commands. This function calls \a activateHook()
         * which must return \a true in order to allow this processing.
         * You can override this method to do something else or in addition
         * to starting the ExecutionEngine.
         * @retval false
         * - if activateHook() returned false
         * - if the engine was not assigned to an ActivityInterface
         * - if the component was already active.
         * @retval true if the \a Active state was entered.
         */
        virtual bool activate();

        /**
         * This method starts the execution engine of this component (if not
         * already running) and executes the \a updateHook() with each period.
         * This function calls the user function \a startHook(), which must return \a true in order to
         * allow this component to run.
         * @retval false
         * - if startHook() returned false
         * - if the engine was not assigned to an ActivityInterface
         * - if the component was already running.
         * @retval true if the \a Running state was entered.
         */
        virtual bool start();

        /**
         * This method stops the execution engine of this component.
         * You can override this method to do something else or in addition
         * to stopping the engine. This function calls cleanupHook() as well.
         * @return false if the component was not \a Running or not \a Active.
         */
        virtual bool stop();

        /**
         * This method instructs a stopped component to enter the
         * pre-operational state again. It calls cleanupHook().
         * The run-time error and warning counters are reset to zero as well.
         * @return true if the component was in the \a Stopped state.
         */
        virtual bool cleanup();

        /**
         * If the component entered the \a FatalError state, call this method
         * to recover. It will call the user function \a resetHook(): if
         * it returns false, the \a PreOperational state is entered, if
         * it returns true, the \a Stopped state is entered.
         * You can not use this method to recover from the RunTimeWarning or
         * RunTimeError states, use recover() instead.
         * @retval true if the component is in the Stopped or PreOperational state.
         * @retval false if the component was not in the FatalError state.
         */
        virtual bool resetError();

        /**
         * Inspect if the component is configured, i.e. in
         * the Stopped, Active or Running state.
         */
        virtual bool isConfigured() const;

        /**
         * Inspect if the component is processing events, i.e. in
         * the Active or Running state.
         */
        virtual bool isActive() const;

        /**
         * Inspect if the component is in the Running, RunTimeWarning or RunTimeError state.
         * As RunTimeWarning and RunTimeError are substates of Running, this method
         * also returns true when the component is in one of these states.
         * See inRunTimeError() and inRunTimeWarning() for testing these error
         * conditions.
         */
        virtual bool isRunning() const;

        /**
         * Get the configured execution period of this component. Note that this
         * value only is used when the component isActive() or isRunning().
         * @retval 0.0 if the component is non-periodic (event based).
         * @retval a negative number when the component is not executable.
         * @retval a positive value when the component is periodic. The period is
         * expressed in seconds.
         * @todo: add a bool setPeriod(double) function to allow changing
         * the period at run-time.
         */
        virtual double getPeriod() const;

        /**
         * Inspect if the component is in the FatalError state.
         */
        virtual bool inFatalError() const;

        /**
         * Inspect if the component is in the RunTimeWarning state.
         */
        virtual bool inRunTimeWarning() const;

        /**
         * Inspect if the component is in the RunTimeError state.
         */
        virtual bool inRunTimeError() const;

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
         * Implement this method such that it contains the code which
         * will be executed when \a cleanup() is called. The default
         * implementation is an empty function.
         */
        virtual void cleanupHook();

        /**
         * Implement this method such that it contains the code which
         * will be executed when \a activate() is called. Return false
         * if the component may not be activated.
         */
        virtual bool activateHook();

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
         * When the ExecutionEngine's Activity is a extras::PeriodicActivity, this
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
         * Implement this method to contain code that must be executed
         * in the RunTimeError state, instead of updateHook(). This allows
         * you to specify the behaviour in an erroneous component.
         * errorHook() is called as long as the component is not
         * recovered(). After recovered(), the updateHook() is called again.
         */
        virtual void errorHook();

        /**
         * Function where the user must insert his 'application' code.
         * When the ExecutionEngine's Activity is a extras::PeriodicActivity, this
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
         * Implement this method to recover from the FatalError state.
         * Return false if no recovery was possible and the
         * \a PreOperational state must be entered. Return true
         * to allow transition to the \a Stopped state.
         */
        virtual bool resetHook();

        /**
         * Call this method in a Running state to indicate a
         * run-time warning condition. If the warning condition is solved,
         * call recovered().
         */
        virtual void warning();

        /**
         * Call this method in a Running state to indicate a
         * run-time error condition. errorHook() will be called
         * instead of updateHook(). If the warning condition is solved,
         * call recovered().
         */
        virtual void error();

        /**
         * Call this method from any place to indicate that this
         * component encountered a fatal error.  The ExecutionEngine
         * is stopped, stopHook() is called and the component waits
         * for a resetError().
         */
        virtual void fatal();

        /**
         * Call this method in a Running state to indicate that the
         * run-time warning or error conditions are gone and nominal
         * operation is resumed. You can not use this method to
         * recover from a fatal() error. Use resetError() instead.
         */
        virtual void recovered();

        // Required to set mTaskState to Running or Stopped.
        // As an alternative, one could query the EE.
        friend class ::RTT::ExecutionEngine;

        std::string    mtask_name;

        /**
         * The execution engine which calls update() and processes
         * our commands, events etc.
         */
        ExecutionEngine* ee;

        TaskState mTaskState;

        int runtime_warnings;
        int runtime_errors;
    private:
        // non copyable
        TaskCore( TaskCore& );
    };
}}

#endif
