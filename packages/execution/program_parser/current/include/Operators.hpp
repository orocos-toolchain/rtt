/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  Operators.hpp 

                        Operators.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
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
