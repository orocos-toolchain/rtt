#ifndef DATASOURCECONDITION_HPP
#define DATASOURCECONDITION_HPP

#include "DataSource.hpp"

namespace ORO_CoreLib
{
  class ConditionInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::ConditionInterface;

  /**
   * a class that wraps a Condition in a DataSource<bool>
   * interface..
   */
  class DataSourceCondition
    : public DataSource<bool>
  {
    ConditionInterface* cond;
  public:
    /**
     * Constructor.
     * DataSourceCondition takes ownership of the condition you pass
     * it..
     */
      DataSourceCondition( ConditionInterface* c );
      DataSourceCondition( const DataSourceCondition& orig );
      ~DataSourceCondition();
      bool get() const;
      void reset();
      ConditionInterface* condition() const;
      virtual DataSource<bool>* clone() const;
  };
}

#endif
