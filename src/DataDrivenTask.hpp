#ifndef OROCOS_RTT_DATA_DRIVEN_TASK_HPP
#define OROCOS_RTT_DATA_DRIVEN_TASK_HPP

#include "TaskContext.hpp"

namespace RTT {
    class DataDrivenTask : public TaskContext
    {
    public:
        /**
         * Create a DataDrivenTask.
         * It's ExecutionEngine will be newly constructed with private 
         * processing of commands, events, programs and state machines.
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        DataDrivenTask( const std::string& name, TaskState initial_state = Stopped );

        /**
         * Create a DataDrivenTask. 
         * Its commands programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        DataDrivenTask(const std::string& name, ExecutionEngine* parent, TaskState initial_state = Stopped );

        /**
         * Starts this task. It creates a separate thread for it and starts
         * listening to the trigger events.
         */
        bool start();
    };
}

#endif

