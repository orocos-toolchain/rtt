/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  ParsedValue.hpp 

                        ParsedValue.hpp -  description
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
 
 
#ifndef PARSEDVALUE_HPP
#define PARSEDVALUE_HPP

#include "DataSource.hpp"
#include <corelib/Property.hpp>
#include "AssignVariableCommand.hpp"

namespace ORO_Execution
{
  using ORO_CoreLib::Property;
  using ORO_CoreLib::PropertyBase;

  struct bad_assignment
  {
  };

  /**
   * We keep defined variables and constants as ParsedValue's,
   * together with some information about their types etc.  This is
   * the abstract base class, the implementations are in
   * ValueORO_Execution.cpp
   */
  class ParsedValueBase
  {
  public:
    virtual ~ParsedValueBase();
    // construct a new ParsedValueBase, containing a
    // VariableDataSource of the same type as this one..
    virtual DataSourceBase* toDataSource() const = 0;
    /**
     * return a command assigning rhs to the DataSource you're
     * holding..  The default implementation returns 0, indicating
     * this DataSource cannot be assigned to..  Throw a
     * bad_assignment if rhs is of the wrong type..
     *
     * If initialization is true, then this is the initialization of
     * the value.  E.g. ConstantValue needs to know this because it
     * does allow one assignment at initialization time, but no
     * further assignments..
     */
    virtual CommandInterface* assignCommand( DataSourceBase* rhs,
                                             bool initialization ) const;
  };

  template<typename T>
  class ParsedValue
    : public ParsedValueBase
  {
  public:
    virtual DataSource<T>* toDataSource() const = 0;
  };

  /**
   * An alias is a name that is assigned to a certain compound
   * expression, I guess you could compare it to a 0-ary function,
   * only less powerful..   Maybe this can be extended to allow for
   * functions with arguments..
   * This class is the most basic ParsedValue implementation, does
   * not allow any assignment, just stores a DataSource<T>, and
   * returns it..  This also makes it useful as the return type of
   * temporary expressions like literal values, and rhs
   * expressions..
   */
  template<typename T>
  class ParsedAliasValue
    : public ParsedValue<T>
  {
    typename DataSource<T>::shared_ptr data;
  public:
    ParsedAliasValue( DataSource<T>* d )
      : data( d )
      {
      };
    DataSource<T>* toDataSource() const
      {
        return data.get();
      };
    CommandInterface* assignCommand( DataSourceBase*, bool ) const
      {
        return 0;
      };
  };

  /**
   * This class represents a variable held in ValueORO_Execution..  It is
   * the only ParsedValue that does something useful in its
   * assignCommand() method..
   */
  template<typename T>
  class ParsedVariableValue
    : public ParsedValue<T>
  {
  public:
    typename VariableDataSource<T>::shared_ptr data;
    ParsedVariableValue()
      : data( new VariableDataSource<T>( T() ) )
      {
      };
    VariableDataSource<T>* toDataSource() const
      {
        return data.get();
      };
    CommandInterface* assignCommand( DataSourceBase* rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = dynamic_cast<DataSource<T>*>( r.get() );
        if ( ! t )
          throw bad_assignment();
        return new AssignVariableCommand<T>( data.get(), t );
      };
  };

  /**
   * This represents a constant value, does not allow assignment,
   * only initialization..
   */
  template<typename T>
  class ParsedConstantValue
    : public ParsedVariableValue<T>
  {
  public:
    ParsedConstantValue()
      : ParsedVariableValue<T>()
      {
      };
    CommandInterface* assignCommand( DataSourceBase* rhs, bool init ) const
      {
        if ( init )
          return ParsedVariableValue<T>::assignCommand( rhs, init );
        else return 0;
      };
  };
}

#endif
