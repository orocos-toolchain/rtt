
#ifndef STATEMACHINE_TASK_HPP
#define STATEMACHINE_TASK_HPP

#include "TaskContext.hpp"
#include "StateMachine.hpp"

namespace ORO_Execution
{
    class StateMachineTask;
    typedef boost::shared_ptr<StateMachineTask> StateMachineTaskPtr;

    /**
     * @brief This class represents a stateMachine as a task in
     * the Orocos TaskContext system.
     */
    class StateMachineTask
        : public TaskContext
    {
        //boost::weak_ptr<StateMachine> sm;
        // used when state machines are copied.
        // functions have a similar mechanism
        VariableDataSource< StateMachineWPtr >::shared_ptr _this; 
        //VariableDataSource<StateMachineTask*> _this; 
        bool true_gen() const;

        CommandFactoryInterface* createCommandFactory();
        DataSourceFactoryInterface* createDataSourceFactory();

    public:
        StateMachineTask* copy(StateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate );

        /**
         * By constructing this object, a stateMachine is added to a taskcontext
         * as a TaskContext, with its commands and methods.
         */
        StateMachineTask(StateMachinePtr statemachine, ExecutionEngine* ee=0);

        ~StateMachineTask();

        /**
         * Returns the StateMachine of this task.
         */
        StateMachinePtr getStateMachine() const { return _this->get().lock(); }

    };
}

#endif


