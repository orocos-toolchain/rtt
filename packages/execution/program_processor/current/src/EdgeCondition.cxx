#include "execution/EdgeCondition.hpp"

#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution
{
	using namespace ORO_CoreLib;

	EdgeCondition::EdgeCondition(ConditionInterface* cond )
        : condition( cond )
	{
	}

	EdgeCondition::~EdgeCondition()
	{
		delete condition;
    }

    void EdgeCondition::setCondition(ConditionInterface* cond)
	{
		condition = cond;
	}

	bool EdgeCondition::evaluate()
	{
		return condition->evaluate();
	}

	ConditionInterface* EdgeCondition::getCondition() const
	{
		return condition;
	}

    void EdgeCondition::reset()
    {
        condition->reset();
    }



}

