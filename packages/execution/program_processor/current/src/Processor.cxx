/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  Processor.cxx

                        Processor.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "execution/Processor.hpp"
#include "execution/ProgramInterface.hpp"
#include "execution/StateMachine.hpp"
#include <corelib/CommandInterface.hpp>
#include <corelib/AtomicQueue.hpp>
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

        struct Processor::ProgramInfo
        {
            Processor::ProgramStatus::status pstate;
            ProgramInfo( ProgramInterface* p)
                : pstate(ProgramStatus::stopped), program(p),
                  step(false) {}
            ProgramInterface* program;
            bool step;
        };

        struct Processor::StateInfo
        {
            Processor::StateMachineStatus::status sstate; // 'global' state of this StateMachine
            StateInfo( StateMachine* s)
                : sstate( StateMachineStatus::inactive ),
                  state(s),
                  action(0),
                  stepping(true)
            {}

            StateMachine* state;
            //boost::function<void(void)> action; // does do alloc
            typedef void (Processor::StateInfo::*ActionPtr)(void); // ptr to member
            ActionPtr action;
            //Fooptr fooptr = &X::foo;        // no alloc
            //i = (x2.*fooptr)(2);            // no alloc

            // activate may be called directly
            bool activate() {
                if ( state->activate() == false )
                    return false;
                if ( state->inTransition() ) {
                    action = &StateInfo::goactive;
                    sstate = StateMachineStatus::activating;
                } else {
                    // wait for start() or requestState()
                    action = 0;
                    sstate = StateMachineStatus::active;
                }
                return true;
            }
            void deactivate() {
                state->deactivate();
                if ( state->inTransition() ) {
                    action = &StateInfo::goinactive;
                    sstate = StateMachineStatus::deactivating;
                } else {
                    action = 0;
                    sstate = StateMachineStatus::inactive;
                }
            }

            void start() {
                sstate = StateMachineStatus::running;
                // keep repeating the run action
                action = &StateInfo::run;
                this->run(); // execute the first time from here.
            }

            void pause() {
                action = 0;
                sstate = StateMachineStatus::paused;
            }

            void stop() {
                if ( state->executePending() || state->inError() ) {
                    state->requestFinalState();
                    // test for atomicity :
                    if ( state->inTransition() ) {
                        action = &StateInfo::gostop;
                        sstate = StateMachineStatus::stopping;
                    } else {
                        action = 0;
                        sstate = StateMachineStatus::stopped;
                    }
                }
            }
            void reset() {
                if ( state->executePending() ) {
                    state->requestInitialState();
                    if ( state->inTransition() ) {
                        action = &StateInfo::goactive;
                        sstate = StateMachineStatus::resetting;
                    } else {
                        action = 0;
                        sstate = StateMachineStatus::active;
                    }
                } else {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                }
            }

            void requestState() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }
                // if we got here, all pending programs are done, wait for next command.
                sstate = StateMachineStatus::active;
                action = 0;
            }

            void singleStep() {
                if ( state->executePending(true) )    // if all steps done,
                    state->requestNextState(true); // one state at a time
                else {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                }
                action = 0; // unset self.
            }

            // make from this state a single transition to the next state (or handle() )
            void singleTransition() {
                if ( state->executePending() ) {   // if all steps done,
                    state->requestNextState();
                    action = &StateInfo::requestState; // handle remainder in requestState
                }
                else {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                }
                // if pending not done, retry next time.
            }

            bool stepping;
        protected:
            // Go through the entry of the initial state:
            void goactive() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }
                sstate = StateMachineStatus::active;
                action = 0;
            }
            // Go through the exit of the current state:
            void goinactive() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }
                sstate = StateMachineStatus::inactive;
                action = 0;
            }
            // Go through exit of former and entry of final state:
            void gostop() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }
                sstate = StateMachineStatus::stopped;
                action = 0;
            }
            void run() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }

                if (stepping)
                    state->requestNextState(); // one state at a time
                else {
                    StateInterface* cur_s = state->currentState();
                    while ( cur_s != state->requestNextState() )  { // go until no transition found.
                        cur_s = state->currentState();
                    }
                }
            }
            void request() {
                if ( state->executePending() == false ) {
                    if ( state->inError() )
                        sstate = StateMachineStatus::error;
                    return;
                }

            }
        };


    Processor::Processor(int queue_size)
        : programs( new ProgMap() ),
          states( new StateMap() ),
          funcs( queue_size ),
         accept(false), a_queue( new ORO_CoreLib::AtomicQueue<CommandInterface*>(queue_size) ),
         f_queue( new ORO_CoreLib::AtomicQueue<ProgramInterface*>(queue_size) ),
         coms_processed(0),
         loadmonitor( new ORO_OS::MutexRecursive() ),
         queuesem( new ORO_OS::Semaphore(0) ),
          doloop(false), loopmode(false)
    {
    }

    Processor::~Processor()
    {
        while ( !programs->empty() ) {
            Logger::log() << Logger::Info << "Processor deletes Program "<< programs->begin()->first << "..."<<Logger::endl;
            programs->begin()->second.pstate = ProgramStatus::stopped; // guarantees deletion :
            this->deleteProgram( programs->begin()->first );
        }

        // first deactivate (hard way) all state machines :
        for( StateMap::iterator i=states->begin(); i != states->end(); ++i) {
            i->second.deactivate();
            i->second.sstate = StateMachineStatus::inactive;
        }
    
        while ( !states->empty() ) {
            // try to unload all
            try {
                Logger::log() << Logger::Info << "Processor deletes StateMachine "<< states->begin()->first << "..."<<Logger::endl;
                this->deleteStateMachine( states->begin()->first );
            } catch (...) {}
        }
            
        delete programs;
        delete states;
        delete a_queue;
        delete f_queue;
        delete loadmonitor;
        delete queuesem;
    }

     Processor::ProgramStatus::status Processor::getProgramStatus(const std::string& name) const
     {
        program_iter it =
            programs->find( name );
        if ( it == programs->end() )
            return ProgramStatus::unloaded;
        return it->second.pstate;
     }

     std::string Processor::getProgramStatusStr(const std::string& name) const
     {
        switch ( getProgramStatus( name ))
            {
            case Processor::ProgramStatus::unloaded:
                return "unloaded";
                break;
            case Processor::ProgramStatus::stopped:
                return "stopped";
                break;
            case Processor::ProgramStatus::running:
                return "running";
                break;
            case Processor::ProgramStatus::stepmode:
                return "paused";
                break;
            case Processor::ProgramStatus::error:
                return "error";
                break;
            }
        return "na";
     }

     Processor::StateMachineStatus::status Processor::getStateMachineStatus(const std::string& name) const
     {
        state_iter it =
            states->find( name );
        if ( it == states->end() )
            return StateMachineStatus::unloaded;
        return it->second.sstate;
     }

     std::string Processor::getStateMachineStatusStr(const std::string& name) const
     {
        switch ( getStateMachineStatus( name ))
            {
            case Processor::StateMachineStatus::unloaded:
                return "unloaded";
                break;
            case Processor::StateMachineStatus::inactive:
                return "inactive";
                break;
            case Processor::StateMachineStatus::stopping:
                return "stopping";
                break;
            case Processor::StateMachineStatus::stopped:
                return "stopped";
                break;
            case Processor::StateMachineStatus::running:
                return "running";
                break;
            case Processor::StateMachineStatus::paused:
                return "paused";
                break;
            case Processor::StateMachineStatus::active:
                return "active";
                break;
            case Processor::StateMachineStatus::activating:
                return "activating";
                break;
            case Processor::StateMachineStatus::deactivating:
                return "deactivating";
                break;
            case Processor::StateMachineStatus::resetting:
                return "resetting";
                break;
            case Processor::StateMachineStatus::error:
                return "error";
                break;
            }
        return "na";
     }


	bool Processor::loadProgram(ProgramInterface* pi)
    {
        program_iter it =
            programs->find( pi->getName() );
        if ( it != programs->end() )
            return false;
        MutexLock lock( *loadmonitor );
        programs->insert( make_pair( pi->getName(), Processor::ProgramInfo(pi) ) );
        pi->reset();
        return true;
    }

	bool Processor::startProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() ) {
            if ( it->second.program->isFinished() )
                it->second.program->reset();       // if the program was finished, reset it.
            it->second.pstate = ProgramStatus::running;
            // so it is possible to start a program from the error state
        }
        return it != programs->end();
    }

	bool Processor::isProgramRunning(const std::string& name) const
    {
        cprogram_iter it =
            programs->find( name );
        if ( it != programs->end() )
            return it->second.pstate == ProgramStatus::running;
        return false;
    }

	bool Processor::pauseProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() ) {
            if ( it->second.program->isFinished() )
                it->second.program->reset();       // if the program was finished, reset it.
            it->second.pstate = ProgramStatus::stepmode;
        }
        // it is possible to pause a program from the error state
        return it != programs->end();
    }

	bool Processor::stopProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() ) {
            it->second.pstate = ProgramStatus::stopped;
            it->second.program->reset();
        }
        return it != programs->end();
    }

	bool Processor::deleteProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() && it->second.pstate == ProgramStatus::stopped )
            {
                MutexLock lock( *loadmonitor );
                delete it->second.program;
                programs->erase(it);
                return true;
            }
        if ( it == programs->end() ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It does not exist." );
                throw program_unload_exception( error );
            }
        if ( it->second.pstate != ProgramStatus::stopped ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It is still running." );
                throw program_unload_exception( error );
            }
        return false;
    }

	bool Processor::loadStateMachine( StateMachine* sc )
    {
        // test if parent ...
        if ( sc->getParent() != 0 ) {
            std::string error(
                "Could not register StateMachine \"" + sc->getName() +
                "\" with the processor. It is not a root StateMachine." );
            throw program_load_exception( error );
        }

        this->recursiveCheckLoadStateMachine( sc ); // throws load_exception
        this->recursiveLoadStateMachine( sc );
        return true;
    }
    
    void Processor::recursiveCheckLoadStateMachine( StateMachine* sc )
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

            std::vector<StateMachine*>::const_iterator it2;
            for (it2 = sc->getChildren().begin(); it2 != sc->getChildren().end(); ++it2)
                {
                    this->recursiveCheckLoadStateMachine( *it2 );
                }
        }
    }

    void Processor::recursiveLoadStateMachine( StateMachine* sc )
    {
        std::vector<StateMachine*>::const_iterator it;
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateMachine( *it );
            }
        
        MutexLock lock( *loadmonitor );
        states->insert( make_pair( sc->getName(), Processor::StateInfo(sc)));
    }

	bool Processor::startStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() && it->second.sstate == StateMachineStatus::active || it->second.sstate == StateMachineStatus::paused) {
            it->second.action = &StateInfo::start;
            return true;
        }
        return false;
    }

	bool Processor::stepStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );
        if ( it == states->end() )
            return false;

        // if paused or active, step single instruction/transition or run one state transition/handle program
        if ( it->second.sstate == StateMachineStatus::paused ) {
            it->second.action = &StateInfo::singleStep;
            return true;
        }
        if ( it->second.sstate == StateMachineStatus::active ) {
            it->second.action = &StateInfo::singleTransition;
            return true;
        }
        return false;
    }

	bool Processor::requestModeStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        // only go to request mode from an active state machine.
        if ( it != states->end() && it->second.state->isActive() && it->second.sstate != StateMachineStatus::error )
            {
                // go into run status to reach the transition evaluation point
                it->second.sstate = StateMachineStatus::running;
                it->second.action = &StateInfo::requestState;
                it->second.stepping = true;
                return true;
            }
        return false;
    }

    bool Processor::requestStateStateMachine(const std::string& name, const std::string& state)
    {
        state_iter it =
            states->find( name );

        // only go to request mode from an active (thus not running,paused etc) state machine.
        if ( it != states->end() && it->second.sstate == StateMachineStatus::active )
            {
                // request the SM the transition :
                StateInterface* target = it->second.state->getState(state);
                if ( target == 0 || it->second.state->requestState( target ) ==  false )
                    return false;
                // go into run status to reach the next transition evaluation point
                it->second.sstate = StateMachineStatus::running;
                it->second.action = &StateInfo::requestState;
                it->second.stepping = true;
                return true;
            }
        return false;
    }

	bool Processor::steppedModeStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            {
                it->second.stepping = true;
            }
        return it != states->end();
    }

	bool Processor::continuousModeStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            {
                it->second.stepping = false;
            }
        return it != states->end();
    }

	bool Processor::isStateMachineRunning(const std::string& name) const
    {
        cstate_iter it =
            states->find( name );

        if ( it != states->end() )
            return it->second.sstate == StateMachineStatus::running;
        return false;
    }

	bool Processor::isStateMachineStepped(const std::string& name) const
    {
        cstate_iter it =
            states->find( name );

        if ( it != states->end() )
            return it->second.stepping;
        return false;
    }

    bool Processor::activateStateMachine( const std::string& name )
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() && it->second.sstate == StateMachineStatus::inactive )
            {
                return it->second.activate();
            }
        return false;
    }

    bool Processor::deactivateStateMachine( const std::string& name )
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            {
                it->second.deactivate();
                return true;
            }
        return false;
    }

    bool Processor::pauseStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() && ( it->second.sstate == StateMachineStatus::running
                                      || it->second.sstate == StateMachineStatus::paused
                                      || it->second.sstate == StateMachineStatus::active))
            {
                it->second.action = &StateInfo::pause;
                return true;
            }
        return false;
    }

	bool Processor::stopStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() && ( it->second.sstate != StateMachineStatus::inactive) )
            {
                it->second.action = &StateInfo::stop;
                return true;
            }
        return false;
    }

	bool Processor::resetStateMachine(const std::string& name)
    {
        // We can only reset when stopped.
        state_iter it =
            states->find( name );

        if ( it != states->end() && it->second.sstate == StateMachineStatus::stopped )
            {
                it->second.action = &StateInfo::reset;
                return true;
            }
        return false;
    }

    bool Processor::unloadStateMachine( const std::string& name )
    {
        // this does the same as deleteStateMachine, except for deleting
        // the unloaded context..
        state_iter it =
            states->find( name );

        if ( it != states->end() )
        {
            // test if parent ...
            if ( it->second.state->getParent() != 0 ) {
                std::string error(
                                  "Could not unload StateMachine \"" + it->second.state->getName() +
                                  "\" with the processor. It is not a root StateMachine." );
                throw program_unload_exception( error );
            }
            recursiveCheckUnloadStateMachine( it->second );
            recursiveUnloadStateMachine( it->second.state );
            return true;
        }
        return false;
    }

    void Processor::recursiveCheckUnloadStateMachine(const StateInfo& si)
    {
        // check this state
        if ( si.sstate != StateMachineStatus::inactive ) {
            std::string error(
                              "Could not unload StateMachine \"" + si.state->getName() +
                              "\" with the processor. It is still active." );
            throw program_unload_exception( error );
        }

        // check children
        std::vector<StateMachine*>::const_iterator it2;
        for (it2 = si.state->getChildren().begin();
             it2 != si.state->getChildren().end();
             ++it2)
            {
                state_iter it =
                    states->find( (*it2)->getName() );
                if ( it == states->end() ) {
                    std::string error(
                              "Could not unload StateMachine \"" + si.state->getName() +
                              "\" with the processor. It contains not loaded children." );
                    throw program_unload_exception( error );
                }
                // all is ok, check child :
                this->recursiveCheckUnloadStateMachine( it->second );
            }
    }

    void Processor::recursiveUnloadStateMachine(StateMachine* sc) {
        std::vector<StateMachine*>::const_iterator it;
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
    }

	bool Processor::deleteStateMachine(const std::string& name)
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            {
                if ( it->second.state->getParent() != 0 ) {
                    std::string error(
                                      "Could not unload StateMachine \"" + it->second.state->getName() +
                                      "\" with the processor. It is not a root StateMachine." );
                    throw program_unload_exception( error );
                }
                StateMachine* todelete = it->second.state;
                // same pre-conditions for delete as for unload :
                recursiveCheckUnloadStateMachine( it->second );
                recursiveUnloadStateMachine( it->second.state ); // this invalidates it !
                delete todelete;
                return true;
            }
        return false;
    }

    struct _executeState
        : public std::unary_function<void, std::pair<std::string,Processor::StateInfo> >
    {
        void operator()( std::pair<const std::string,Processor::StateInfo>& s ) {
            if ( s.second.action )
                (s.second.*(s.second.action))();
        }
    };

    struct _stopState
        : public std::unary_function<void, std::pair<std::string,Processor::StateInfo> >
    {
        void operator()( std::pair<const std::string,Processor::StateInfo>& s )
        {
            if ( s.second.sstate == Processor::StateMachineStatus::paused
                 || s.second.sstate == Processor::StateMachineStatus::active
                 || s.second.sstate == Processor::StateMachineStatus::running ) {
                s.second.stop();
                if (s.second.sstate != Processor::StateMachineStatus::stopped )
                    _executeState()( s ); // try one last time
                if (s.second.sstate != Processor::StateMachineStatus::stopped )
                    Logger::log() << Logger::Critical << "Processor failed to bring StateMachine "<<s.first
                                  << " into the final state. Program stalled in state '"
                                  << s.second.state->currentState()->getName()<<"' line number "
                                  << s.second.state->getLineNumber()<<Logger::endl; // critical failure !
            }
        }
    };

    struct _executeProgram
        : public std::unary_function<void, std::pair<std::string,Processor::ProgramInfo> >
    {
        void operator()( std::pair<const std::string,Processor::ProgramInfo>& p )
        {
            if (p.second.pstate == Processor::ProgramStatus::running) {
                if ( p.second.program->executeUntil() == false )
                    p.second.pstate = Processor::ProgramStatus::error;
                if ( p.second.program->isFinished() ) {
                    p.second.pstate = Processor::ProgramStatus::stopped;
                }
            }
        }
    };

    struct _stopProgram
        : public std::unary_function<void, std::pair<std::string,Processor::ProgramInfo> >
    {
        void operator()( std::pair<const std::string,Processor::ProgramInfo>& p )
        {
            if (p.second.pstate != Processor::ProgramStatus::stopped) {
                p.second.pstate = Processor::ProgramStatus::stopped;
                p.second.program->reset();
            }
        }
    };

    struct _stepProgram
        : public std::unary_function<void, std::pair<std::string,Processor::ProgramInfo>& >
    {
        void operator()( std::pair<const std::string,Processor::ProgramInfo>& p )
        {
            if (p.second.pstate == Processor::ProgramStatus::stepmode && p.second.step) {
                if ( p.second.program->executeStep() == false )
                    p.second.pstate = Processor::ProgramStatus::error;
                if ( p.second.program->isFinished() ) {
                    p.second.pstate = Processor::ProgramStatus::stopped;
                }
                p.second.step = false;
            }
        }
    };

    using ORO_CoreLib::Logger;

    void Processor::loop()
    {
        while ( doloop )
            {
                queuesem->wait();
                // execute step with semaphore management.
                this->step();
            }
    }

    bool Processor::breakLoop()
    {
        if (doloop) {
            doloop = false;
            queuesem->signal();
        }
        return true;
    }

    void Processor::resumeLoop()
    {
        if ( loopmode )
            queuesem->signal();
    }


    bool Processor::initialize()
    {
        a_queue->clear();
        f_queue->clear();
        accept = true;
        // doloop is set here and not in loop() to avoid race condition with breakLoop()
        // if not periodic, loop() will be called.
        doloop = !this->getTask()->isPeriodic(); 
        loopmode = doloop;
        return true;
    }

    void Processor::finalize()
    {
        accept = false;
        doloop = false;
        loopmode = false;
        // stop all programs and SCs.
        {
            MutexLock lock( *loadmonitor );
            for_each(programs->begin(), programs->end(), _stopProgram());
            // enter final state, this is the safest bet if a SM is still running.
            for_each(states->begin(), states->end(), _stopState() ); 
        }
    }

	void Processor::step()
    {
        {
            MutexLock lock( *loadmonitor );
            // Evaluate all states->
            for_each(states->begin(), states->end(), _executeState() );

            //Execute all normal programs->
            for_each(programs->begin(), programs->end(), _executeProgram() );

            //Execute all programs in Stepping mode.
            for_each(programs->begin(), programs->end(), _stepProgram() );
        }

        // Execute any FunctionGraph :
        {
            // 0. find an empty spot to store :
            ProgramInterface* foo = 0;
            std::vector<ProgramInterface*>::iterator f_it = find(funcs.begin(), funcs.end(), foo );
            // 1. Fetch new ones from queue.
            while ( !f_queue->isEmpty() && f_it != funcs.end() ) {
                f_queue->dequeue( foo );
                // decrement semaphore if looping, but be sure not to block in any case
                if ( loopmode )
                    queuesem->trywait();
                *f_it = foo;
                // stored successfully, now reset for next item from queue.
                foo = 0;
                f_it = find(f_it, funcs.end(), foo );
            }
            // 2. execute all present
            for(std::vector<ProgramInterface*>::iterator it = funcs.begin();
                it != funcs.end(); ++it )
                if ( *it )
                    if ( (*it)->isFinished() || (*it)->inError() )
                        (*it) = 0;
                    else
                        (*it)->executeUntil();
        }

        // Execute any additional (deferred/external) command.
        {
            // execute one command from the AtomicQueue.
            CommandInterface* com;
            int res = a_queue->dequeueCounted( com );
            if ( res ) {
                // decrement semaphore if looping, but be sure not to block in any case
                if ( loopmode )
                    queuesem->trywait();
                // note : the command's result is discarded.
                // Wrap your command (ie CommandDispatch) to keep track
                // of the result of enqueued commands.
                com->execute();
                // let the user know this command was processed.
                coms_processed = res;
            }
        }
    }


    bool Processor::stepProgram(const std::string& name)
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() )
            it->second.step = true;
        return it != programs->end();
    }

    int Processor::process( CommandInterface* c )
    {
        if (accept && c) {
            int result = a_queue->enqueueCounted( c );
            if ( loopmode )
                queuesem->signal();
            return result;
        }
        return 0;
    }

    bool Processor::runFunction( ProgramInterface* f )
    {
        if (accept && f) {
            int result = f_queue->enqueue( f );
            if ( loopmode )
                queuesem->signal();
            return result != 0;
        }
        return false;
    }

    bool Processor::isFunctionFinished( ProgramInterface* f ) const
    {
        if ( f == 0 )
            return false;
        // function is finished if it is no longer in our map.
        // funcs is fixed size, so iteration is always safe.
        return std::find( funcs.begin(), funcs.end(), f ) == funcs.end();
    }

    std::vector<std::string> Processor::getProgramList() const
    {
        std::vector<std::string> ret;
        for ( cprogram_iter i = programs->begin(); i != programs->end(); ++i )
            ret.push_back( i->first );
        return ret;
    }

    const ProgramInterface* Processor::getProgram(const std::string& name) const
    {
        program_iter it =
            programs->find( name );

        if ( it != programs->end() )
            return it->second.program;
        return 0;
    }

    const StateMachine* Processor::getStateMachine(const std::string& name) const
    {
        state_iter it =
            states->find( name );

        if ( it != states->end() )
            return it->second.state;
        return 0;
    }

    std::vector<std::string> Processor::getStateMachineList() const
    {
        std::vector<std::string> ret;
        for ( cstate_iter i = states->begin(); i != states->end(); ++i )
            ret.push_back( i->first );
        return ret;
    }

    bool Processor::isProcessed( int nr ) const
    {
        return ( nr <= coms_processed );
    }
}

