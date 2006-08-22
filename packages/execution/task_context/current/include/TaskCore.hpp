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
