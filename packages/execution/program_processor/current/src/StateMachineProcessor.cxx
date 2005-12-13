
#include "execution/StateMachineProcessor.hpp"
#include <corelib/Logger.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/function.hpp>
#include <os/MutexLock.hpp>
#include <os/Semaphore.hpp>
#include <iostream>

namespace ORO_Execution
{

    //using boost::bind;
    using namespace boost::lambda;
    using namespace boost;
    using namespace std;
    using ORO_OS::MutexLock;
    using namespace ORO_CoreLib;


    StateMachineProcessor::StateMachineProcessor(ORO_OS::Semaphore* work_sem)
        : states( new StateMap() ),
          loadmonitor( new ORO_OS::MutexRecursive() ),
          queuesem( work_sem )
    {
    }

    StateMachineProcessor::~StateMachineProcessor()
    {
        // first deactivate (hard way) all state machines :
        for( StateMap::iterator i=states->begin(); i != states->end(); ++i) {
            i->second->deactivate();
            i->second->deactivate();
        }
    
        while ( !states->empty() ) {
            // try to unload all
            try {
                Logger::log() << Logger::Info << "StateMachineProcessor unloads StateMachine "<< states->begin()->first << "..."<<Logger::endl;
                this->unloadStateMachine( states->begin()->first );
            } catch (...) {}
        }
            
        delete states;
        delete loadmonitor;
    }

     StateMachine::Status::StateMachineStatus StateMachineProcessor::getStateMachineStatus(const std::string& name) const
     {
        state_iter it =
            states->find( name );
        if ( it == states->end() )
            return Status::unloaded;
        return it->second->getStatus();
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
        state_iter it =
            states->find( sc->getName() );

        if ( it != states->end() ) {
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
        
        MutexLock lock( *loadmonitor );
        states->insert( make_pair( sc->getName(), sc));
        sc->setStateMachineProcessor(this);
    }

    bool StateMachineProcessor::unloadStateMachine( const std::string& name )
    {
        // this does the same as deleteStateMachine, except for deleting
        // the unloaded context..
        state_iter it =
            states->find( name );

        if ( it != states->end() )
        {
            // test if parent ...
            if ( it->second->getParent() ) {
                std::string error(
                                  "Could not unload StateMachine \"" + it->second->getName() +
                                  "\" with the processor. It is not a root StateMachine." );
                throw program_unload_exception( error );
            }
            recursiveCheckUnloadStateMachine( it->second );
            recursiveUnloadStateMachine( it->second );
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
                state_iter it =
                    states->find( (*it2)->getName() );
                if ( it == states->end() ) {
                    std::string error(
                              "Could not unload StateMachine \"" + si->getName() +
                              "\" with the processor. It contains not loaded children." );
                    throw program_unload_exception( error );
                }
                // all is ok, check child :
                this->recursiveCheckUnloadStateMachine( it->second );
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
        state_iter it2 =
            states->find( sc->getName() );

        assert( it2 != states->end() ); // we checked that this is possible

        MutexLock lock( *loadmonitor );
        states->erase(it2);
        sc->setStateMachineProcessor( 0 );
    }

	bool StateMachineProcessor::deleteStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            {
                if ( it->second->getParent() ) {
                    std::string error(
                                      "Could not unload StateMachine \"" + it->second->getName() +
                                      "\" with the processor. It is not a root StateMachine." );
                    throw program_unload_exception( error );
                }
                // same pre-conditions for delete as for unload :
                recursiveCheckUnloadStateMachine( it->second );
                recursiveUnloadStateMachine( it->second ); // this invalidates it !
                return true;
            }
        return false;
    }

    bool StateMachineProcessor::initialize()
    {
        return true;
    }

    void StateMachineProcessor::finalize()
    {
        // stop all programs and SCs.
        {
            MutexLock lock( *loadmonitor );
            for(state_iter it = states->begin(); it != states->end(); ++it) {
                if ( it->second->isActive() == false)
                    continue;
                if ( it->second->isReactive() )
                    it->second->requestFinalState();
                if ( it->second->isAutomatic() )
                    it->second->stop();
                if (it->second->currentState() != it->second->getFinalState() )
                    it->second->execute(); // try one last time
                if (it->second->currentState() != it->second->getFinalState() )
                    Logger::log() << Logger::Critical << "StateMachineProcessor failed to bring StateMachine "<<it->first
                                  << " into the final state. Program stalled in state '"
                                  << it->second->currentState()->getName()<<"' line number "
                                  << it->second->getLineNumber()<<Logger::endl; // critical failure !
            }
        }
    }

	void StateMachineProcessor::step()
    {
        {
            MutexLock lock( *loadmonitor );
            // Evaluate all states->
            for(state_iter it = states->begin(); it != states->end(); ++it)
                it->second->execute();
        }
    }

    const StateMachinePtr StateMachineProcessor::getStateMachine(const std::string& name) const
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            return it->second;
        return StateMachinePtr();
    }

    StateMachinePtr StateMachineProcessor::getStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            return it->second;
        return StateMachinePtr();
    }

    std::vector<std::string> StateMachineProcessor::getStateMachineList() const
    {
        std::vector<std::string> ret;
        for ( cstate_iter i = states->begin(); i != states->end(); ++i )
            ret.push_back( i->first );
        return ret;
    }
}

