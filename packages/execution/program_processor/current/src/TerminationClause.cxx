#include "execution/TerminationClause.hpp"

#include <corelib/ConditionInterface.hpp>
#include "execution/ProgramNode.hpp"


namespace ORO_Execution
{
	using namespace ORO_CoreLib;

	TerminationClause::TerminationClause(ConditionInterface* cond, ProgramNode* node)
	{
		setCondition(cond);
		setProgramNode(node);
	}


	TerminationClause::~TerminationClause()
	{
		delete condition;
        }


	ProgramNode* TerminationClause::getProgramNode()
	{
		return programNode.get();
	}

	void TerminationClause::setProgramNode(ProgramNode* node)
	{
                programNode = node;
	}

    void TerminationClause::setCondition(ConditionInterface* cond)
	{
		condition = cond;
	}

	bool TerminationClause::evaluate()
	{
		bool result = condition->evaluate();
		return result;
	}

	ConditionInterface* TerminationClause::getCondition()
	{
		return condition;
	}

        void TerminationClause::reset()
        {
                condition->reset();
        }



}

