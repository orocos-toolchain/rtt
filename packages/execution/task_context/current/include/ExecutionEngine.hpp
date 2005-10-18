#ifndef ORO_EXECUTION_ENGINE_HPP
#define ORO_EXECUTION_ENGINE_HPP

#include <corelib/RunnableInterface.hpp>
#include <corelib/TaskInterface.hpp>
#include <execution/Processor.hpp>

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
        bool cproc_owner, pproc_owner, smproc_owner;

        // to be replaced by classes 'CommandProcessor','ProgramProcessor', and 'StateMachineProcessor'.
        Processor* cproc;
        Processor* pproc;
        Processor* smproc;

    public:
        /**
         * Create an execution engine with a CommandProcessor, ProgramProcessor 
         * and StateMachineProcessor. The EventProcessor is the event processor
         * of this task. If you provide \a cp, \a pp or \a smp, the given Processors
         * will not be executed by this ExecutionEngine and it is assumed that they
         * are under control of an external ExecutionEngine. If none is provided,
         * they are internally created, managed and executed.
         */
        ExecutionEngine( Processor* cp = 0, Processor* pp = 0, Processor* smp = 0 );
        
        ~ExecutionEngine();

        virtual bool initialize();

        virtual void step();

        virtual void finalize();

        virtual void setTask(ORO_CoreLib::TaskInterface* t);

        Processor* getCommandProcessor() const {
            return cproc;
        }

        Processor* getProgramProcessor() const {
            return pproc;
        }

        Processor* getStateMachineProcessor() const {
            return smproc;
        }

        void setCommandProcessor(Processor* cp);

        void setProgramProcessor(Processor* pp);

        void setStateMachineProcessor(Processor* smp);

        ORO_CoreLib::EventProcessor* getEventProcessor() const;
};

}
#endif
