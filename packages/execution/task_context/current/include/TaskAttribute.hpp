/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TaskAttribute.hpp 

                        TaskAttribute.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ORO_TASKATTRIBUTE_HPP
#define ORO_TASKATTRIBUTE_HPP

#include "DataSource.hpp"
#include <corelib/Property.hpp>
#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;
  using ORO_CoreLib::Property;
  using ORO_CoreLib::PropertyBase;

  struct bad_assignment
  {
  };

  /**
   * We keep defined variables and constants as TaskAttribute's,
   * together with some information about their types etc.  This is
   * the abstract base class, the implementations are in
   * ValueParser.cpp
   */
  class TaskAttributeBase
  {
  public:
    virtual ~TaskAttributeBase();
      /**
       * Return a DataSource which contains the same contents.
       */
    virtual DataSourceBase* toDataSource() const = 0;

    /**
     * Returns a clone of this TaskAttributeBase.
     */
    virtual TaskAttributeBase* clone() const = 0;

    /**
     * Returns a copy of this TaskAttributeBase.  Uses the given
     * replacements to copy held DataSources.
     */
    virtual TaskAttributeBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const = 0;

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

      /**
       * Create an assignment command of the TaskAttributeBase
       * to the rhs DataSource, with an index.
       * example is the left hand side [] operator. Returns zero if
       * not applicable. It can not be used for initialisation.
       */
    virtual CommandInterface* assignIndexCommand( DataSourceBase* index,
                                             DataSourceBase* rhs ) const;
  };

  template<typename T>
  class TaskAttribute
    : public TaskAttributeBase
  {
  public:
    virtual DataSource<T>* toDataSource() const = 0;
  };

  /**
   * An alias is a name that is assigned to a certain compound
   * expression, You could compare it to a 0-ary function,
   * only less powerful.
   * This class is the most basic TaskAttribute implementation, does
   * not allow any assignment, just stores a DataSource<T>, and
   * returns it.  This also makes it useful as the return type of
   * temporary expressions like literal values, and rhs
   * expressions.
   */
  template<typename T>
  class TaskAliasAttribute
    : public TaskAttribute<T>
  {
    typename DataSource<T>::shared_ptr data;
  public:
    TaskAliasAttribute( DataSource<T>* d )
      : data( d )
      {
      }
    DataSource<T>* toDataSource() const
      {
        return data.get();
      }
    TaskAliasAttribute<T>* clone() const
      {
        return new TaskAliasAttribute<T>( data.get() );
      }
    TaskAliasAttribute<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
      {
        return new TaskAliasAttribute<T>( data->copy( replacements ) );
      }
  };


}
#endif
