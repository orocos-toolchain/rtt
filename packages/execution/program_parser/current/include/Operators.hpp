#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <string>
#include <vector>
#include "DataSource.hpp"

namespace ORO_Execution
{

  /**
   * \file this file contains operator information objects,
   * These objects contain information on how to apply one operator on
   * a set of arguments, and contain a method that does it..  This is
   * mostly done to allow me to work with template stuff to save me a
   * lot of typing time ;)
   */

  /**
   * UnaryOperator contains information on some unary operator that
   * operates on one arg of a certain type.  Examples of unary
   * operators are logical not for booleans, unary minus for ints,
   * unary minus for doubles, unary minus for vectors, unary plus for
   * ints (identity operator) etc.  We always provide templates that
   * accept a STL style adaptable functor..
   */
  class UnaryOp
  {
  public:
    virtual ~UnaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument DataSource is of the correct type, then return an
     * appropriate DataSource. ( i.e. a DataSource that will apply a
     * certain operation on the value it gets from its argument
     * DataSource, and will return that value ).  Otherwise, return 0.
     */
    virtual DataSourceBase* build( const std::string& op,
                                   DataSourceBase* a ) = 0;
  };

  class BinaryOp
  {
  public:
    virtual ~BinaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument DataSource's are of the correct type, then return an
     * appropriate DataSource ( i.e. a DataSource that will apply a
     * certain operation on the values it gets from its argument
     * DataSources, and will return that value ).
     * Otherwise, return 0.
     */
    virtual DataSourceBase* build(
      const std::string& op, DataSourceBase* a, DataSourceBase* b ) = 0;
  };

  class TernaryOp
  {
  public:
    virtual ~TernaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument DataSource's are of the correct type, then return an
     * appropriate DataSource ( i.e. a DataSource that will apply a
     * certain operation on the values it gets from its argument
     * DataSources, and will return that value ).
     * Otherwise, return 0.
     */
    virtual DataSourceBase* build(
      const std::string& op, DataSourceBase* a, DataSourceBase* b,
      DataSourceBase* c ) = 0;
  };

  class OperatorRegistry
  {
    std::vector<UnaryOp*> unaryops;
    std::vector<BinaryOp*> binaryops;
    std::vector<TernaryOp*> ternaryops;
    OperatorRegistry();
    OperatorRegistry( const OperatorRegistry& );
    ~OperatorRegistry();
    void add( UnaryOp* o );
    void add( BinaryOp* o );
    void add( TernaryOp* o );
  public:
    static OperatorRegistry& instance();
    DataSourceBase* applyUnary( const std::string& op, DataSourceBase* a );
    DataSourceBase* applyBinary(
      const std::string& op, DataSourceBase* a, DataSourceBase* b );
    DataSourceBase* applyTernary(
      const std::string& op, DataSourceBase* a, DataSourceBase* b,
      DataSourceBase* c );
  };
}

#endif
