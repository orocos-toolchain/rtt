/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  StateMachine.cxx

                        StateMachine.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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
#include "StateMachine.hpp"
#include "../ExecutionEngine.hpp"
#include "../internal/DataSource.hpp"
#include "../Service.hpp"
#include "CommandFunctors.hpp"
#include <Logger.hpp>
#include <functional>

#include <assert.h>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include "internal/mystd.hpp"

#define TRACE(msg) do {                                                                     \
    if (!mtrace)                                                                            \
      break;                                                                                \
    Logger::In in( _name );                                                                 \
    log(Info) << '[' << this->getStatusStr() << ']' << std::string(" ") + msg <<endlog();   \
} while(0)

namespace RTT {
    using namespace detail;
    using boost::tuples::get;
    using namespace std;
    using namespace boost;


    std::string StateMachine::emptyString;

    StateMachine::StateMachine(StateMachinePtr parent, const string& name )
        : smpStatus(nill), _parent (parent) , _name(name), smStatus(Status::unloaded),
          initstate(0), finistate(0), current( 0 ), next(0), initc(0),
          currentProg(0), currentExit(0), currentHandle(0), currentEntry(0), currentRun(0), currentTrans(0),
          checking_precond(false), mstep(false), mtrace(false), evaluating(0)
    {
        this->addState(0); // allows global state transitions
    }

   StateMachine::~StateMachine()
    {
       if ( this->isLoaded() ){
           getEngine()->removeFunction(this);
       }
        delete initc;
        TRACE( "StateMachine '" + _name + "' destroyed." );
    }

   void StateMachine::loading() {
       TRACE( "Being Loaded in ExecutionEngine." );
       smStatus = Status::inactive;
       for(TransitionMap::iterator it=stateMap.begin(); it != stateMap.end(); ++it) {
           // inform all entry/exit state scripts:
           if (it->first)
               it->first->loaded( this->getEngine() );
           // inform all transition scripts
           for(TransList::iterator tlit= it->second.begin(); tlit != it->second.end(); ++tlit ) {
               if ( get<4>(*tlit) )
                   get<4>(*tlit)->loaded( this->getEngine() );
           }
       }
       for( EventMap::iterator it = eventMap.begin(); it != eventMap.end(); ++it)
       {
           // inform all transition scripts
           for(EventList::iterator tlit= it->second.begin(); tlit != it->second.end(); ++tlit ) {
               if ( get<5>(*tlit) )
                   get<5>(*tlit)->loaded( this->getEngine() );
           }
       }
   }

   void StateMachine::unloading() {
       TRACE( "Being unloaded from ExecutionEngine." );
           if ( this->isActive() == false)
               return;
           if ( this->isReactive() )
               this->requestFinalState();
           if ( this->isAutomatic() )
               this->stop();
           if (this->currentState() != this->getFinalState() )
               this->execute(); // try one last time
           if (this->currentState() != this->getFinalState() )
               log(Critical) << "Failed to bring StateMachine "<< this->getName()
                             << " into the final state. Program stalled in state '"
                             << this->currentState()->getName()<<"' line number "
                             << this->getLineNumber()<<endlog(); // critical failure !
   }

    StateMachine::Status::StateMachineStatus StateMachine::getStatus() const {
        return smStatus;
    }

    string StateMachine::getStatusStr() const {

        switch ( smStatus )
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

    bool StateMachine::pause()
    {
        if ( smStatus != Status::inactive && smStatus != Status::unloaded ) {
            TRACE( "Will pause." );
            if (currentProg) {
                currentProg->pause();
                currentProg->execute();
            }
            smpStatus = pausing;
            return true;
        }
        TRACE( "Won't pause." );
        return false;
    }

    bool StateMachine::step()
    {
        if ( smStatus == Status::paused && mstep == false ) {
            TRACE( "Will step." );
            mstep = true;
            return true;
        }
        if ( smStatus == Status::active ) {
            TRACE( "Will step." );
            smStatus = Status::requesting;
            return true;
        }
        TRACE( "Won't step." );
        return false;
    }

    bool StateMachine::start()
    {
        return this->automatic();
    }

    bool StateMachine::automatic()
    {
        // if you go from reactive to automatic,
        // first execute the run program, before
        // evaluating transitions.
        if ( smStatus != Status::inactive && smStatus != Status::unloaded && smStatus != Status::error) {
            TRACE( "Will start." );
            smStatus = Status::running;
            os::MutexLock lock(execlock);
            runState( current );
            return true;
        }
        TRACE( "Won't start." );
        return false;
    }

    bool StateMachine::reactive()
    {
        if ( smStatus != Status::inactive && smStatus != Status::unloaded && smStatus != Status::error ) {
            TRACE( "Will enter reactive mode." );
            smStatus = Status::active;
            return true;
        }
        TRACE( "Won't enter reactive mode." );
        return false;
    }

    bool StateMachine::stop()
    {
        if ( smStatus != Status::inactive && smStatus != Status::unloaded ) {
            TRACE( "Will stop." );
            smpStatus = gostop;
            return true;
        }
        TRACE( "Won't stop." );
        return false;
    }

    bool StateMachine::reset()
    {
        // if waiting in final state, go ahead.
        if ( smStatus == Status::stopped ) {
            TRACE( "Will reset.");
            smpStatus = goreset;
            return true;
        }
        TRACE("Won't reset.");
        return false;
    }

    bool StateMachine::execute()
    {
        // before dealing with transitional states,
        // check if we're actually running.
        if (smStatus == Status::inactive || smStatus == Status::unloaded) {
            smpStatus = nill;
            return true;
        }
        os::MutexLock lock(execlock);

        // internal transitional issues.
        switch (smpStatus) {
        case pausing:
            TRACE("Is paused now.");
            smStatus = Status::paused;
            smpStatus = nill;
            return true;
            break;
        case gostop:
            this->executePending();
            if ( this->requestFinalState() ) {
                // test for atomicity :
                if ( this->inTransition() ) {
                    smStatus = Status::stopping;
                } else {
                    TRACE("Is stopped now.");
                    smStatus = Status::stopped;
                }
                smpStatus = nill;
            }
            return true;
            break;
        case goreset:
            if ( this->executePending() ) {
                this->requestInitialState();
                if ( this->inTransition() ) {
                    smStatus = Status::resetting;
                } else {
                    TRACE("Is reset now.");
                    smStatus = Status::active;
                }
            }
            smpStatus = nill;
            return true;
            break;
        case nill:
            break;
        }

        // public visible states.
        switch (smStatus) {
        case Status::inactive:
            return true;
            break;
        case Status::requesting:
            if ( this->executePending() ) {   // if all steps done,
                this->requestNextState();
                this->executePending();       // execute steps of next state
                TRACE("Is active now.");
                smStatus = Status::active;
            }
            break;
        case Status::active:
            this->executePending();
            break;
        case Status::running:
            if ( this->executePending() == false) {
                TRACE("Yielding...");
                break;
            }
            // if all pending done:
            this->requestNextState();     // one state at a time
            this->executePending();       // execute steps of next state
            break;
        case Status::paused:
            if (mstep) {
                if ( this->executePending(true) ) {    // if all steps done,
                    this->requestNextState(true); // one state at a time
                    this->executePending(true);       // execute steps of next state
                }
                TRACE("Did a step.");
                mstep = false;
            }
            break;
        case Status::error:
        case Status::unloaded:
            break;
        case Status::activating:
            this->executePending();
            if ( !this->inTransition() ) {
                TRACE("Is active now.");
                smStatus = Status::active;
            }
            break;
        case Status::stopping:
            if ( this->executePending() ) {
                TRACE("Is stopped now.");
                smStatus = Status::stopped;
            }break;
        case Status::deactivating:
            if ( this->executePending() ) {
                TRACE("Is inactive now.");
                smStatus = Status::inactive;
            }break;
        case Status::resetting:
            if ( this->executePending() ) {
                TRACE("Is reset now.");
                smStatus = Status::active;
            }break;
        case Status::stopped:
            this->executePending();
            if ( current != finistate ) {// detect leaving final state by event/request
                smStatus = Status::active;
            } break;
        }
        return true;
    }

    bool StateMachine::requestInitialState()
    {
        os::MutexLock lock(execlock);
        // all conditions that must be satisfied to enter the initial state :
        if ( interruptible() && ( current == initstate || current == finistate ) )
        {
            TRACE("Will enter initial state.");
            // this will try to execute the state change atomically
            this->requestStateChange( initstate );
            return true;
        }
        TRACE("Won't enter initial state.");
        return false;
    }

    bool StateMachine::requestFinalState()
    {
        os::MutexLock lock(execlock);
        // if we are inactive or in transition, don't do anything.
        if ( current == 0 || ( !inError() && !interruptible() ) ) {
            TRACE("Won't enter final state.");
            return false;
        }

        // this will try to execute the state change atomically
        if ( this->requestStateChange( finistate ) ) {
            TRACE("Will enter final state.");
            return true;
        }
        TRACE("Won't enter final state.");
        return false;
    }

    void StateMachine::changeState(StateInterface* newState, ProgramInterface* transProg, bool stepping) {
        if ( newState == current )
            {
                // this is only true if current state was selected in a transition of current.
                if ( transProg ) {
                    TRACE("Transition triggered to self: '"+current->getName()+"'");
                    transProg->reset();
                    if (transProg->start() == false )
                        smStatus = Status::error;
                    currentTrans = transProg;
                    // manually reset reqstep, or the next iteration would skip transition checks.
                    reqstep = stateMap.find( current )->second.begin();
                    // from now on, we are in transition to self !
                    // currentRun is _not_ set to zero or reset.
                    // it is/may be interrupted by trans, then continued.
                    // if already in handle, it is completed before trans, then trans is executed.
                    // since a transProg was given, no handle is executed.
                } else {
                    // execute the default action (schedule handle )
                    // if no transition to another state took place and no transprog specified.
                    // only schedule a handle if not yet in progress.
                    if ( currentHandle == 0 )
                        handleState( current );
                }
            }
        else
            {
                TRACE("Transition triggered from '"+ (current ? current->getName() : "null") +"' to '"+(newState ? newState->getName() : "null")+"'.");
                // reset handle and run, in case it is still set ( during error
                // or when an event arrived ).
                currentRun = 0;
                currentHandle = 0;
                if ( transProg ) {
                    transProg->reset();
                    if ( transProg->start() == false )
                        smStatus = Status::error;

                }
                currentTrans = transProg;

                next = newState;

                disableEvents(current);
                leaveState(current);
                assert( currentEntry == 0); // we assume that in executePending, trans and exit get executed first and entry is stil null.
            }
        // schedule a run for the next 'step'.
        // if handle above finished, run will be called directly
        // in executePending. if handle was not finished
        // or stepping, it will be called after handle.
        runState( newState );
    }

    void StateMachine::enableGlobalEvents( )
    {
        enableEvents(0);
    }
    void StateMachine::disableGlobalEvents( )
    {
        disableEvents(0);
    }
    void StateMachine::enableEvents( StateInterface* s )
    {
//        if (s) {
//            TRACE("Enabling events for state '"+s->getName()+"'.");
//        } else
//            TRACE("Enabling global events.");
        EventMap::mapped_type& hlist = eventMap[s];
        for (EventList::iterator eit = hlist.begin();
             eit != hlist.end();
             ++eit) {
            assert( get<6>(*eit).connected() == false );
            get<6>(*eit).connect();
        }
    }
    void StateMachine::disableEvents( StateInterface* s )
    {
//        if (s) {
//            TRACE("Disabling events for state '"+s->getName()+"'.");
//        } else
//            TRACE("Disabling global events.");
        EventMap::mapped_type& hlist = eventMap[s];
                for (EventList::iterator eit = hlist.begin();
                     eit != hlist.end();
                     ++eit) {
                    assert( get<6>(*eit).connected() == true );
                    get<6>(*eit).disconnect();
                }

    }

    bool StateMachine::createEventTransition( ServicePtr sp, ExecutionEngine* target_engine,
                                              const std::string& ename, vector<DataSourceBase::shared_ptr> args,
                                              StateInterface* from, StateInterface* to,
                                              ConditionInterface* guard, boost::shared_ptr<ProgramInterface> transprog,
                                              StateInterface* elseto, boost::shared_ptr<ProgramInterface> elseprog )
    {
        Logger::In in("StateMachine::createEventTransition");
        DisposableInterface::shared_ptr di =  sp->getLocalOperation(ename);
        OperationCallerInterface::shared_ptr oci = dynamic_pointer_cast<OperationCallerInterface>(di);
        if ( !oci ) {
            log(Error) << "Can not receive event '"<< ename <<"' in StateMachine : not a local operation."<< endlog();
            return false;
        }

        if ( !( sp && guard ) ) {
            log(Error) << "Invalid arguments for event '"<< ename <<"'. ";
            if (!sp)
                log() <<"Service was null. ";
            if (!guard)
                log() <<"Guard Condition was null. ";
            log() << endlog();
            return false;
        }

        if ( to == 0 )
            to = from;

        // get ename from event service, provide args as arguments
        // event should be activated upon entry of 'from'
        // guard is evaluated to get final 'ok'.
        // event does a requestStateChange( to );
        // if 'ok', execute transprog during transition.

        // Store guard and transprog for copy/clone semantics.
        // upon SM copy, recreate the handles for the copy SM with a copy of guard/transprog.
        // Same for args. I guess we need to store all arguments of this function to allow
        // proper copy semantics, such that the event handle can be created for each new SM
        // instance. Ownership of guard and transprog is to be determined, but seems to ly
        // with the SM. handle.destroy() can be called upon SM destruction.
        Handle handle;

        log(Debug) << "Creating Signal handler for Operation '"<< ename <<"' from state "<< (from ? from->getName() : string("(global)")) << " to state " << ( to ? to->getName() : string("(global)") ) <<Logger::endl;
#ifdef ORO_SIGNALLING_OPERATIONS
        // don't deliver this signal asynchronously, we want it right away.
        handle = sp->produceSignal( ename, new CommandFunction( boost::bind( &StateMachine::eventTransition, this, from, guard, transprog.get(), to, elseprog.get(), elseto) ), args, 0 );
#endif
        if ( !handle.ready() ) {
            Logger::log() << Logger::Error << "Could not setup handle for event '"<<ename<<"'."<<Logger::endl;
            return false; // event does not exist...
        }
        // all our handles must start in disconnected state:
        handle.disconnect();
        // BIG NOTE : we MUST store handle otherwise, the connection is destroyed (cfr setup vs connect).
        // Off course, we also need it to connect/disconnect the event.
        eventMap[from].push_back( boost::make_tuple( sp, ename, args, to, guard, transprog, handle, elseto, elseprog) );
        // add the states to the statemap.
        stateMap[from];
        stateMap[to];
        return true;
    }

    bool StateMachine::eventTransition(StateInterface* from, ConditionInterface* c, ProgramInterface* p, StateInterface* to, ProgramInterface* elsep, StateInterface* elseto )
    {
        // called by event to begin Transition to 'to'.
        // This interrupts the current run program at yield point
        // the transition and/or exit program can cleanup...

        MutexLock lock(execlock); // recursive

        if ( !current)
            return true;

        // Only transition if this event was meant for this state and we are not
        // in transition already.
        // If condition fails, check precondition 'else' state (if present) and
        // execute else program (may be null).

        if (from == 0)
            from  = current;
        if (to == 0)
            to = current;
        TRACE("Received Signal in state '"+ current->getName()+"' for transition from state '" + from->getName() + "' to state '" + to->getName() + "'");
        if ( from == current && !this->inTransition() ) {
            if ( c->evaluate() && checkConditions(to, false) == 1 ) {
                TRACE( "Valid transition from " + from->getName() +
                       +" to "+to->getName()+".");
                // stepping == false ! This executes the whole state transition
                changeState( to, p, false );              //  valid transition to 'to'. Q: should stepping be on or off ?
                // trigger EE in order to force execution of the remainder of the state transition:
                this->getEngine()->getActivity()->timeout();
            }
            else {
                if ( c->evaluate() == false ) 
                TRACE( "Rejected transition from " + from->getName() +
                        " to " + to->getName() +
                        " within state " + current->getName() + ": guards failed.");
                if ( checkConditions(to,false) != 1 ) 
                TRACE( "Rejected transition from " + from->getName() +
                        " to " + to->getName() +
                        " within state " + current->getName() + ": preconditions failed.");
            }
        }
#if 1
        else {
            if (this->inTransition() ) {
                TRACE( "Rejected transition from " + from->getName() +
                              " within " + current->getName() + ": already in transition.");
            } else {
                TRACE( "Rejected transition from " + from->getName() +
                              + " within " + current->getName() + ": wrong state.");
            }
        }
#endif
        return true;
    }

    StateInterface* StateMachine::requestNextState(bool stepping)
    {
        os::MutexLock lock(execlock);
        // bad idea, user, don't run this if we're not active...
        if( current == 0 )
            return 0;
        // only a run program may be interrupted...
        if ( !interruptible() || currentTrans ) {
            return current; // can not accept request, still in transition.
        }

        // Reset global conditions.
        TransList::const_iterator it, it1, it2;
        it1 = stateMap.find( 0 )->second.begin();
        it2 = stateMap.find( 0 )->second.end();

        if ( reqstep == stateMap.find( current )->second.begin() ) // avoid reseting too much in stepping mode.
            for ( it= it1; it != it2; ++it)
                get<0>(*it)->reset();

        if ( reqstep == reqend ) { // if nothing to evaluate, eval globals, then just handle()

            for ( ; it1 != it2; ++it1 )
                if ( get<0>(*it1)->evaluate()
                     && checkConditions( get<1>(*it1) ) == 1 ) {
                    StateInterface* next = get<1>(*it1);
                    if ( next == 0 ) // handle current if no next
                        changeState( current, get<4>(*it1).get(), stepping );
                    else
                        changeState( next, get<4>(*it1).get(), stepping );
                    // the request was accepted
                    return current;
                }
            // no transition found: handle()
            changeState( current, 0, stepping );
            return current;
        }

        // if we got here, at least one evaluation to check
        do {
            if ( get<0>(*reqstep)->evaluate() ) {
                // evaluate() might call stop() or other sm functions:
                if (reqstep == reqend )
                    return current;
                // check preconds of target state :
                int cres = checkConditions( get<1>(*reqstep), stepping );
                if (cres == 0) {
                    break; // only returned in stepping
                }
                if( cres == 1) {
                    changeState( get<1>(*reqstep), get<4>(*reqstep).get(), stepping );
                    break; // valid transition
                }
                // if cres == -1 : precondition failed, increment reqstep...
            }
             if ( reqstep + 1 == reqend ) {
                // to a state specified by the user (global)
                for ( ; it1 != it2; ++it1 ) {
                    if ( get<0>(*it1)->evaluate() && checkConditions( get<1>(*it1) ) == 1 ) {
                             StateInterface* next = get<1>(*it1);
                             if ( next == 0) // handle current if no next
                                 changeState( current, get<4>(*it1).get(), stepping );
                             else
                                 changeState( next, get<4>(*it1).get(), stepping );
                             // the request was accepted
                             return current;
                         }
                    }
                // no transition was found, reset and 'schedule' a handle :
                reqstep = stateMap.find( current )->second.begin();
                evaluating = get<3>(*reqstep);
                changeState( current, 0, stepping );
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
        os::MutexLock lock(execlock);
        // bad idea, user, don't run this if we're not active...
        if ( current == 0 )
            return 0;
        TransList::const_iterator it1, it2;
        it1 = stateMap.find( current )->second.begin();
        it2 = stateMap.find( current )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() && checkConditions( get<1>(*it1)) == 1 ) {
                return get<1>(*it1);
            }

        // also check the global transitions.
        it1 = stateMap.find( 0 )->second.begin();
        it2 = stateMap.find( 0 )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() && checkConditions( get<1>(*it1)) == 1 ) {
                return get<1>(*it1);
            }

        return current;
    }

    std::vector<std::string> StateMachine::getStateList() const {
        vector<string> result;
        vector<StateInterface*> sl;
        StateInterface* dummy = 0;
        transform( stateMap.begin(), stateMap.end(), back_inserter(sl), select1st<TransitionMap::value_type>() );
        sl.erase( find(sl.begin(), sl.end(), dummy) );
        transform( sl.begin(), sl.end(), back_inserter(result), boost::bind( &StateInterface::getName, _1 ) );
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
            if ( it->first && it->first->getName() == name )
                return it->first;
            ++it;
        }
        return 0;
    }

    bool StateMachine::requestStateChange( StateInterface * s_n )
    {
        os::MutexLock lock(execlock);
        // bad idea, user, don't run this if we're not active...
        if( current == 0 )
            return false;

        if ( !interruptible() ) {
            return false; // can not accept request, still in transition
        }

        // we may make transition to next state :

        // to current state
        if ( current == s_n )
        {
            changeState( s_n, 0 );
            executePending();
            return true;
        }

        // between 2 states specified by the user.
        TransList::iterator it, it1, it2;
        it1 = stateMap.find( current )->second.begin();
        it2 = stateMap.find( current )->second.end();

        for ( ; it1 != it2; ++it1 )
            if ( get<1>(*it1) == s_n
                 && get<0>(*it1)->evaluate()
                 && checkConditions( s_n ) == 1 ) {
                changeState( s_n, get<4>(*it1).get() );
                // the request was accepted
                executePending();
                return true;
            }

        // to a state specified by the user (global)
        it1 = stateMap.find( 0 )->second.begin();
        it2 = stateMap.find( 0 )->second.end();

        // reset all conditions
        for ( it= it1; it != it2; ++it)
            get<0>(*it)->reset();

        // evaluate them
        for ( ; it1 != it2; ++it1 )
            if ( get<1>(*it1) == s_n
                 && get<0>(*it1)->evaluate()
                 && checkConditions( s_n ) == 1 ) {
                changeState( s_n, get<4>(*it1).get() );
                // the request was accepted
                executePending();
                return true;
            }

        // to final state
        if ( finistate == s_n )
        {
            changeState( s_n, 0 );
            executePending();
            return true;
        }

        // to inital state from final state
        if ( initstate == s_n && current == finistate)
        {
            changeState( s_n, 0 );
            executePending();
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
        if ( current != 0)
            return;
        precondMap.insert( make_pair(state, make_pair( cnd, line)) );
        stateMap[state]; // add to state map.
    }

    void StateMachine::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority, int line )
    {
        this->transitionSet( from, to, cnd, boost::shared_ptr<ProgramInterface>(), priority, line);
    }

    void StateMachine::transitionSet( StateInterface* from, StateInterface* to,
                                      ConditionInterface* cnd, boost::shared_ptr<ProgramInterface> transprog,
                                      int priority, int line )
    {
        // we must be inactive.
        if ( current != 0)
            return;

        if (from) {
            TRACE("Created transition from "+from->getName() +"' to '"+ to->getName()+"'");
        } else {
            TRACE("Created global transition to '"+ to->getName()+"'");
        }
        // insert both from and to in the statemap
        TransList::iterator it;
        for ( it= stateMap[from].begin(); it != stateMap[from].end() && get<2>(*it) >= priority; ++it)
            ; // this ';' is intentional
        stateMap[from].insert(it, boost::make_tuple( cnd, to, priority, line, transprog ) );
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
        assert(s);
//        TRACE( "Planning to leave state " + s->getName() );
        currentExit = s->getExitProgram();
        if ( currentExit ) {
            currentExit->reset();
            if (currentExit->start() == false)
                smStatus = Status::error;
        }
    }

    void StateMachine::runState( StateInterface* s )
    {
        assert(s);
        currentRun = s->getRunProgram();
        if ( currentRun ) {
            currentRun->reset();
            if (currentRun->start() == false)
                smStatus = Status::error;
        }
    }

    void StateMachine::handleState( StateInterface* s )
    {
        assert(s);
        currentHandle = s->getHandleProgram();
        if ( currentHandle ) {
            currentHandle->reset();
            if (currentHandle->start() == false)
                smStatus = Status::error;
        }
    }

    void StateMachine::enterState( StateInterface* s )
    {
        assert(s);
//        TRACE( "Planning to enter state " + s->getName() );

        // Before a state is entered, all transitions are reset !
        TransList::iterator it;
        for ( it= stateMap.find(s)->second.begin(); it != stateMap.find(s)->second.end(); ++it)
            get<0>(*it)->reset();

        currentEntry = s->getEntryProgram();
        if ( currentEntry ) {
            currentEntry->reset();
            if (currentEntry->start() == false)
                smStatus = Status::error;
        }
    }

    bool StateMachine::executePending( bool stepping )
    {
        // This function has great resposibility, since it acts like
        // a scheduler for pending requests. It tries to devise what to
        // do on basis of the contents of variables (like current*, next,...).
        // This is a somewhat
        // fragile implementation but requires very little bookkeeping.
        // if returns true : a transition (exit/entry) is done
        // and a new state may be requested.

        if ( inError() )
            return false;

        // first try to finish the current entry program, if any:
        if ( currentEntry ) {
            TRACE("Executing entry program of '"+ (current ? current->getName() : "(null)") +"'" );
            if ( this->executeProgram(currentEntry, stepping) == false )
                return false;
            // done.
            TRACE("Finished  entry program of '"+ (current ? current->getName() : "(null)") +"'" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentRun;
                return false;
            }
        }

        // if a transition has been scheduled, proceed directly instead of doing a run:
        if ( currentTrans ) {
            TRACE("Executing transition program from '"+ (current ? current->getName() : "(null)") + "' to '"+ ( next ? next->getName() : "(null)")+"'" );
            // exception : transition during handle, first finish handle !
            if ( currentHandle ) {
                if ( this->executeProgram(currentHandle, stepping) == false )
                return false;
            } else
                if ( this->executeProgram(currentTrans, stepping) == false )
                    return false;
            // done.
            TRACE("Finished  transition program from '"+ (current ? current->getName() : "(null)") + "' to '"+ ( next ? next->getName() : "(null)")+"'" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentExit ? currentExit : (currentEntry ? currentEntry : currentRun);
                return false;
            }
        }

        // last is exit
        if ( currentExit ) {
            TRACE("Executing exit program from '"+ (current ? current->getName() : "(null)") + "' (going to '"+ (next ? next->getName() : "(null)") +"')" );
            if ( this->executeProgram(currentExit, stepping) == false )
                return false;
            // done.
            TRACE("Finished  exit program from '"+ (current ? current->getName() : "(null)") + "' (going to '"+ (next ? next->getName() : "(null)") +"')" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = (currentEntry ? currentEntry : currentRun);
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
            TRACE("Formally  transitioning from      '"+ (current ? current->getName() : "(null)") + "' to '"+ (next ? next->getName() : "(null)") +"'" );

            current = next;
            enterState(next);
            enableEvents(next);
        }

        // finally, execute the current Entry of the new state:
        if ( currentEntry ) {
            TRACE("Executing entry program of '"+ (current ? current->getName() : "(null)") +"'" );
            if ( this->executeProgram(currentEntry, stepping) == false )
                return false;
            // done.
            TRACE("Finished  entry program of '"+ (current ? current->getName() : "(null)") +"'" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentRun;
                return false;
            }
            // Either way, if currentEntry was set here, we get out by returning true or false.
            // currentHandle is unlikely to be set, but currentRun is set by 'changeState' and
            // should only be executed the next execution cycle (if no transition is scheduled by then).
            return true;
        }

        // Handle is executed after the transitions failed.
        if ( currentHandle ) {
            TRACE("Executing handle program of '"+ (current ? current->getName() : "(null)") +"'" );
            if ( this->executeProgram(currentHandle, stepping) == false )
                return false;
            // done.
            TRACE("Finished  handle program of '"+ (current ? current->getName() : "(null)") +"'" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping ) {
                currentProg = currentRun;
                return false;
            }
        }

        // Run is executed before the transitions.
        if ( currentRun ) {
            TRACE("Executing run program of '"+ (current ? current->getName() : "(null)") +"'" );
            if ( this->executeProgram(currentRun, stepping) == false )
                return false;
            // done.
            TRACE("Finished  run program of '"+ (current ? current->getName() : "(null)") +"'" );
            // in stepping mode, delay 'true' one executePending().
            if ( stepping )
                return false;
        }

        return true; // all pending is done
    }

    bool StateMachine::executeProgram(ProgramInterface*& cp, bool stepping)
    {
        if ( cp == 0)
            return false;
        // execute this stateprogram and cleanup if needed.
        currentProg = cp;
        if ( stepping )
            currentProg->step();
        else
            currentProg->execute();
        if ( currentProg && currentProg->inError() ) {
            smStatus = Status::error;
            smpStatus = nill;
            TRACE("Encountered run-time error at line " << this->getLineNumber() );
            return false;
        }

        if ( currentProg && !currentProg->isStopped() )
            return false;

        cp = currentProg = 0;
        return true;
    }


    // in exit/entry or transition programs
    bool StateMachine::inTransition() const {
        return ( (currentProg != 0)  && (currentProg != currentRun) && (currentProg != currentHandle) ) || (current != next);
    }

    // only run program may be interrupted.
    bool StateMachine::interruptible() const {
        return currentProg == 0  || currentProg == currentRun;
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

    void StateMachine::trace(bool t) {
        mtrace =t;
    }

    bool StateMachine::activate()
    {
        // inactive implies loaded, but check additionally if smp is at least active
        if ( smStatus != Status::inactive ) {
            TRACE("Won't activate: already active.");
            return false;
        }
        os::MutexLock lock(execlock);

        smpStatus = nill;

        if ( this->checkConditions( getInitialState() ) != 1 ) {
            TRACE("Won't activate: preconditions failed.");
            return false; //preconditions not met.
        }

        if ( initc ) {
            initc->reset();
            initc->readArguments();
            if ( initc->execute() == false ) {
                TRACE("Won't activate: Init Commands failed.");
                return false; // fail to activate.
            }
        }

        current = getInitialState();
        next = current;
        enterState( getInitialState() );
        reqstep = stateMap.find( next )->second.begin();
        reqend = stateMap.find( next )->second.end();

        // Enable all event handlers
        enableGlobalEvents();
        enableEvents(current);

        // execute the entry program of the initial state.
        if ( !inError() ) {
            if ( this->executePending() ) {
                smStatus = Status::active;
                TRACE("Activated.");
            } else {
                if ( !inError() ) {
                    TRACE("Still activating.");
                    smStatus = Status::activating;
                }
            }
        }

        return true;
    }


    bool StateMachine::deactivate()
    {
        // the only time to refuse executing this is when we did set ourselves to inactive before.
        if ( smStatus == Status::inactive) {
            TRACE("Won't deactivate: already inactive.");
            return false;
        }
        os::MutexLock lock(execlock);

        // disable all event handlers
        disableGlobalEvents();

        // whatever state we are in, leave it.
        // but if current exit is in error, skip it alltogether.
        if ( currentExit && currentExit->inError() )
            currentExit = 0;
        if ( currentTrans && currentTrans->inError() )
            currentTrans = 0;
        // if we stalled, in previous deactivate
        // even skip/stop exit program.
        if ( next != 0 && current ) {
            leaveState( current );
            disableEvents(current);
        } else {
            currentExit = 0;
            currentTrans = 0;
        }

        // do not call enterState( 0 )
        currentProg = 0;
        currentEntry = 0;
        currentHandle  = 0;
        currentRun  = 0;
        next = 0;

        // this will execute the exitFunction (if any) and, if successfull,
        // set current to zero (using next).
        if ( this->executePending() ) {
            TRACE("Deactivated.");
            smStatus = Status::inactive;
        } else {
            TRACE("Still deactivating.");
            smStatus = Status::deactivating;
        }

        return true;
    }
}
