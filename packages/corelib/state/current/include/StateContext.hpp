/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateContext.hpp 

                        StateContext.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
    ***************************************************************************
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    ***************************************************************************/
 
 
#ifndef STATECONTEXT_HPP
#define STATECONTEXT_HPP

#include "StateInterface.hpp"
#include "ConditionInterface.hpp"

#include <map>

namespace ORO_CoreLib
{
	using  std::map;
    /**
     * A StateContext keeps track of the current StateInterface and all
     * transitions from this StateInterface to another. One can request
     * a transition from one StateInterface to another which will fail
     * or succeed depending on a previously set condition. 
     *
     * By default, any state transition fails.
     *
     * @note It should be noted that a state transition
     *       from and to the current state is not disallowed in the 
     *       "from Any to X-State" case. The user should check herself with
     *       currentState() when it should not occur.
     *
     * @note A more efficient implementation might be needed
     *       for the case this->requestState( this->nextState() );
     */
    class StateContext
    {
        typedef map< StateInterface*, std::pair<ConditionInterface*, StateInterface*> >
        TransitionMap;
        typedef map< StateInterface*, ConditionInterface*>
        TransitionAnyMap;

    public:

        /**
         * Create a StateContext instance with an empty initial state.
         */
        StateContext();

        /**
         * Create a StateContext instance with a given initial state.
         *
         * @param s_init
         *        The first state which must be entered.
         * @post The StateContext is in state <s_init>
         */
        StateContext( StateInterface* s_init );

        /**
         * Enter the initial state of the StateContext.
         *
         * @param s_init
         *        The first state of the StateContext.
         * @post  The StateContext has entered and handled <s_init>.
         */
        void initState( StateInterface* s_init );

        /**
         * Search from the current state a candidate next state.
         * If none is found, the current state is taken.
         * Next, handle the resulting state.
         *
         * This call is equivalent to
         * this->requestState( this->nextState() ), but more
         * efficient.
         *
         * @return The current state.
         */
        StateInterface* requestNextState();

        /**
         * Search from the current state a candidate next state.
         * If none is found, the current state is returned.
         */
        StateInterface* nextState();

        /**
         * Request a state transition to a new state.
         * If the transition is not set by transitionSet(), acquiering
         * the state will fail.
         * 
         * @param  s_n
         *         The state to change to
         * @return true 
         *          if the transition is successfull
         *         false
         *          if the transition is not allowed
         */
        bool requestState( StateInterface * s_n );

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
         * @post  All transitions from <from> to <to> will succeed under
         *        condition <cnd>
         */
        void transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd );

        /**
         * Express a possible transition from any state (including <target> )
         * to a specified one  under a certain condition.
         *
         * @param target
         *        The state which can be entered
         * @param cnd
         *        The Condition under which the transition may succeed
         * @post  All transitions to <target> will succeed under
         *        condition <cnd>
         */
        void transitionSet( StateInterface* target, ConditionInterface* cnd );

        /**
         * Retrieve the current state of the context
         */
        StateInterface* currentState();

    private:
        void leaveState( StateInterface* s );

        void enterState( StateInterface* s );

        /**
         * The current state the Context is in
         */
        StateInterface* current;

        /**
         * A map keeping track of all conditional transitions
         * between two states
         */
        TransitionMap stateMap;

        /**
         * A map keeping track of all conditional transitions
         * to a specific state
         */
        TransitionAnyMap stateAnyMap;
    };
}

#endif

