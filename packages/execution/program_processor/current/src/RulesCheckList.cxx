#include "execution/RulesCheckList.hpp"
#include "execution/Rule.hpp"

namespace ORO_Execution
{

			RulesCheckList::RulesCheckList()
			{
			}
        	
        	RulesCheckList::RulesCheckList(Rule* rule)
        	{
        		addRule(rule);	
        	}
		
			/**
		 	 * This operation checks every rule. 
		 	 * If a rule is valid, that rule will be
		     * executed.
		     */
			bool RulesCheckList::check()
			{
				typedef std::vector<Rule*>::iterator it;
				bool result = false;
				
				
				for ( it ptr = rules->begin(); ptr != rules->end(); ++ptr )
            	{
                	bool b = ( *ptr )->check();
                	result = result || b ;
            	}		
            	
            	return result;				
			}


}

