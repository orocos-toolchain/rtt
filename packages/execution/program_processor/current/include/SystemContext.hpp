/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  SystemContext.hpp 

                        SystemContext.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef SYSTEMCONTEXT_HPP
#define SYSTEMCONTEXT_HPP

#include <corelib/StateContext.hpp>
#include <vector>

namespace ORO_Execution
{

	class SystemStateInterface;

	using namespace ORO_CoreLib;

	/**
	 * This class represents a state context considering
	 * system states for the configuration of an orocos system.
	 *
	 * @see  base class StateContext
	 */
	class SystemContext : public StateContext
	{
		//everything from base class, narrowed down to
		//SystemStateInterfaces instead of StateInterfaces
		public:

			/**
         	 * Create a StateContext instance with an empty initial state.
        	 */
			SystemContext();

			/**
         	 * Create a StateContext instance with a given initial state.
         	 *
         	 * @param s_init
         	 *        The first state which must be entered.
         	 * @post The StateContext is in state <s_init>
         	 */
			SystemContext(SystemStateInterface* s_init);

          virtual ~SystemContext();

            /**
         	 * Enter the initial state of the StateContext.
         	 *
         	 * @param s_init
         	 *        The first state of the StateContext.
         	 * @post  The StateContext has entered and handled <s_init>.
         	 */
         	virtual void initState(SystemStateInterface* s_init);

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
			virtual bool requestState(SystemStateInterface* s_n);

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
            void transitionSet( SystemStateInterface* from, SystemStateInterface* to, ConditionInterface* cnd );

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
            void transitionSet( SystemStateInterface* target, ConditionInterface* cnd );

           	/**
             * Retrieve the current state of the context
             */
            virtual SystemStateInterface* currentState();

        private:
            std::vector<SystemStateInterface*> states;
	};



}

#endif


