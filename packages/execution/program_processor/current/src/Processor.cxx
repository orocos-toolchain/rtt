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
#include "execution/StateContextTree.hpp"
#include <corelib/CommandInterface.hpp>
#include <corelib/AtomicQueue.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <os/MutexLock.hpp>
#include <iostream>

namespace ORO_Execution
{

    using boost::bind;
    using namespace std;
    using ORO_OS::MutexLock;

        struct Processor::ProgramInfo
        {
            Processor::ProgramStatus::status pstate;
            ProgramInfo(const std::string&_name, ProgramInterface* p)
                : pstate(ProgramStatus::stopped), program(p),
                  step(false), name(_name) {}
            ProgramInterface* program;
            bool step;
            std::string name;
        };

        struct Processor::StateInfo
        {
            Processor::StateContextStatus::status sstate; // 'global' state of this StateContext
            StateInfo(const std::string& _name, StateContextTree* s)
                : sstate( StateContextStatus::inactive ),
                  state(s),
                  action(0),
                  stepping(true),
                  name(_name)
            {}

            StateContextTree* state;
            boost::function<void(void)> action; // set action to zero to 'pause'
            //void (StateInfo::action*)(void);

            // (de)activate may be called directly
            void activate() {
                state->activate();
                action = 0;
                sstate = StateContextStatus::active;
            }
            void deactivate() {
                state->deactivate();
                action = 0;
                sstate = StateContextStatus::inactive;
            }

            // start, stop , pause and reset work with the
            // action functor.
            void start() {
                sstate = StateContextStatus::running;
                // keep repeating the run action
                action = bind (&StateInfo::run, this);
                //action = &StateInfo::run;
                this->run(); // execute the first time from here.
            }

            void pause() {
                action = 0;
                sstate = StateContextStatus::paused;
            }

            void stop() {
                action = 0;
                state->requestFinalState();
                    sstate = StateContextStatus::stopped;
            }
            void reset() {
                action = 0;
                state->requestInitialState();
                    sstate = StateContextStatus::active;
            }

            void singleStep() {
                state->requestNextState(); // one state at a time
                action = 0; // unset self.
            }

            bool stepping;
            std::string name;
        protected:
            void run() {
                if (stepping)
                    state->requestNextState(); // one state at a time
                else {
                    ORO_CoreLib::StateInterface* cur_s = state->currentState();
                    while ( cur_s != state->requestNextState() ) // go until no transition found.
                        cur_s = state->currentState();
                }
            }
            //StateInfo( const StateInfo& ) {}
        };


    Processor::Processor(int queue_size)
        :funcs( queue_size ),
         f_it( funcs.begin() ),
        accept(false), a_queue( new ORO_CoreLib::AtomicQueue<CommandInterface*>(queue_size) ),
         f_queue( new ORO_CoreLib::AtomicQueue<ProgramInterface*>(queue_size) ),
         coms_processed(0)
    {
        programs = new list<ProgramInfo>();
        states   = new list<StateInfo>();
    }

    Processor::~Processor()
    {
        delete programs;
        delete states;
        delete a_queue;
    }

    bool program_lookup( const Processor::ProgramInfo& pi, const std::string& name)
    {
        return (pi.name == name);
    }

    bool state_lookup( const Processor::StateInfo& si, const std::string& name)
    {
        return (si.name == name);
    }

     Processor::ProgramStatus::status Processor::getProgramStatus(const std::string& name) const
     {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name ) );
        if ( it == programs->end() )
            return ProgramStatus::unloaded;
        return it->pstate;
     }

     Processor::StateContextStatus::status Processor::getStateContextStatus(const std::string& name) const
     {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it == states->end() )
            return StateContextStatus::unloaded;
        return it->sstate;
     }


	bool Processor::loadProgram(ProgramInterface* pi)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, pi->getName() ) );
        if ( it != programs->end() )
            return false;
        MutexLock lock( progmonitor );
        programs->push_back( Processor::ProgramInfo(pi->getName(), pi) );
        pi->reset();
        return true;
    }

	bool Processor::startProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() ) {
            if ( it->program->isFinished() )
                it->program->reset();       // if the program was finished, reset it.
            it->pstate = ProgramStatus::running;
            // so it is possible to start a program from the error state
        }
        return it != programs->end();
    }

	bool Processor::isProgramRunning(const std::string& name) const
    {
        cprogram_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() )
            return it->pstate == ProgramStatus::running;
        return false;
    }

	bool Processor::pauseProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() )
            it->pstate = ProgramStatus::stepmode;
        // it is possible to pause a program from the error state
        return it != programs->end();
    }

	bool Processor::stopProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() ) {
            it->pstate = ProgramStatus::stopped;
            it->program->reset();
        }
        return it != programs->end();
    }

	bool Processor::deleteProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() && it->pstate == ProgramStatus::stopped )
            {
                MutexLock lock( progmonitor );
                delete it->program;
                programs->erase(it);
                return true;
            }
        if ( it == programs->end() ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It does not exist." );
                throw program_unload_exception( error );
            }
        if ( it->pstate != ProgramStatus::stopped ) {
                std::string error(
                                  "Could not unload Program \"" + name +
                                  "\" with the processor. It is still running." );
                throw program_unload_exception( error );
            }
        return false;
    }

	bool Processor::loadStateContext( StateContextTree* sc )
    {
        // test if parent ...
        if ( sc->getParent() != 0 ) {
            std::string error(
                "Could not register StateContext \"" + sc->getName() +
                "\" with the processor. It is not a root StateContext." );
            throw program_load_exception( error );
        }

        this->recursiveCheckLoadStateContext( sc ); // throws load_exception
        this->recursiveLoadStateContext( sc );
        return true;
    }
    
    void Processor::recursiveCheckLoadStateContext( StateContextTree* sc )
    {
        // test if already present..., this cannot detect corrupt
        // trees with double names...
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, sc->getName() ) );
        if ( it != states->end() ) {
            std::string error(
                "Could not register StateContext \"" + sc->getName() +
                "\" with the processor. A StateContext with that name is already present." );
            throw program_load_exception( error );

            std::vector<StateContextTree*>::const_iterator it2;
            for (it2 = sc->getChildren().begin(); it2 != sc->getChildren().end(); ++it2)
                {
                    this->recursiveCheckLoadStateContext( *it2 );
                }
        }
    }

    void Processor::recursiveLoadStateContext( StateContextTree* sc )
    {
        std::vector<StateContextTree*>::const_iterator it;
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveLoadStateContext( *it );
            }
        
        MutexLock lock( statemonitor );
        states->push_back(Processor::StateInfo(sc->getName(), sc));
    }

	bool Processor::startStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && it->sstate == StateContextStatus::active || it->sstate == StateContextStatus::paused) {
            it->action = bind( &StateInfo::start, &(*it) );
            return true;
        }
        return false;
    }

	bool Processor::stepStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && it->sstate == StateContextStatus::paused) {
            it->action = bind( &StateInfo::singleStep, &(*it) );
            return true;
        }
        return false;
    }

	bool Processor::steppedStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
            {
                it->stepping = true;
            }
        return it != states->end();
    }

	bool Processor::continuousStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
            {
                it->stepping = false;
            }
        return it != states->end();
    }

	bool Processor::isStateContextRunning(const std::string& name) const
    {
        cstate_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
            return it->sstate == StateContextStatus::running;
        return false;
    }

	bool Processor::isStateContextStepped(const std::string& name) const
    {
        cstate_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
            return it->stepping;
        return false;
    }

    bool Processor::activateStateContext( const std::string& name )
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && it->sstate == StateContextStatus::inactive )
            {
                it->activate();
                return true;
            }
        return false;
    }

    bool Processor::deactivateStateContext( const std::string& name )
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && it->sstate == StateContextStatus::stopped )
            {
                it->deactivate();
                return true;
            }
        return false;
    }

    bool Processor::pauseStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && ( it->sstate == StateContextStatus::running
                                      || it->sstate == StateContextStatus::paused
                                      || it->sstate == StateContextStatus::active))
            {
                it->action = bind( &StateInfo::pause, &(*it) );
                return true;
            }
        return false;
    }

	bool Processor::stopStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && ( it->sstate == StateContextStatus::paused
                                     || it->sstate == StateContextStatus::active
                                     || it->sstate == StateContextStatus::running) )
            {
                it->action = bind( &StateInfo::stop, &(*it) );
                return true;
            }
        return false;
    }

	bool Processor::resetStateContext(const std::string& name)
    {
        // We can only reset when stopped.
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() && it->sstate == StateContextStatus::stopped )
            {
                it->action = bind( &StateInfo::reset, &(*it) );
                return true;
            }
        return false;
    }

    bool Processor::unloadStateContext( const std::string& name )
    {
        // this does the same as deleteStateContext, except for deleting
        // the unloaded context..
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
        {
            // test if parent ...
            if ( it->state->getParent() != 0 ) {
                std::string error(
                                  "Could not unload StateContext \"" + it->state->getName() +
                                  "\" with the processor. It is not a root StateContext." );
                throw program_unload_exception( error );
            }
            recursiveCheckUnloadStateContext( *it );
            recursiveUnloadStateContext( it->state );
            return true;
        }
        return false;
    }

    void Processor::recursiveCheckUnloadStateContext(const StateInfo& si)
    {
        // check this state
        if ( si.sstate != StateContextStatus::inactive ) {
            std::string error(
                              "Could not unload StateContext \"" + si.state->getName() +
                              "\" with the processor. It is still active." );
            throw program_unload_exception( error );
        }

        // check children
        std::vector<StateContextTree*>::const_iterator it2;
        for (it2 = si.state->getChildren().begin();
             it2 != si.state->getChildren().end();
             ++it2)
            {
                state_iter it =
                    find_if(states->begin(),
                            states->end(),
                            bind(state_lookup, _1, (*it2)->getName() ) );
                if ( it == states->end() ) {
                    std::string error(
                              "Could not unload StateContext \"" + si.state->getName() +
                              "\" with the processor. It contains not loaded children." );
                    throw program_unload_exception( error );
                }
                // all is ok, check child :
                this->recursiveCheckUnloadStateContext( *it );
            }
    }

    void Processor::recursiveUnloadStateContext(StateContextTree* sc) {
        std::vector<StateContextTree*>::const_iterator it;
        // erase children
        for (it = sc->getChildren().begin(); it != sc->getChildren().end(); ++it)
            {
                this->recursiveUnloadStateContext( *it );
            }
        
        // erase this sc :
        state_iter it2 =
            find_if(states->begin(),
                    states->end(),
                    bind(state_lookup, _1, sc->getName() ) );
        assert( it2 != states->end() ); // we checked that this is possible

        MutexLock lock( statemonitor );
        states->erase(it2);
    }

	bool Processor::deleteStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states->begin(), states->end(), bind(state_lookup, _1, name) );
        if ( it != states->end() )
            {
                if ( it->state->getParent() != 0 ) {
                    std::string error(
                                      "Could not unload StateContext \"" + it->state->getName() +
                                      "\" with the processor. It is not a root StateContext." );
                    throw program_unload_exception( error );
                }
                StateContextTree* todelete = it->state;
                // same pre-conditions for delete as for unload :
                recursiveCheckUnloadStateContext( *it );
                recursiveUnloadStateContext( it->state ); // this invalidates it !
                delete todelete;
                return true;
            }
        return false;
    }

    void _executeState( Processor::StateInfo& s)
    {
        if ( s.action )
            s.action();
    }

    void _stopState( Processor::StateInfo& s)
    {
        if ( s.sstate == Processor::StateContextStatus::paused
            || s.sstate == Processor::StateContextStatus::active
            || s.sstate == Processor::StateContextStatus::running )
            s.stop();
    }

    void _executeProgram( Processor::ProgramInfo& p)
    {
        if (p.pstate == Processor::ProgramStatus::running) {
            if ( p.program->execute() == false )
                p.pstate = Processor::ProgramStatus::error;
            if ( p.program->isFinished() )
                p.pstate = Processor::ProgramStatus::stopped;
        }
    }

    void _stopProgram( Processor::ProgramInfo& p)
    {
        if (p.pstate == Processor::ProgramStatus::running) {
            p.pstate = Processor::ProgramStatus::stopped;
            p.program->reset();
        }
    }

    void _stepProgram( Processor::ProgramInfo& p)
    {
        if (p.pstate == Processor::ProgramStatus::stepmode && p.step)
            {
                if ( p.program->execute() == false )
                    p.pstate = Processor::ProgramStatus::error;
                if ( p.program->isFinished() )
                    p.pstate = Processor::ProgramStatus::stopped;
                p.step = false;
            }
    }

    bool Processor::initialize()
    {
        a_queue->clear();
        accept = true;
        return true;
    }

    void Processor::finalize()
    {
        accept = false;
        // stop all programs and SCs.
        {
            MutexLock lock( progmonitor );
            for_each(programs->begin(), programs->end(), _stopProgram);
        }
        {
            MutexLock lock( statemonitor );
            for_each(states->begin(), states->end(), _stopState );
        }
    }

	void Processor::step()
    {
        {
            MutexLock lock( statemonitor );
            // Evaluate all states->
            for_each(states->begin(), states->end(), _executeState );
        }

        {
            MutexLock lock( progmonitor );
            //Execute all normal programs->
            for_each(programs->begin(), programs->end(), _executeProgram);

            //Execute all programs in Stepping mode.
            for_each(programs->begin(), programs->end(), _stepProgram );
        }

        // Execute any FunctionGraph :
        {
            // 1. Fetch new ones from queue.
            while ( !f_queue->isEmpty() ) {
                ProgramInterface* foo;
                f_queue->dequeue( foo );
                // find an empty spot to store :
                while( *f_it != 0 ) {
                    ++f_it;
                    if (f_it == funcs.end() )
                        f_it = funcs.begin();
                }
                *f_it = foo;
            }
            // 2. execute all present
            for(std::vector<ProgramInterface*>::iterator it = funcs.begin();
                it != funcs.end(); ++it )
                if ( *it )
                    if ( (*it)->isFinished() || (*it)->inError() )
                        (*it) = 0;
                    else
                        (*it)->execute(); // should we check on inError() ?
        }

        // Execute any additional (deferred/external) command.
        {
            // execute one command from the AtomicQueue.
            CommandInterface* com;
            int res = a_queue->dequeueCounted( com );
            if ( res ) {
                com->execute();
                coms_processed = res;
            }
        }
    }


    bool Processor::stepProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() )
            it->step = true;
        return it != programs->end();
    }

    int Processor::process( CommandInterface* c )
    {
        if (accept)
            return a_queue->enqueueCounted( c );
        return 0;
    }

    bool Processor::runFunction( ProgramInterface* f )
    {
        if (accept)
            return f_queue->enqueue( f );
        return false;
    }

    bool Processor::isFunctionFinished( ProgramInterface* f )
    {
        if ( f == 0 )
            return false;
        // function is finished if it is no longer in our map.
        return std::find( funcs.begin(), funcs.end(), f ) == funcs.end();
    }

    std::vector<std::string> Processor::getProgramList()
    {
        std::vector<std::string> ret;
        for ( program_iter i = programs->begin(); i != programs->end(); ++i )
            ret.push_back( i->name );
        return ret;
    }

    ProgramInterface* Processor::getProgram(const std::string& name) const
    {
        program_iter it =
            find_if(programs->begin(), programs->end(), bind(program_lookup, _1, name) );
        if ( it != programs->end() )
            return it->program;
        return 0;
    }

    std::vector<std::string> Processor::getStateContextList()
    {
        std::vector<std::string> ret;
        for ( state_iter i = states->begin(); i != states->end(); ++i )
            ret.push_back( i->name );
        return ret;
    }

    bool Processor::isProcessed( int nr )
    {
        return nr <= coms_processed;
    }
}

