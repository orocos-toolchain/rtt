#include "execution/RulesAbortList.hpp"
#include "execution/Rule.hpp"

namespace ORO_Execution
{
	
			RulesAbortList::RulesAbortList()
			{
			}
        	
        	RulesAbortList::RulesAbortList(Rule* rule)
        	{
        		//rules->push_back(rule);
        		addRule(rule);	
        	}
		
			
			bool RulesAbortList::check()
			{
				typedef std::vector<Rule*>::iterator it;
				bool done = false;
				
				it ptr = rules->begin();
				
				while (!done && ptr !=rules->end() )
				{
					done = (*ptr)->check();
					++ptr;
				}
				
				return done;
			
			}


}

