#ifndef RULESABORTLIST_HPP
#define RULESABORTLIST_HPP


#include "RulesList.hpp"

namespace ORO_Execution
{
	
	/**
	 * This class represents a list of strong rules.
	 * When this list is checked and a rule is found
	 * which is violated, the checking operation will be 
	 * aborted.
	 */
	class RulesAbortList : public RulesList
	{
		public :
		
			RulesAbortList();
        
        	RulesAbortList(Rule* rule);
        
        	virtual ~RulesAbortList()
        	{} 	
		
			/**
		 	 * This operation checks every rule untill
		  	 * a rule is found which is violated.  Then that rule is 
		 	 * executed and the function returns true (if no
		 	 * rule is violated, the function returns false).
		 	 */
			virtual bool check();
	
	};

}

#endif
