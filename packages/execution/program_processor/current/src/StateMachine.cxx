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
#include "boost/tuple/tuple.hpp"

#include <functional>

#include <assert.h>

namespace ORO_Execution
{
    using boost::tuples::get;

    StateMachine::StateMachine(StateMachine* parent, const std::string& name )
        : _parent (parent) , _name(name),
          initstate(0), finistate(0), current( 0 ), next(0), initc(0),
          currentProg(0), currentExit(0), currentHandle(0), currentEntry(0), error(false), evaluating(0)
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
                handleState( current );
            }
        else
            {
                // reset handle, in case it is still set ( during error ).
                currentHandle = 0;
                // if error in current Exit, skip it.
                if ( currentExit && currentExit->inError() )
                    currentExit = 0;
                else
                    leaveState( current );
                enterState( newState );
            }
        this->executePending(stepping);
    }

    StateInterface* StateMachine::requestNextState(bool stepping)
    {
        // bad idea, user, don't run this if we're not active...
        assert ( current != 0 );
        TransList::const_iterator the_end = stateMap.find( current )->second.end();
        if ( currentProg ) {
            return current; // can not accept request, still in transition.
        }
        if ( reqstep == the_end ) { // if nothing to evaluate, just handle()
            changeState( current, stepping );
            return current;
        }

        // if we got here, at least one evaluation to check
        do {
            evaluating = get<3>(*reqstep);
            if ( get<0>(*reqstep)->evaluate() ) {
                changeState( get<1>(*reqstep), stepping );
                break;
            }
            if ( reqstep + 1 == the_end ) {
                // no transition was found, reset and 'schedule' a handle :
                reqstep = stateMap.find( current )->second.begin();
                changeState( current, stepping );
                break;
            }
            else
                ++reqstep;
        } while ( !stepping );

        return current;
    }

    StateInterface* StateMachine::nextState()
    {
        // bad idea, user, don't run this if we're not active...
        assert ( current != 0 );
        TransList::const_iterator it1, it2;
        it1 = stateMap.find( current )->second.begin();
        it2 = stateMap.find( current )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() )
                return get<1>(*it1);

        return current;
    }

    StateInterface* StateMachine::getState(const std::string& name) const
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
                 && get<0>(*it1)->evaluate() )
            {
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

    std::string StateMachine::getText() const {
        return std::string();
    }

    void StateMachine::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority, int line )
    {
        // we must be inactive.
        assert( current == 0);
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

    void StateMachine::leaveState( StateInterface* s )
    {
        currentExit = s->getExitProgram();
        if ( currentExit )
            currentExit->reset();
    }

    void StateMachine::handleState( StateInterface* s )
    {
        currentHandle = s->getHandleProgram();
        if ( currentHandle )
            currentHandle->reset();
    }

    void StateMachine::enterState( StateInterface* s )
    {
        // Before a state is entered, all transitions are reset !
        TransList::iterator it;
        for ( it= stateMap.find(s)->second.begin(); it != stateMap.find(s)->second.end(); ++it)
            get<0>(*it)->reset();

        next = s;
        currentEntry = s->getEntryProgram();
        if ( currentEntry )
            currentEntry->reset();
//         currentHandle = s->getHandleProgram();
//         if ( currentHandle )
//             currentHandle->reset();
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
        if ( currentExit && this->executeProgram(currentExit, stepping) == false )
            return false;

        // make change transition after exit of previous state:
        current = next;
        if ( current ) {
            evaluating = 0; 
            reqstep = stateMap.find( current )->second.begin();
        }
        else
            return true; // done if current == 0 !
                
        if ( currentEntry && this->executeProgram(currentEntry, stepping) == false )
            return false;
        if ( currentHandle && this->executeProgram(currentHandle, stepping) == false )
            return false;

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

        if ( initc ) {
            initc->reset();
            initc->execute();
        }

        current = getInitialState();
        enterState( current );
        reqstep = stateMap.find( current )->second.begin();

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
        next = 0;

        // reset error flag.
        error = false;
        // this will execute the exitFunction (if any) and, if successfull,
        // set current to zero (using next).
        this->executePending();
        return true;
    }
}

