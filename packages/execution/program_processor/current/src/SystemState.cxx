#include "execution/SystemState.hpp"

namespace ORO_Execution
{
	using namespace std;
	
    SystemState::SystemState()
    	:checkList(new RulesCheckList),abortList(new RulesAbortList),onExitCmd(0),onEntryCmd(0)
    {
    }

	SystemState::~SystemState()
	{
		delete checkList;
		delete abortList;
		delete onExitCmd;
		delete onEntryCmd;
	}

	void SystemState::handle()
	{
		checkRules();		
	}
	
	void SystemState::onEntry()
	{
		if (onEntryCmd != 0) {onEntryCmd->execute();}
	}
	void SystemState::onExit()
	{
		if(onExitCmd != 0) {onExitCmd->execute();}
	}
	
	void SystemState::addWeakRule(Rule* r)
	{
		checkList->addRule(r);
	}
	
	void SystemState::addStrongRule(Rule* r)
	{
		abortList->addRule(r);		
	}
		
	void SystemState::checkRules()
	{
		//logisch gezien zouden eerst de stoute regeltjes
		//getest moeten worden, dan de brave
		bool violated = abortList->check();
		if (!violated) { checkList->check();}			
	}
	
	void SystemState::setEntryCmd(CommandInterface* c)
	{
		onEntryCmd = c;
	}
			 
	void SystemState::setExitCmd(CommandInterface* c)
	{
		onExitCmd = c;
	}

}

