#ifndef ORO_EXECUTION_ENGINE_HPP
#define ORO_EXECUTION_ENGINE_HPP

#include <corelib/RunnableInterface.hpp>
#include <corelib/TaskInterface.hpp>
#include <execution/CommandProcessor.hpp>
#include <execution/ProgramProcessor.hpp>
#include <execution/StateMachineProcessor.hpp>

namespace ORO_OS {
    class Semaphore;
}
namespace ORO_Execution
{
    /**
     * An execution engine serialises the execution of all commands, programs,
     * state machines and incomming events for a task.
     * Any 'concurrent' activity executing in the same execution engine
     * is guaranteed to be non preempting, thus thread-safe.
     */
    class ExecutionEngine
        : public ORO_CoreLib::RunnableInterface
    {
    protected:
        ORO_OS::Semaphore* work_sem;
        ORO_OS::Semaphore* loop_sem;
        TaskContext*     taskc;
        ExecutionEngine* mainee;

        CommandProcessor* cproc;
        ProgramProcessor* pproc;
        StateMachineProcessor* smproc;
        ORO_CoreLib::EventProcessor* eproc;

        std::vector<ExecutionEngine*> children;

        bool eerun;
    public:
        /**
         * Create an execution engine with a CommandProcessor, ProgramProcessor 
         * and StateMachineProcessor. The EventProcessor is the event processor
         * of this task. If you provide another ExecutionEngine as argument,
         * this execution engine delegates all requests to that execution engine
         * and does itself nothing.
         */
        ExecutionEngine( TaskContext* owner, ExecutionEngine* other_ee = 0 );
        
        ~ExecutionEngine();

        virtual bool initialize();

        virtual void step();
        virtual void loop();
        virtual bool breakLoop();

        virtual void finalize();

        virtual void setTask(ORO_CoreLib::TaskInterface* t);

        ExecutionEngine* getParent();

        TaskContext* getTaskContext() { return taskc; }

        void reparent(ExecutionEngine* new_parent);

        CommandProcessor* commands() const {
            return mainee ? mainee->getCommandProcessor() : cproc;
        }

        ProgramProcessor* programs() const {
            return mainee ? mainee->getProgramProcessor() : pproc;
        }

        StateMachineProcessor* states() const {
            return mainee ? mainee->getStateMachineProcessor() : smproc;
        }

        ORO_CoreLib::EventProcessor* events() const;

        CommandProcessor* getCommandProcessor() const {
            return mainee ? mainee->getCommandProcessor() : cproc;
        }

        ProgramProcessor* getProgramProcessor() const {
            return mainee ? mainee->getProgramProcessor() : pproc;
        }

        StateMachineProcessor* getStateMachineProcessor() const {
            return mainee ? mainee->getStateMachineProcessor() : smproc;
        }

        ORO_CoreLib::EventProcessor* getEventProcessor() const;

    };

}
#endif
