#include "execution/ConditionComposite.hpp"

namespace ORO_Execution
{
  bool ConditionCompositeNOT::evaluate()
  {
    return ! cond->evaluate();
  }

  ConditionCompositeNOT::~ConditionCompositeNOT()
  {

  }

  void ConditionCompositeNOT::reset()
  {
    cond->reset();
  };

  ConditionInterface* ConditionCompositeNOT::clone() const
  {
    return new ConditionCompositeNOT( cond->clone() );
  };
}

