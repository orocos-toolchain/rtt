#include "execution/DataSourceCondition.hpp"

#include "corelib/ConditionInterface.hpp"

namespace ORO_Execution
{
  DataSourceCondition::DataSourceCondition( ConditionInterface* c )
    : cond( c )
  {
  }

  DataSourceCondition::DataSourceCondition( const DataSourceCondition& orig )
    : cond( orig.condition()->clone() )
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

  ConditionInterface* DataSourceCondition::condition() const
  {
      return cond;
  }

  void DataSourceCondition::reset()
  {
    cond->reset();
  }

  DataSource<bool>* DataSourceCondition::clone() const
  {
      return new DataSourceCondition( cond->clone() );
  }
}
