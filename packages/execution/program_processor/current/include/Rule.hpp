/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Rule.hpp 

                        Rule.hpp -  description
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
 
 
#ifndef RULE_HPP
#define RULE_HPP


namespace ORO_CoreLib
{
	class ConditionInterface;
	class CommandInterface;
}

namespace ORO_Execution
{

	using namespace ORO_CoreLib;


	/**
	 * This class represents a condition-action couple
	 * representing an if-then clause.
	 */
	class Rule
	{

		public:

			/**
			 * Construct a rule with a given condition and
			 * command.
                         * The rule takes ownership of both the
                         * condition and the command..
			 *
			 * @post The condition of this rule is set to the given condition.
			 * @post The action of this rule is set to the given command.
			 */
			Rule(ConditionInterface* cond, CommandInterface* cmd);

			virtual ~Rule();

			/**
			 * Return the condition coupled to this rule.
			 */
			//virtual ConditionInterface* getCondition();

			/**
			 * Checks this rule, evaluates the condition and
			 * if true, executes the action.
			 *
			 * @return True, if condition is true
			 */
			virtual bool check();

		protected:

			virtual void setCondition(ConditionInterface* cond);
			virtual void setAction(CommandInterface* cmd);

		private:
			/**
			 * Action to be performed when condition is fulfilled.
			 */
			CommandInterface* action;

			/**
			 * Condition to be fulfilled.
			 */
			ConditionInterface* condition;

	};

}

#endif
