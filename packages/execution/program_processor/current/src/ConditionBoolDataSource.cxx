#include "execution/ConditionBoolDataSource.hpp"

namespace ORO_Execution
{
  ConditionBoolDataSource::~ConditionBoolDataSource()
  {
  };
  bool ConditionBoolDataSource::evaluate()
  {
    return data->get();
  }
  ConditionInterface* ConditionBoolDataSource::clone() const
  {
    return new ConditionBoolDataSource( data.get() );
  };
  void ConditionBoolDataSource::reset()
  {
    data->reset();
  }
}

