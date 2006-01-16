
#include "execution/StateMachineProcessor.hpp"
#include <corelib/Logger.hpp>

#include <boost/bind.hpp>
#include <os/Semaphore.hpp>
#include <iostream>

namespace ORO_Execution
{

    using namespace boost;
    using namespace std;
    using namespace ORO_CoreLib;


    StateMachineProcessor::StateMachineProcessor(ORO_OS::Semaphore* work_sem)
        : states( new StateMap(4) ),
          queuesem( work_sem )
    {
    }

    StateMachineProcessor::~StateMachineProcessor()
    {
        // first deactivate (hard way) all state machines :
        states->apply( bind( &StateMachine::deactivate, _1));
        states->apply( bind( &StateMachine::deactivate, _1));
    
        while ( !states->empty() ) {
            // try to unload all
            try {
                Logger::log() << Logger::Info << "StateMachineProcessor unloads StateMachine "<< states->front()->getName() << "..."<<Logger::endl;
                this->unloadStateMachine( states->front()->getName() );
            } catch (...) {}
        }
            
        delete states;
    }

     StateMachine::Status::StateMachineStatus StateMachineProcessor::getStateMachineStatus(const std::string& name) const
     {
         StateMachinePtr pip = states->find_if( bind(equal_to<string>(), name, bind(&StateMachine::getName, _1)) );
         if ( pip )
             return pip->getStatus();
         return Status::unloaded;
     }

     std::string StateMachineProcessor::getStateMachineStatusStr(const std::string& name) const
     {
        switch ( getStateMachineStatus( name ))
            {
            case Status::inactive:
                return "inactive";
                break;
            case Status::stopping:
                return "stopping";
                break;
            case Status::stopped:
                return "stopped";
                break;
            case Status::requesting:
                return "requesting";
                break;
            case Status::running:
                return "running";
                break;
            case Status::paused:
                return "paused";
                break;
            case Status::active:
                return "active";
                break;
            case Status::activating:
                return "activating";
                break;
            case Status::deactivating:
                return "deactivating";
                break;
            case Status::resetting:
                return "resetting";
                break;
            case Status::error:
                return "error";
                break;
            case Status::unloaded:
                return "unloaded";
                break;
            }
        return "na";
     }

	bool StateMachineProcessor::loadStateMachine( StateMachinePtr sc )
    {
        // test if parent ...
        if ( sc->getParent() ) {
            std::string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. It is not a root StateMachine." );
            throw program_load_exception( error );
        }

        this->recursiveCheckLoadStateMachine( sc ); // throws load_exception
        this->recursiveLoadStateMachine( sc );
        return true;
    }
    
    void StateMachineProcessor::recursiveCheckLoadStateMachine( StateMachinePtr sc )
    {
        // test if already present..., this cannot detect corrupt
        // trees with double names...
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), sc->getName(), bind(&StateMachine::getName, _1)) );

        if ( pip ) {
            std::string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. A StateMachine with that name is already present." );
            throw program_load_exception( error );

            std::vector<StateMachinePtr>::const_iterator it2;
            for (it2 = sc->getChildren().begin(); it2 != sc->getChildren().end(); ++it2)
                {
                    this->recursiveCheckLoadStateMachine( *it2 );
                }
        }
    }

    void StateMachineProcessor::recursiveLoadStateMachine( StateMachinePtr sc )
    {
        std::vector<StateMachinePtr>::const_iterator it;
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateMachine( *it );
            }
        
        states->grow();
        states->append(sc);
        sc->setStateMachineProcessor(this);
    }

    bool StateMachineProcessor::unloadStateMachine( const std::string& name )
    {
        // this does the same as deleteStateMachine, except for deleting
        // the unloaded context..
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), name, bind(&StateMachine::getName, _1)) );

        if ( pip ) {
            // test if parent ...
            if ( pip->getParent() ) {
                std::string error(
                                  "Could not unload StateMachine \"" + pip->getName() +
                                  "\" with the processor. It is not a root StateMachine." );
                throw program_unload_exception( error );
            }
            recursiveCheckUnloadStateMachine( pip );
            recursiveUnloadStateMachine( pip );
            return true;
        }
        return false;
    }

    void StateMachineProcessor::recursiveCheckUnloadStateMachine(StateMachinePtr si)
    {
        // check this state
        if ( si->isActive() ) {
            std::string error(
                              "Could not unload StateMachine \"" + si->getName() +
                              "\" with the processor. It is still active, status is "+
                    this->getStateMachineStatusStr( si->getName() ) );
            throw program_unload_exception( error );
        }

        // check children
        std::vector<StateMachinePtr>::const_iterator it2;
        for (it2 = si->getChildren().begin();
             it2 != si->getChildren().end();
             ++it2)
            {
                StateMachinePtr pip = states->find_if( bind(equal_to<string>(), (*it2)->getName(), bind(&StateMachine::getName, _1)) );
                if ( !pip ) {
                    std::string error(
                              "Could not unload StateMachine \"" + si->getName() +
                              "\" with the processor. It contains not loaded child "+ (*it2)->getName() );
                    throw program_unload_exception( error );
                }
                // all is ok, check child :
                this->recursiveCheckUnloadStateMachine( pip );
            }
    }

    void StateMachineProcessor::recursiveUnloadStateMachine(StateMachinePtr sc) {
        std::vector<StateMachinePtr>::const_iterator it;
        // erase children
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveUnloadStateMachine( *it );
            }
        
        // erase this sc :
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), sc->getName(), bind(&StateMachine::getName, _1)) );

        assert( pip ); // we checked that this is possible

        states->erase(pip);
        states->shrink();
        sc->setStateMachineProcessor( 0 );
    }

	bool StateMachineProcessor::deleteStateMachine(const std::string& name)
    {
        return this->unloadStateMachine(name);
    }

    bool StateMachineProcessor::initialize()
    {
        return true;
    }

    static void shutdown_hard(StateMachinePtr sc)  {
        if ( sc->isActive() == false)
            return;
        if ( sc->isReactive() )
            sc->requestFinalState();
        if ( sc->isAutomatic() )
            sc->stop();
        if (sc->currentState() != sc->getFinalState() )
            sc->execute(); // try one last time
        if (sc->currentState() != sc->getFinalState() )
            Logger::log() << Logger::Critical << "StateMachineProcessor failed to bring StateMachine "<< sc->getName()
                          << " into the final state. Program stalled in state '"
                          << sc->currentState()->getName()<<"' line number "
                          << sc->getLineNumber()<<Logger::endl; // critical failure !
    }

    void StateMachineProcessor::finalize()
    {
        // stop all SCs the 'hard' way.
        states->apply(bind(&shutdown_hard, _1));
    }

	void StateMachineProcessor::step()
    {
        // Evaluate all states->
        states->apply(bind(&StateMachine::execute, _1));
    }

    const StateMachinePtr StateMachineProcessor::getStateMachine(const std::string& name) const
    {
        return states->find_if(bind(equal_to<string>(), name, bind(&StateMachine::getName, _1)));
    }

    StateMachinePtr StateMachineProcessor::getStateMachine(const std::string& name)
    {
        return states->find_if(bind(equal_to<string>(), name, bind(&StateMachine::getName, _1)));
    }

    std::vector<std::string> StateMachineProcessor::getStateMachineList() const
    {
        std::vector<string> sret;
        states->apply( bind( &vector<string>::push_back, sret, bind( &StateMachine::getName, _1) ) );
        return sret;
    }
}

