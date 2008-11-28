#ifndef OROCOS_RTT_DATA_DRIVEN_TASK_HPP
#define OROCOS_RTT_DATA_DRIVEN_TASK_HPP

#include "TaskContext.hpp"
#include <set>

namespace RTT {
    class DataDrivenTask : public TaskContext
    {
    protected:
        std::vector< PortInterface* > updated_ports;
        /**
         * This callback is called each time data arrived on an
         * event port.
         */
        void dataOnPort(PortInterface*);
    public:
        /**
         * Create a DataDrivenTask.
         *
         * It's ExecutionEngine will be newly constructed with private
         * processing of commands, events, programs and state machines.
         *
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        DataDrivenTask( const std::string& name, TaskState initial_state = Stopped );

        /**
         * Create a DataDrivenTask.
         *
         * Its commands programs and state machines are processed by \a parent.
         * Use this constructor to share execution engines among task contexts, such that
         * the execution of their functionality is serialised (executed in the same thread).
         *
         * @param name The name of this component.
         * @param initial_state Provide the \a PreOperational parameter flag here
         * to force users in calling configure(), before they call start().
         */
        DataDrivenTask(const std::string& name, ExecutionEngine* parent, TaskState initial_state = Stopped );

        /**
         * Starts this task. It creates a separate thread for it and starts
         * waiting for new data to be available on input ports
         */
        bool start();

        /**
         * Hook called in the Running state.
         *
         * The default implementation calls updateHook(updated_ports)
         */
        void updateHook();

        /**
         * This method gets called when new data is available on some input ports. The ports
         * are listed as argument to the method
         *
         * The default implementation does nothing;
         */
        virtual void updateHook(std::vector<PortInterface*> const& updated_ports);
    };
}

#endif

