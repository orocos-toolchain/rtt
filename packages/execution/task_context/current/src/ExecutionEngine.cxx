
#include <os/Semaphore.hpp>
#include <corelib/CompletionProcessor.hpp>
#include <corelib/Logger.hpp>
#include <execution/ExecutionEngine.hpp>
#include <execution/TaskContext.hpp>
#include <algorithm>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace std;

    ExecutionEngine::ExecutionEngine( TaskContext* owner, ExecutionEngine* parent /* = 0 */ )
        : work_sem( parent ? 0 : new ORO_OS::Semaphore(0)),
          loop_sem( parent ? 0 : new ORO_OS::Semaphore(0)),
          taskc(owner),
          mainee(parent ? parent->getParent() : 0),
          cproc( mainee ? 0 : new CommandProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem) ),
          pproc( mainee ? 0 : new ProgramProcessor(ORONUM_EXECUTION_PROC_QUEUE_SIZE, work_sem) ),
          smproc( mainee ? 0 : new StateMachineProcessor(work_sem) ),
          eproc( mainee ? 0 : new EventProcessor() ),
          eerun( false )
    {
        Logger::In in("ExecutionEngine");
        if (mainee)
            Logger::log() << Logger::Debug << "Creating child "+owner->getName()+" of task "+mainee->getTaskContext()->getName()<<Logger::endl;
        else
            Logger::log() << Logger::Debug << "Creating root task "+owner->getName()<<Logger::endl;
        // add self to parent
        if (mainee)
            mainee->children.push_back(this);
    }
        
    ExecutionEngine::~ExecutionEngine()
    {
        Logger::In in("~ExecutionEngine");
        Logger::log() << Logger::Debug << "Destroying "+taskc->getName()<<Logger::endl;

        // it is possible that the mainee is assigned to a task
        // but that this child EE is already destroyed.
        if (mainee && this->getTask())
            this->RunnableInterface::setTask(0);

        delete cproc;
        delete pproc;
        delete smproc;
        delete eproc;

        delete work_sem;
        delete loop_sem;

        // add children to parent (reparent) and remove self from parent
        if (mainee) {
            std::vector<ExecutionEngine*>::iterator it=find(mainee->children.begin(),mainee->children.end(), this);
            if ( it != mainee->children.end() )
                mainee->children.erase(it);
        }
        for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it != children.end();++it){
            // this operation may make a child orphan.
            (*it)->mainee = mainee;
            if (mainee)
                mainee->children.push_back(*it); // not orphan
            else {
                // orphan, provide it new processors.
                (*it)->cproc = new CommandProcessor();
                (*it)->pproc = new ProgramProcessor();
                (*it)->smproc = new StateMachineProcessor();
                (*it)->eproc = new EventProcessor();
            }
        }
    }

    ExecutionEngine* ExecutionEngine::getParent() {
        return mainee ? mainee : this;
    }

    void ExecutionEngine::reparent(ExecutionEngine* new_parent) {
        if (new_parent == this || new_parent == mainee)
            return;
        // first remove from old parent.
        if (mainee) {
            std::vector<ExecutionEngine*>::iterator it=find(mainee->children.begin(),mainee->children.end(), this);
            if ( it != mainee->children.end() )
                mainee->children.erase(it);
            mainee = 0;
        }
        if (new_parent != 0 ) {
            delete cproc;
            cproc = 0;
            delete pproc;
            pproc = 0;
            delete smproc;
            smproc = 0;
            delete eproc;
            eproc = 0;
            // special order here, first do setTask without disturbing future mainee
            this->setTask( new_parent->getParent()->getTask() );
            mainee = new_parent->getParent();
            mainee->children.push_back(this);
            return;
        }
        if (new_parent == 0) {
            // orphan, provide new processors.
            cproc = new CommandProcessor();
            pproc = new ProgramProcessor();
            smproc = new StateMachineProcessor();
            eproc = new EventProcessor();
            this->setTask(0);
        }
    }

    void ExecutionEngine::setTask(TaskInterface* t)
    {
        Logger::In in("ExecutionEngine::setTask");

        if (mainee) {
            // Let parent update RunnableInterface...
            mainee->setTask(t);
        } else {
            if (t)
                Logger::log() <<Logger::Debug <<taskc->getName()<<": informing processors of new task."<<Logger::endl;
            else
                Logger::log() <<Logger::Debug <<taskc->getName()<<": disconnecting processors of task."<<Logger::endl;
                
            // I am an orphan.
            if (cproc)
                cproc->setTask(t);
            if (pproc)
                pproc->setTask(t);
            if (smproc)
                smproc->setTask(t);
            if (eproc)
                eproc->setTask(t);
            RunnableInterface::setTask(t);
        }
        
        // inform all children, even if we have a parent.
        // attention, avoid recursive endless loop !
        for (std::vector<ExecutionEngine*>::iterator it = children.begin(); it !=children.end();++it)
            (*it)->RunnableInterface::setTask( t );

        if (t)
            if ( ! t->isPeriodic() ) {
                Logger::log() << Logger::Info << taskc->getName()+" is not periodic."<< Logger::endl;
            } else {
                Logger::log() << Logger::Info << taskc->getName()+" is periodic."<< Logger::endl;
            }
        else
            Logger::log() << Logger::Info << taskc->getName()+" is disconnected from its task."<< Logger::endl;
    }

    bool ExecutionEngine::initialize() {
        if ( mainee )
            return true;

        if ( pproc->initialize() ) {
            if ( smproc->initialize() ) {
                if( cproc->initialize() ) {
                    if (eproc->initialize()) {
                        // non periodic loop() uses this flag to detect breakLoop()
                        if ( !this->getTask()->isPeriodic() )
                            eerun = true;
                        return true;
                    }
                    cproc->finalize();
                } 
                smproc->finalize();
            }
            pproc->finalize();
        }
        return false;
    }

    void ExecutionEngine::step() {
        if (mainee)
            return;
        pproc->step();
        smproc->step();
        cproc->step();
        eproc->step();
    }

    void ExecutionEngine::loop() {
        if (mainee)
            return;
        while( eerun ) {
            work_sem->wait();
            this->step();
        }
        loop_sem->signal();
    }

    bool ExecutionEngine::breakLoop() {
        if (mainee)
            return false;
        if (eerun) {
            eerun = false;
            work_sem->signal();
            loop_sem->wait();
            return true;
        }
        return false;
    }

    void ExecutionEngine::finalize() {
        if (mainee)
            return;
        eerun = false;
        pproc->finalize();
        smproc->finalize();
        cproc->finalize();
        eproc->finalize();
    }

    ORO_CoreLib::EventProcessor* ExecutionEngine::events() const {
        return mainee ? mainee->events() : eproc;
    }


    ORO_CoreLib::EventProcessor* ExecutionEngine::getEventProcessor() const {
        return mainee ? mainee->events() : eproc;
    }

}

