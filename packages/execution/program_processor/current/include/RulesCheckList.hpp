#ifndef RULESCHECKLIST_HPP
#define RULESCHECKLIST_HPP


#include "RulesList.hpp"

namespace ORO_Execution
{
	/**
	 * This class represents a list of weak rules.
	 * When this list is checked, every rule which 
	 * is true will be executed.
	 */
	class RulesCheckList : public RulesList
	{
		public :
		
			RulesCheckList();
        	RulesCheckList(Rule* rule);
        
        	virtual ~RulesCheckList()
        	{} 	
		
			/**
		 	 * This operation checks every rule. 
		 	 * If a rule is valid, that rule will be
		     * executed.
		     */
			virtual bool check();
	
	};

}

#endif
