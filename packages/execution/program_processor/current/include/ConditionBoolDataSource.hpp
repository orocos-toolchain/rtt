#ifndef CONDITIONBOOLDATASOURCE_HPP
#define CONDITIONBOOLDATASOURCE_HPP

#include <corelib/ConditionInterface.hpp>
#include "DataSource.hpp"


namespace ORO_Execution
{

    using ORO_CoreLib::ConditionInterface;

  class ConditionBoolDataSource
    : public ConditionInterface
  {
    DataSource<bool>::shared_ptr data;
  public:
    ConditionBoolDataSource( DataSource<bool>* s )
      : data( s )
      {
      };
    ~ConditionBoolDataSource();
    bool evaluate();
    ConditionInterface* clone() const;
    void reset();
  };
};

#endif
