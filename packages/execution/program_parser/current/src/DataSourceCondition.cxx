#include "execution/DataSourceCondition.hpp"

#include "corelib/ConditionInterface.hpp"

namespace ORO_Execution
{
  DataSourceCondition::DataSourceCondition( ConditionInterface* c )
    : cond( c )
  {
  }

  DataSourceCondition::~DataSourceCondition()
  {
    delete cond;
  }

  bool DataSourceCondition::get() const
  {
    return cond->evaluate();
  }

  void DataSourceCondition::reset()
  {
    cond->reset();
  }
}
