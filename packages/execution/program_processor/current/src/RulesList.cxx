#include "execution/RulesList.hpp"

#include "execution/Rule.hpp"

namespace ORO_Execution
{

	RulesList::RulesList()
		:rules(new std::vector<Rule*>)
	{}


    RulesList::~RulesList()
    {
      typedef std::vector<Rule*>::iterator it;
      for ( it ptr = rules->begin(); ptr != rules->end(); ++ptr )
      {
        delete *ptr;
      }
      delete rules;
    }

	void RulesList::addRule(Rule* rule)
	{
		rules->push_back(rule);
	}

}

