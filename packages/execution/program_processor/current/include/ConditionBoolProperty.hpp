#ifndef CONDITIONBOOLPROPERTY_HPP
#define CONDITIONBOOLPROPERTY_HPP

#include <corelib/ConditionInterface.hpp>
#include <corelib/Property.hpp>

using ORO_CoreLib::ConditionInterface;
using ORO_CoreLib::Property;

namespace ORO_Execution
{
  /**
   * ConditionBoolProperty is a Condition which reads out a
   * Property<bool>.
   */
  class ConditionBoolProperty
    : public ConditionInterface
  {
    const Property<bool>& mprop;
  public:
    ConditionBoolProperty( const Property<bool>& prop )
      : mprop( prop )
      {
      };
    bool evaluate();
    ConditionInterface* clone() const;
    std::string toString();
  };
}

#endif
