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
