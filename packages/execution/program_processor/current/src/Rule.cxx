#include "execution/Rule.hpp"
#include <corelib/ConditionInterface.hpp>
#include <corelib/CommandInterface.hpp>

using namespace ORO_CoreLib;

namespace ORO_Execution
{

	Rule::Rule(ConditionInterface* cond, CommandInterface* cmd)
	{
		setCondition(cond);
		setAction(cmd);
	}

	Rule::~Rule()
	{
                delete condition;
                delete action;
	}

	bool Rule::check()
	{
		bool result = condition->evaluate();
		if (result) { action->execute(); }
		return result;
	}

	void Rule::setCondition(ConditionInterface* cond)
	{
		condition = cond;
	}

	void Rule::setAction(CommandInterface* cmd)
	{
		action = cmd;
	}

}
