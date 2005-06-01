/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  StateMachine.cxx

                        StateMachine.cxx -  description
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
#include "execution/StateMachine.hpp"
#include "execution/mystd.hpp"
#include "boost/tuple/tuple.hpp"

#include <functional>

#include <assert.h>
#include <boost/bind.hpp>

namespace ORO_Execution
{
    using boost::tuples::get;
    using namespace std;
    using namespace boost;

    StateMachine::StateMachine(StateMachine* parent, const string& name )
        : _parent (parent) , _name(name),
          initstate(0), finistate(0), current( 0 ), next(0), initc(0),
          currentProg(0), currentExit(0), currentHandle(0), currentEntry(0), currentRun(0),
          checking_precond(false), error(false), evaluating(0)
    {}

    StateMachine::~StateMachine()
    {
        delete initc;
    }


    bool StateMachine::requestInitialState()
    {
        // all conditions that must be satisfied to enter the initial state :
        if ( !currentProg && ( current == initstate || current == finistate ) )
        {
            changeState( initstate );
            return true;
        }
        return false;
    }

    void StateMachine::requestFinalState()
    {
        // if we are inactive or in transition, don't do anything.
        if ( current == 0 || ( error == false && currentProg ) )
            return;

        error = false;
        changeState( finistate );
    }

    void StateMachine::changeState(StateInterface* newState, bool stepping) {
        if ( newState == current )
            {
                // execute the default action (handle current)
                // if no transition to another state took place
                handleState( current );
            }
        else
            {
                // reset handle, in case it is still set ( during error ).
                currentRun = 0;
                currentHandle = 0;
                // if error in current Exit, skip it.
                if ( currentExit && currentExit->inError() )
                    currentExit = 0;
                else
                    leaveState( current );
                enterState( newState );
            }

        // if not stepping, try to execute handle directly.
        // if stepping, postpone this
        if ( !stepping )
            this->executePending(stepping);

        // schedule a run for the next 'step'.
        // if handle above finished, it will be called directly
        // in the user's executePending. if handle was not finished
        // or stepping, it will be called after handle.
        runState( newState );
    }

    StateInterface* StateMachine::requestNextState(bool stepping)
    {
        // bad idea, user, don't run this if we're not active...
        assert ( current != 0 );
        if ( currentProg ) {
            return current; // can not accept request, still in transition.
        }
        if ( reqstep == reqend ) { // if nothing to evaluate, just handle()
            changeState( current, stepping );
            return current;
        }

        // if we got here, at least one evaluation to check
        do {
            if ( get<0>(*reqstep)->evaluate() ) {
                // check preconds of target state :
                int cres = checkConditions( get<1>(*reqstep), stepping );
                if (cres == 0) {
                    break; // only returned in stepping
                }
                if( cres == 1) {
                    changeState( get<1>(*reqstep), stepping );
                    break; // valid transition
                }
                // if cres == -1 : precondition failed, increment reqstep...
            }
            if ( reqstep + 1 == reqend ) {
                // no transition was found, reset and 'schedule' a handle :
                reqstep = stateMap.find( current )->second.begin();
                evaluating = get<3>(*reqstep);
                changeState( current, stepping );
                break;
            }
            else {
                ++reqstep;
                evaluating = get<3>(*reqstep);
            }
        } while ( !stepping );

        return current;
    }

    int StateMachine::checkConditions( StateInterface* state, bool stepping ) {
        // if the preconditions of \a state are checked the first time in stepping mode, reset the iterators.
        if ( !checking_precond || !stepping ) {
            prec_it = precondMap.equal_range(state); // state is the _target_ state
        }

        // will be set to true if stepping below.
        //checking_precond = false;
        
        while ( prec_it.first != prec_it.second ) {
            if (checking_precond == false && stepping ) {
                evaluating = prec_it.first->second.second; // indicate we will evaluate this line (if any).
                checking_precond = true;
                return 0;
            }
            if ( prec_it.first->second.first->evaluate() == false ) {
                checking_precond = false;
                return -1; // precondition failed
            }
            ++( prec_it.first );
            if (stepping) {
                if ( prec_it.first != prec_it.second )
                    evaluating = prec_it.first->second.second; // indicate we will evaluate the next line (if any).
                checking_precond = true;
                return 0; // not done yet.
            }
        }
        checking_precond = false;
        return 1; // success !
    }
        

    StateInterface* StateMachine::nextState()
    {
        // bad idea, user, don't run this if we're not active...
        assert ( current != 0 );
        TransList::const_iterator it1, it2;
        it1 = stateMap.find( current )->second.begin();
        it2 = stateMap.find( current )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() && checkConditions( get<1>(*it1)) == 1 )
                return get<1>(*it1);

        return current;
    }

    std::vector<std::string> StateMachine::getStateList() const {
        vector<string> result;
        vector<StateInterface*> sl;
        transform( stateMap.begin(), stateMap.end(), back_inserter(sl), ORO_std::select1st<TransitionMap::value_type>() );
        transform( sl.begin(), sl.end(), back_inserter(result), bind( &StateInterface::getName, _1 ) );
        return result;
    }

    void StateMachine::addState( StateInterface* s )
    {
        stateMap[s];
    }


    StateInterface* StateMachine::getState(const string& name) const
    {
        TransitionMap::const_iterator it = stateMap.begin();
        while ( it != stateMap.end() ) {
            if ( it->first->getName() == name )
                return it->first;
            ++it;
        }
        return 0;
    }

    bool StateMachine::requestState( StateInterface * s_n )
    {
        // bad idea, user, don't run this if we're not active...
        assert ( current != 0 );

        if ( currentProg ) {
            return false; // can not accept request, still in transition
        }

        // we may make transition to next state :

        // to current state
        if ( current == s_n )
        {
            changeState( s_n );
            return true;
        }

        // to final state
        if ( finistate == s_n )
        {
            changeState( s_n );
            return true;
        }

        // to inital state from final state
        if ( initstate == s_n && current == finistate)
        {
            changeState( s_n );
            return true;
        }

        // between 2 specific states
        TransList::iterator it1, it2;
        it1 = stateMap.find( current )->second.begin();
        it2 = stateMap.find( current )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<1>(*it1) == s_n
                 && get<0>(*it1)->evaluate()
                 && checkConditions( s_n ) == 1 ) {
                changeState( s_n );
                // the request was accepted
                return true;
            }
        // the request has failed.
        return false;
    }

    int StateMachine::getLineNumber() const {
        // if not activated, return first line.
        // work around race condition that current[Prog] may become zero,
        // thus first store in local variable :
        StateInterface* statecopy = current;
        if ( statecopy == 0 )
            return 1;
        ProgramInterface* copy = currentProg;
        if ( copy )
            return copy->getLineNumber();
        if ( evaluating )
            return evaluating;

        // if none of the above, return entry point :
        return statecopy->getEntryPoint();
    }

    string StateMachine::getText() const {
        return string();
    }

    void StateMachine::preconditionSet(StateInterface* state, ConditionInterface* cnd, int line )
    {
        // we must be inactive.
        assert( current == 0);
        assert( stateMap.count( state ) == 1 && "Add a state first before setting preconditions !" );
        precondMap.insert( make_pair(state, make_pair( cnd, line)) );
    }

    void StateMachine::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority, int line )
    {
        // we must be inactive.
        assert( current == 0);
        assert( stateMap.count( from ) == stateMap.count(to) == 1 && "Add a state first before setting transitions !" );
        // insert both from and to in the statemap
        TransList::iterator it;
        for ( it= stateMap[from].begin(); it != stateMap[from].end() && get<2>(*it) >= priority; ++it)
            ; // this ';' is intentional 
        stateMap[from].insert(it, boost::make_tuple( cnd, to, priority, line ) );
        stateMap[to]; // insert empty vector for 'to' state.
    }

    StateInterface* StateMachine::currentState() const
    {
        return current;
    }

    ProgramInterface* StateMachine::currentProgram() const
    {
        return currentProg;
    }

    void StateMachine::leaveState( StateInterface* s )
    {
        currentExit = s->getExitProgram();
        if ( currentExit ) {
            currentExit->reset();
            if (currentProg == 0 )
                currentProg = currentExit;
        }
    }

    void StateMachine::runState( StateInterface* s )
    {
        currentRun = s->getRunProgram();
        if ( currentRun ) {
            currentRun->reset();
            if (currentProg == 0 )
                currentProg = currentRun;
        }
    }

    void StateMachine::handleState( StateInterface* s )
    {
        currentHandle = s->getHandleProgram();
        if ( currentHandle ) {
            currentHandle->reset();
            if (currentProg == 0 )
                currentProg = currentHandle;
        }
    }

    void StateMachine::enterState( StateInterface* s )
    {
        // Before a state is entered, all transitions are reset !
        TransList::iterator it;
        for ( it= stateMap.find(s)->second.begin(); it != stateMap.find(s)->second.end(); ++it)
            get<0>(*it)->reset();

        next = s;
        currentEntry = s->getEntryProgram();
        if ( currentEntry ) {
            currentEntry->reset();
            if (currentProg == 0 )
                currentProg = currentEntry;
        }
    }

    bool StateMachine::executePending( bool stepping )
    {
        // This function has great resposibility, since it acts like
        // a scheduler for pending requests. It tries to devise what to
        // do on basis of the contents of variables (like current*, next,...).
        // This is a somewhat
        // fragile implementation but requires very little bookkeeping.

        if ( error )
            return false;
        // if returns true : a transition (exit/entry) is done
        // and a new state may be requested.

        // first try exit
        if ( currentExit ) {
            if ( this->executeProgram(currentExit, stepping) == false )
                return false;
            // done.
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentEntry ? currentEntry : currentRun;
                return false;
            }
        }

        // only reset the reqstep if we changed state.
        // if we did not change state, it will be reset in requestNextState().
        if ( current != next ) {
            if ( next ) {
                reqstep = stateMap.find( next )->second.begin();
                reqend  = stateMap.find( next )->second.end();
                // init for getLineNumber() :
                if ( reqstep == reqend )
                    evaluating = 0;
                else
                    evaluating = get<3>(*reqstep);
            } else {
                current = 0;
                return true;  // done if current == 0 !
            }
            // make change transition after exit of previous state:
            current = next;
        }

        if ( currentEntry ) {
            if ( this->executeProgram(currentEntry, stepping) == false )
                return false;
            // done.
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentRun;
                return false;
            }
        }

        // Handle is executed after the transitions failed.
        if ( currentHandle ) {
            if ( this->executeProgram(currentHandle, stepping) == false )
                return false;
            // done.
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentRun;
                return false;
            }
        }

        // Run is executed before the transitions.
        if ( currentRun ) {
            if ( this->executeProgram(currentRun, stepping) == false )
                return false;
            // done.
            // in stepping mode, delay 'true' one executePending().
            if ( stepping )
                return false;

        }

        return true; // all pending is done
    }

    bool StateMachine::executeProgram(ProgramInterface*& cp, bool stepping)
    {
        // execute this stateprogram and cleanup if needed.
        currentProg = cp;
        if ( (stepping && currentProg->executeStep() == false )
             || (!stepping && currentProg->executeUntil() == false) ) {
            error = true;
            return false;
        }

        if ( !currentProg->isFinished() )
            return false; 

        cp = currentProg = 0;
        return true;
    }


    bool StateMachine::inTransition() const {
        return currentProg != 0;
    }

    void StateMachine::setInitialState( StateInterface* s )
    {
        initstate = s;
        stateMap[initstate];
    }

    void StateMachine::setFinalState( StateInterface* s )
    {
        finistate = s;
        stateMap[finistate];
    }

    bool StateMachine::activate()
    {
        if ( current != 0 )
            return false;

        if ( this->checkConditions( getInitialState() ) != 1 )
            return false; //preconditions not met.

        if ( initc ) {
            initc->reset();
            initc->execute();
        }

        current = getInitialState();
        enterState( current );
        reqstep = stateMap.find( current )->second.begin();
        reqend = stateMap.find( current )->second.end();

        // execute the entry program of the initial state.
        this->executePending();

        return true;
    }

    bool StateMachine::deactivate()
    {
        if ( current == 0 )
            return false;

        // whatever state we are in, leave it.
        // but if current exit is in error, skip it alltogether.
        if ( currentExit && currentExit->inError() )
            currentExit = 0;
        else
            leaveState( current );
        // do not call enterState( 0 )
        currentEntry = 0;
        currentHandle  = 0;
        currentRun  = 0;
        next = 0;

        // reset error flag.
        error = false;
        // this will execute the exitFunction (if any) and, if successfull,
        // set current to zero (using next).
        this->executePending();
        return true;
    }
}

