#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "SystemStateInterface.hpp"
#include "Rule.hpp"
#include "RulesCheckList.hpp"
#include "RulesAbortList.hpp"


namespace ORO_Execution
{

	/**
	 * This class represents system states which
	 * are part of an orocos system configuration.
	 * 
	 * @see  base class SystemStateInterface
	 */
	class SystemState : public SystemStateInterface
	{
	
		public:
			
			/**
			 * Constructs an empty system state. 
			 */
			SystemState();
			
			virtual ~SystemState();
			
			
			/*** from StateInterface ***/
			virtual void handle();
			virtual void onEntry();
			virtual void onExit();
			
			
			
			/*** from SystemStateInterface ***/
			virtual void addStrongRule(Rule* r);
			virtual void addWeakRule(Rule* r);
			virtual void checkRules();	
			virtual void setEntryCmd(CommandInterface* c);
			virtual void setExitCmd(CommandInterface* c);		
			
		
		private:
			
			RulesCheckList* checkList;
			RulesAbortList* abortList;
			
			//voorlopig 'rudimentaire' ondersteuning voor configuratie
			CommandInterface* onExitCmd;
			CommandInterface* onEntryCmd;
	};
}


#endif

