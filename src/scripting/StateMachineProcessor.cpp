/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineProcessor.cxx

                        StateMachineProcessor.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "StateMachineProcessor.hpp"
#include <Logger.hpp>
#include "../internal/Exceptions.hpp"

#include <boost/bind.hpp>

namespace RTT {
    using namespace detail;

    using namespace boost;
    using namespace std;



    StateMachineProcessor::StateMachineProcessor()
        : states( new StateMap(4) )
    {
    }

    StateMachineProcessor::~StateMachineProcessor()
    {
        // first deactivate (hard way) all state machines :
        states->apply( bind( &StateMachine::deactivate, _1));
        states->apply( bind( &StateMachine::deactivate, _1));

        this->clear();

        delete states;
    }

    void StateMachineProcessor::clear() {
        while ( !states->empty() ) {
            // try to unload all
            Logger::log() << Logger::Info << "StateMachineProcessor unloads StateMachine "<< states->front()->getName() << "..."<<Logger::endl;
#ifndef ORO_EMBEDDED
            try {
                this->unloadStateMachine( states->front()->getName() );
            }
            catch ( program_load_exception& ple) {
                Logger::log() << Logger::Error << ple.what() <<Logger::endl;
                states->erase( states->front() ); // plainly remove it to avoid endless loop.
            }
#else
            if (this->unloadStateMachine( states->front()->getName() ) == false) {
                Logger::log() << Logger::Error << "Error during unload !" <<Logger::endl;
                states->erase( states->front() ); // plainly remove it to avoid endless loop.
            }
#endif
        }
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
            ORO_THROW_OR_RETURN( program_load_exception( error ) , false);
        }

        if (this->recursiveCheckLoadStateMachine( sc ) == false)
            return false; // throws load_exception
        this->recursiveLoadStateMachine( sc );
        return true;
    }

    bool StateMachineProcessor::recursiveCheckLoadStateMachine( StateMachinePtr sc )
    {
        // test if already present..., this cannot detect corrupt
        // trees with double names...
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), sc->getName(), bind(&StateMachine::getName, _1)) );

        if ( pip ) {
            std::string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. A StateMachine with that name is already present." );
            ORO_THROW_OR_RETURN( program_load_exception( error ), false );

            std::vector<StateMachinePtr>::const_iterator it2;
            for (it2 = sc->getChildren().begin(); it2 != sc->getChildren().end(); ++it2)
                {
                    if ( this->recursiveCheckLoadStateMachine( *it2 ) == false)
                        return false;
                }
        }
        return true;
    }

    void StateMachineProcessor::recursiveLoadStateMachine( StateMachinePtr sc )
    {
        std::vector<StateMachinePtr>::const_iterator it;

        // first load parent.
        states->grow();
        states->append(sc);
        sc->setStateMachineProcessor(this);

        // then load children.
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateMachine( *it );
            }

    }

    bool StateMachineProcessor::unloadStateMachine( const std::string& name )
    {
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), name, bind(&StateMachine::getName, _1)) );

        if ( pip ) {
            // test if parent ...
            if ( pip->getParent() ) {
                std::string error(
                                  "Could not unload StateMachine \"" + pip->getName() +
                                  "\" with the processor. It is not a root StateMachine." );
                ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
            }
            if (recursiveCheckUnloadStateMachine( pip ) == false)
                return false;
            recursiveUnloadStateMachine( pip );
            return true;
        }
        return false;
    }

    bool StateMachineProcessor::recursiveCheckUnloadStateMachine(StateMachinePtr si)
    {
        // check this state
        if ( si->isActive() ) {
            std::string error(
                              "Could not unload StateMachine \"" + si->getName() +
                              "\" with the processor. It is still active, status is "+
                    this->getStateMachineStatusStr( si->getName() ) );
            ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
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
                    ORO_THROW_OR_RETURN( program_unload_exception( error ), false);
                }
                // all is ok, check child :
                if ( this->recursiveCheckUnloadStateMachine( pip ) == false)
                    return false;
            }
        return true;
    }

    void StateMachineProcessor::recursiveUnloadStateMachine(StateMachinePtr sc) {
        std::vector<StateMachinePtr>::const_iterator it;

        // first erase children
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveUnloadStateMachine( *it );
            }

        // erase this sc :
        StateMachinePtr pip = states->find_if( bind(equal_to<string>(), sc->getName(), bind(&StateMachine::getName, _1)) );

        assert( pip ); // we checked that this is possible

        // lastly, unload the parent.
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
        states->apply( bind( &vector<string>::push_back, ref(sret), bind( &StateMachine::getName, _1) ) );
        return sret;
    }
}

