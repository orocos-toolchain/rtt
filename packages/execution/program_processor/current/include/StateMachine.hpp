/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  StateMachineTree.hpp 

                        StateMachineTree.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef HIERARCHICAL_STATE_MACHINE_HPP
#define HIERARCHICAL_STATE_MACHINE_HPP

#include "StateInterface.hpp"
#include "corelib/ConditionInterface.hpp"
#include "corelib/CommandInterface.hpp"
#include "corelib/DataSourceBase.hpp"
#include "corelib/Handle.hpp"

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <boost/tuple/tuple.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::ConditionInterface;
    using ORO_CoreLib::CommandInterface;

    class TaskContext;
    class EventService;

    /**
     * @brief A hierarchical StateMachine which is
     * loaded in the Program Processor.
     *
     * A StateMachine can have children and one parent.
     * @todo Implement the whole transition mechanism with the
     * Strategy software pattern to allow cleaner implementation.
     */
    class StateMachine
    {
        /**
         * The key is the current state, the value is the transition condition to
         * another state with a certain priority (int), on a line (int), with a transition program
         */
        typedef std::vector< boost::tuple<ConditionInterface*, StateInterface*, int, int, ProgramInterface*> > TransList;
        typedef std::map< StateInterface*, TransList > TransitionMap;
        typedef std::multimap< StateInterface*, std::pair<ConditionInterface*, int> > PreConditionMap;
        typedef std::vector< boost::tuple<EventService*,
                                          std::string, std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>,
                                          StateInterface*,
                                          ConditionInterface*, ProgramInterface*, ORO_CoreLib::Handle> > EventList;
        typedef std::map< StateInterface*, EventList > EventMap;
        std::vector<StateMachine*> _children;
        StateMachine* _parent;
    protected:
        std::string _name;
        TaskContext* taskcontext;
    public:

        typedef std::vector<StateMachine*> ChildList;

        /**
         * The destructor is virtual since ParsedStateMachine still inherits
         * this class.
         */
        virtual ~StateMachine();

        /**
         * Create a new StateMachine with an optional parent.
         * Set \a parent to zero for the top state machine. The initial Status of
         * a StateMachine is always inactive.
         */
        StateMachine(StateMachine* parent, const std::string& name="Default");

        /**
         * Create a new StateMachine in a TaskContext with an optional parent.
         * Set \a parent to zero for the top state machine. The initial Status of
         * a StateMachine is always inactive.
         */
        StateMachine(StateMachine* parent, TaskContext* tc, const std::string& name="Default");

        /**
         * Get the active status of this StateMachine.
         */
        inline bool isActive() const { return current != 0; }

        /**
         * Get the error status of this StateMachine.
         */
        inline bool inError() const { return error; }

        /**
         * Start this StateMachine. The Initial state will be entered.
         */
        bool activate();

        /**
         * Stop this StateMachine. The current state is left.
         */
        bool deactivate();

        /**
         * Search from the current state a candidate next state.
         * If none is found, the current state is taken.
         * Next, handle the resulting state.
         *
         * @note This call is @em not equivalent to
         * this->requestState( this->nextState() ), since multiple
         * invocations of this->nextState() may result in different results, hence,
         * this->requestState( this->nextState() ) may return false.
         * Use this method instead to automatically go to the next state.
         *
         * @param stepping provide true if the transition evaluations should 
         * be executed one at a time.
         * @return The current state.
         */
        StateInterface* requestNextState(bool stepping = false);

        /**
         * Go stepwise through evaluations to find out next state.
         * @see requestNextState()
         */
        StateInterface* requestNextStateStep();

        /**
         * Request going to the Final State. This will always
         * proceed.
         */
        void requestFinalState();

        /**
         * Request going to the Initial State. This function will only
         * proceed if the current state is the Final State or the
         * Initial State. If it fails, one can try
         * to requestNextState() which may lead to the
         * initial state anyway if the transition is set.
         * This path is not tried by this function.
         *
         * @return The true on success, false if not allowed.
         */
        bool requestInitialState();

        /**
         * Search from the current state a candidate next state.
         * If none is found, the current state is returned.
         * @note The mere calling of this method, may influence
         * future possible results. Multiple invocations of nextState()
         * may return different results, so use with care.
         * @see requestNextState()
         */
        StateInterface* nextState();

        /**
         * Get a list of the names of all the present states.
         */
        std::vector<std::string> getStateList() const;

        /**
         * Lookup a State by name. Returns null if not found.
         */
        StateInterface* getState( const std::string & name ) const;

        /**
         * Add a State. If already present, changes nothing.
         */
        void addState( StateInterface* s );

        /**
         * Request a state transition to a new state.
         * If the transition is not set by transitionSet(), acquiering
         * the state will fail.
         *
         * @param  s_n
         *         The state to change to
         * @retval true
         *          if the transition is successfull
         * @retval false
         *          if the transition is not allowed
         */
        bool requestState( StateInterface * s_n );

        /**
         * Execute any pending State (exit, entry, handle) programs.
         * You must executePending, before calling requestState() or
         * requestNextState(). You should only call requestState() or requestNextState()
         * if executePending returns true.
         *
         * Due to the pending requests, the currentState() may have changed.
         * 
         * @param stepping provide true if the pending programs should 
         * be executed one step at a time.
         * @retval true if nothing was pending @retval false if there was
         * some program executing.
         */
        bool executePending( bool stepping = false );

        /**
         * Express a precondition for entering a state.  The
         * precondition will be chained (Logical AND) with any
         * transition to this state. This means that any transition to
         * this state will only succeed if all preconditions hold. If
         * \a state is the initial state, the preconditions must be
         * true to make \a actiate() succeed. If \a state is the final
         * state, they will not be checked upon \a requestFinalState,
         * since requestFinalState always succeeds.
         *
         * @param state
         *        The state for which the preconditions must hold
         * @param cnd
         *        The Pre-Condition under which a transition to this state may succeed
         * @param line
         *        The line number where this precondition was introduced.
         */
        void preconditionSet( StateInterface* state, ConditionInterface* cnd, int line);

        /**
         * Express a possible transition from one state to another under
         * a certain condition.
         *
         * @param from
         *        The state which should be left
         * @param to
         *        The state which should be entered
         * @param cnd
         *        The Condition under which the transition may succeed
         * @param priority
         *        The priority of this transition; low number (like -1000) is low priority
         *        high number is high priority (like + 1000). Transitions of equal
         *        priority are traversed in an unspecified way.
         * @param line
         *        The line number where this transition was introduced.
         * @post  All transitions from \a from to \a to will succeed under
         *        condition \a cnd
         */
        void transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority, int line);

        /**
         * Express a possible transition from one state to another under
         * a certain condition.
         *
         * @param from
         *        The state which should be left
         * @param to
         *        The state which should be entered
         * @param cnd
         *        The Condition under which the transition may succeed
         * @param transprog
         *        The program to be executed between exit of \a from and entry of \a to.
         * @param priority
         *        The priority of this transition; low number (like -1000) is low priority
         *        high number is high priority (like + 1000). Transitions of equal
         *        priority are traversed in an unspecified way.
         * @param line
         *        The line number where this transition was introduced.
         * @post  All transitions from \a from to \a to will succeed under
         *        condition \a cnd
         */
        void transitionSet( StateInterface* from, StateInterface* to,
                            ConditionInterface* cnd, ProgramInterface* transprog,
                            int priority, int line);

        /**
         * Express a possible transition from one state to another
         * when an Event is fired under a certain condition (guard).
         *
         * @param ename
         *        The name of the ORO_CoreLib::Event under which a transition should be made
         * @param args
         *        The arguments which the event handler must set upon occurence.
         * @param from
         *        The state which should be left
         * @param to
         *        The state which should be entered
         * @param guard
         *        The Condition under which the transition may succeed
         * @param transprog
         *        The program to be executed between exit of \a from and entry of \a to.
         * @param es
         *        The EventService in which \a ename can be found.
         */
        bool createEventTransition( EventService* es,
                                    const std::string& ename, std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> args,
                                    StateInterface* from, StateInterface* to,
                                    ConditionInterface* guard, ProgramInterface* transprog );

        /**
         * Set the initial state of this StateMachine.
         */
        void setInitialState( StateInterface* s );

        /**
         * Set the final state of this StateMachine.
         */
        void setFinalState( StateInterface* s );

        /**
         * Retrieve the current state of the state machine. Returns null if
         * the StateMachine is not active.
         */
        StateInterface* currentState() const;

        /**
         * Retrieve the current program in execution. Returns null if
         * the StateMachine is not active or no programs are being run.
         */
        ProgramInterface* currentProgram() const;

        /**
         * Retrieve the initial state of the state machine.
         */
        StateInterface* getInitialState() const {
            return initstate;
        }

        /**
         * Retrieve the final state of the state machine.
         */
        StateInterface* getFinalState() const {
            return finistate;
        }

        /**
         * This was added for extra (non-user visible) initialisation
         * when the StateMachine is activated.
         * @param c The command to execute upon each \a activate.
         * \a c is aggregated by this state machine and deleted in
         * the destructor.
         */
        void setInitCommand( CommandInterface* c)
        {
            initc = c;
        }

        CommandInterface* getInitCommand() const
        {
            return initc;
        }

        /**
         * Get the parent, returns zero if no parent.
         */
        StateMachine* getParent() const
        {
            return _parent;
        }

        void setParent(StateMachine* parent)
        {
            _parent = parent;
        }

        /**
         * Get a list of all child state machines.
         */
        const ChildList& getChildren() const
        {
            return _children;
        }

        void addChild( StateMachine* child ) {
            _children.push_back( child );
        }

        /**
         * This method must be overloaded to get a useful
         * hierarchy.
         */
        const std::string& getName() const {
            return _name;
        }

        /**
         * Returns the current program line in execution,
         * @return 1 if not active
         */
        int getLineNumber() const;

        /**
         * Return the text to which getLineNumber() refers.
         */
        virtual std::string getText() const;

        /**
         * Inspect if the StateMachine is performing a state transition.
         * A transition is in progress if entry, handle or exit programs are executed.
         * @return true if it is executing a program (except run program), false if it
         * is not executing a program OR executing the run program.
         */
        bool inTransition() const;

    protected:
        /**
         * A map keeping track of all States and conditional transitions
         * between two states. Every state of this StateMachine must be]
         * present as a \em key in this map.
         */
        TransitionMap stateMap;

        /**
         * A map keeping track of all preconditions of a state. Not
         * all states need to be present as a \em key in this map.
         */
        PreConditionMap precondMap;

        /**
         * A map keeping track of all events of a state. Not all
         * states need to be present as a \em key in this map.
         */
        EventMap eventMap;

    private:
        void changeState( StateInterface* s, ProgramInterface* tprog, bool stepping = false );

        void leaveState( StateInterface* s );

        void enterState( StateInterface* s );

        void runState( StateInterface* s );

        void handleState( StateInterface* s );

        bool executeProgram(ProgramInterface*& cp, bool stepping);

        int checkConditions( StateInterface* state, bool stepping = false );

        void enableEvents( StateInterface* s );
        void disableEvents( StateInterface* s );

        /**
         * Internal use only. Make a transition to state 'to' with transitionprogram 'p' under condition 'c'.
         */
        void eventTransition( ConditionInterface* c,  ProgramInterface* p, StateInterface* to );

        /**
         * The Initial State.
         */
        StateInterface* initstate;

        /**
         * The Final State.
         */
        StateInterface* finistate;

        /**
         * The current state the Machine is in.
         * current == 0 means that the state machine is currently inactive.
         */
        StateInterface* current;

        /**
         * The next state the Machine will go to.
         */
        StateInterface* next;

        CommandInterface* initc;

        ProgramInterface* currentProg;
        ProgramInterface* currentExit;
        ProgramInterface* currentHandle;
        ProgramInterface* currentEntry;
        ProgramInterface* currentRun;
        ProgramInterface* currentTrans;

        TransList::iterator reqstep;
        TransList::iterator reqend;

        std::pair<PreConditionMap::const_iterator,PreConditionMap::const_iterator> prec_it;
        bool checking_precond;

        bool error;

        int evaluating;
    }; 
}

#endif
