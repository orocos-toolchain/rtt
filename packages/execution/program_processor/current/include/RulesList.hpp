#ifndef RULESLIST_HPP
#define RULESLIST_HPP

#include <vector>

namespace ORO_Execution
{

	class Rule;

	/**
	 * This abstract class represents a list containing rules.
	 */
	class RulesList
	{

		public:

        	virtual ~RulesList();

			/**
			 * Add given rule to this list.  The list
			 * takes ownership of the rule.
			 */
			virtual void addRule(Rule* rule);

			/**
			 * Check the rules in this list.
			 *
			 * @return True, if there is a rule which is checked to be true.
			 */
			virtual bool check()=0;

		protected:
			//only for derived classes
			RulesList();

		  	std::vector<Rule*>* rules;

	};

}

#endif


