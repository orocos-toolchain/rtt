
#include <corelib/CompletionProcessor.hpp>
#include <execution/ExecutionEngine.hpp>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;

    ExecutionEngine::ExecutionEngine( Processor* cp /*= 0*/, Processor* pp /*= 0*/, Processor* smp /*= 0*/ )
        : cproc_owner( cp ? false:true), pproc_owner(false), smproc_owner(false),
          cproc(cp ? cp : new Processor() ),
          pproc(cproc),
          smproc(cproc)
    {
    }
        
    ExecutionEngine::~ExecutionEngine()
    {
        if (cproc_owner)
            delete cproc;
        if (pproc_owner)
            delete pproc;
        if (smproc_owner)
            delete smproc;
    }

    void ExecutionEngine::setTask(TaskInterface* t)
    {
        RunnableInterface::setTask(t);
        if (cproc_owner)
            cproc->setTask(t);
        if (pproc_owner)
            pproc->setTask(t);
        if (smproc_owner)
            smproc->setTask(t);
    }

    bool ExecutionEngine::initialize() {
        if ( cproc_owner )
            return cproc->initialize();
        //             if ( cproc->initialize() )
        //                 if ( pproc->initialize() )
        //                     if( smproc->initialize() )
        //                         return true;
        //                     else {
        //                         pproc->finalize();
        //                         cproc->finalize();
        //                     }
        //                 else {
        //                     cproc->finalize();
        //                 }
        //             return false;
        return true;
    }

    void ExecutionEngine::step() {
        //             pproc->step();
        //             smproc->step();
        if ( cproc_owner )
            cproc->step();
    }

    void ExecutionEngine::finalize() {
        //             smproc->finalize();
        //             pproc->finalize();
        if ( cproc_owner )
            cproc->finalize();
    }

    void ExecutionEngine::setCommandProcessor(Processor* cp) {
        if ( cproc_owner )
            delete cproc;
        cproc_owner = false;
        if ( cp == 0 ) {
            cproc = new Processor();
            cproc_owner=true;
        } else
            cproc = cp;
    }

    void ExecutionEngine::setProgramProcessor(Processor* pp) {
        // todo: like above
        pproc = pp;
    }

    void ExecutionEngine::setStateMachineProcessor(Processor* smp) {
        // todo: like above
        smproc = smp;
    }

    ORO_CoreLib::EventProcessor* ExecutionEngine::getEventProcessor() const {
        if ( this->getTask() )
            return this->getTask()->getEventProcessor();
        else
            return ORO_CoreLib::CompletionProcessor::Instance()->getEventProcessor();
    }

}

