#ifndef CONDITIONCOMPARE_HPP
#define CONDITIONCOMPARE_HPP

#include <corelib/ConditionInterface.hpp>
#include <typeinfo>

#include "DataSource.hpp"

namespace ORO_Execution
{
  using ORO_CoreLib::ConditionInterface;
  /**
   * A general compare condition.  This compares two variables of type
   * T, which it gets from two DataSource<T>, using the
   * compare_op given.  You should use std::less, std::less_equal,
   * std::greater, std::greater_equal, std::equal_to and
   * std::not_equal_to or other binary predicate functors as
   * compare_op..
   */
  template<typename T, typename compare_op>
  class ConditionCompare
    : public ConditionInterface
  {
    typename DataSource<T>::shared_ptr mdata1;
    typename DataSource<T>::shared_ptr mdata2;
  public:
    ConditionCompare( DataSource<T>* data1, DataSource<T>* data2 )
      : mdata1( data1 ), mdata2( data2 )
      {
      }

    bool evaluate();
    ConditionInterface* copy() const
      {
        return new ConditionCompare<T, compare_op>(
          mdata1.get(), mdata2.get() );
      };
    std::string toString()
      {
        return std::string( "compare<" ) + typeid( T ).name() +
          ", " + typeid( compare_op ).name() + ">";
      };
  };

  template<typename T, typename compare_op>
  bool ConditionCompare<T, compare_op>::evaluate()
  {
    compare_op op;
    return op( mdata1->get(), mdata2->get() );
  };
}

#endif
