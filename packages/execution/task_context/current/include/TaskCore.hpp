#ifndef ORO_TASK_CORE_HPP
#define ORO_TASK_CORE_HPP

#include "ExecutionEngine.hpp"

#include <string>

namespace ORO_Execution
{
    /**
     * The minimal Orocos task. It has a name and and ExecutionEngine
     * to process events or commands and run the task's function. It
     * is useful for in embedded systems, where communication between
     * tasks may be hard coded and hence, no interface browsing or
     * task browsing is required (for every task).
     */
    class TaskCore
    {
        // non copyable
        TaskCore( TaskCore& );
    protected:
        std::string    _task_name;
    
        ExecutionEngine ee;
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
         * When the ExecutionEngine's Task is a PeriodicTask, this
         * function is called by the ExecutionEngine in each periodic
         * step after all command, event,... processors. When it's Task is a
         * TaskNonPeriodic, this function is called after an Event or
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
        const std::string& getName()
        {
            return _task_name;
        }

        /**
         * Change the name of this TaskCore.
         */
        void setName(const std::string& n)
        {
            _task_name = n;
        }

        /**
         * Get a const pointer to the ExecutionEngine of this Task.
         * @see getExecutionEngine()
         */
        const ExecutionEngine* engine() const
        {
            return &ee;
        }

        /**
         * Get a pointer to the ExecutionEngine of this Task.
         * @see getExecutionEngine()
         */
        ExecutionEngine* engine()
        {
            return &ee;
        }
    };
}

#endif
