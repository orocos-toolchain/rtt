/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  Operators.hpp 

                        Operators.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef ORO_OPERATORS_HPP
#define ORO_OPERATORS_HPP

#include <string>
#include <vector>
#include "DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT { namespace detail
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

  class SixaryOp
  {
  public:
    virtual ~SixaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument DataSource's are of the correct type, then return an
     * appropriate DataSource ( i.e. a DataSource that will apply a
     * certain operation on the values it gets from its argument
     * DataSources, and will return that value ).
     * Otherwise, return 0.
     */
    virtual DataSourceBase* build(
      const std::string& op,
      DataSourceBase* a, DataSourceBase* b, DataSourceBase* c,
      DataSourceBase* d, DataSourceBase* e, DataSourceBase* f) = 0;
  };

  /**
   * The Dot Operator allows access to members of composite types, 
   * such as in frame.pos.x .
   * This is used if a dot was parsed on a value type. It is different by semantics of the
   * UnaryOp because the dot is parsed as a binary operator but 
   * actually is a unary ( hard to explain, look at the code...)
   */
  class DotOp
  {
  public:
    virtual ~DotOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument DataSource is of the correct type, then return an
     * appropriate DataSource. ( i.e. a DataSource that will apply a
     * certain operation on the value it gets from its argument
     * DataSource, and will return that value ).  Otherwise, return 0.
     */
    virtual DataSourceBase* build( const std::string& member,
                                   DataSourceBase* a ) = 0;
  };
}
    /**
     * This class builds on upon construction all expression
     * operators known to Orocos. Mainly used for scripting.
     */
  class OperatorRepository
  {
    std::vector<detail::DotOp*> dotops;
    std::vector<detail::UnaryOp*> unaryops;
    std::vector<detail::BinaryOp*> binaryops;
    std::vector<detail::TernaryOp*> ternaryops;
    std::vector<detail::SixaryOp*> sixaryops;
    OperatorRepository();
    OperatorRepository( const OperatorRepository& );

  public:
    void add( detail::UnaryOp* o );
    void add( detail::DotOp* o );
    void add( detail::BinaryOp* o );
    void add( detail::TernaryOp* o );
    void add( detail::SixaryOp* o );

      typedef boost::shared_ptr<OperatorRepository> shared_ptr;
    ~OperatorRepository();
    static shared_ptr Instance();
    DataSourceBase* applyDot( const std::string& member, DataSourceBase* value );
    DataSourceBase* applyUnary( const std::string& op, DataSourceBase* a );
    DataSourceBase* applyBinary(
      const std::string& op, DataSourceBase* a, DataSourceBase* b );
    DataSourceBase* applyTernary(
      const std::string& op, DataSourceBase* a, DataSourceBase* b,
      DataSourceBase* c );
    DataSourceBase* applySixary(
      const std::string& op,
      DataSourceBase* a, DataSourceBase* b, DataSourceBase* c,
      DataSourceBase* d, DataSourceBase* e, DataSourceBase* f);
  };
}

#endif
