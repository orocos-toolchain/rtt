#include "execution/ConditionBoolProperty.hpp"

namespace ORO_Execution
{
  bool ConditionBoolProperty::evaluate()
  {
    return mprop.get();
  };

  std::string ConditionBoolProperty::toString()
  {
    return std::string( "property " ) + mprop.getName();
  };

  ConditionInterface* ConditionBoolProperty::clone() const
  {
    return new ConditionBoolProperty( mprop );
  };
}

