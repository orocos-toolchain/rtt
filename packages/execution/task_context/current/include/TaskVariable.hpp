/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  TaskVariable.hpp

                        TaskVariable.hpp -  description
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

#ifndef ORO_TASK_VARIABLE_HPP
#define ORO_TASK_VARIABLE_HPP

#include "TaskAttribute.hpp"
#include "AssignVariableCommand.hpp"

namespace ORO_Execution
{
  /**
   * This class represents a variable held in ValueParser..  It is
   * the only TaskAttribute that does something useful in its
   * assignCommand() method..
   */
  template<typename T>
  class TaskVariable
    : public TaskAttribute<T>
  {
  public:
    typename VariableDataSource<T>::shared_ptr data;
    TaskVariable()
      : data( new VariableDataSource<T>() )
      {
      }
    TaskVariable(T t)
      : data( new VariableDataSource<T>( t ) )
      {
      }
    TaskVariable( typename VariableDataSource<T>::shared_ptr d )
      : data( d )
      {
      }
    VariableDataSource<T>* toDataSource() const
      {
        return data.get();
      }
    CommandInterface* assignCommand( DataSourceBase* rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = dynamic_cast<DataSource<T>*>( r.get() );
        if ( ! t ) {
            throw bad_assignment();
        }
        return new AssignVariableCommand<T>( data.get(), t );
      };
    TaskVariable<T>* clone() const
      {
        return new TaskVariable<T>( data );
      }
    TaskVariable<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
      {
        return new TaskVariable<T>( data->copy( replacements ) );
      }
  };

  template<typename T, typename Index, typename SetType, typename Pred>
  class TaskIndexVariable
    : public TaskAttribute<T>
  {
  protected:
    typename VariableDataSource<T>::shared_ptr data;
  public:
    TaskIndexVariable()
        : data( new VariableDataSource<T>( ) )
      {
      }
    TaskIndexVariable( T t)
        : data( new VariableDataSource<T>( t ) )
      {
      }
    TaskIndexVariable( typename VariableDataSource<T>::shared_ptr d )
      : data( d )
      {
      }
    VariableDataSource<T>* toDataSource() const
      {
        return data.get();
      }

    CommandInterface* assignCommand( DataSourceBase* rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = dynamic_cast<DataSource<T>*>( r.get() );
        if ( ! t )
          throw bad_assignment();
        return new AssignVariableCommand<T>( data.get(), t );
      }

    CommandInterface* assignIndexCommand( DataSourceBase* index, DataSourceBase* rhs ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSourceBase::shared_ptr i( index );
        DataSource<SetType>* t = dynamic_cast<DataSource<SetType>*>( r.get() );
        if ( ! t )
          throw bad_assignment();
        DataSource<Index>* ind = dynamic_cast<DataSource<Index>*>( i.get() );
        if ( ! ind )
          throw bad_assignment();
        return new AssignIndexCommand<T, Index, SetType, Pred>(data.get(), ind ,t );
      }

    TaskIndexVariable<T, Index, SetType,Pred>* clone() const
      {
        return new TaskIndexVariable( data );
      }
    TaskIndexVariable<T, Index, SetType,Pred>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
      {
        return new TaskIndexVariable( data->copy( replacements ) );
      }
  };

    /**
     * Overload assignCommand to check for container size.
     */
  template<typename T, typename Index, typename SetType, typename Pred>
  struct TaskIndexContainerVariable
      : public TaskIndexVariable<T,Index,SetType,Pred>
  {
    TaskIndexContainerVariable( T t)
        : TaskIndexVariable<T,Index,SetType,Pred>( t )
      {
      }
    TaskIndexContainerVariable( typename VariableDataSource<T>::shared_ptr d )
        : TaskIndexVariable<T,Index,SetType,Pred>( d )
      {
      }
    CommandInterface* assignCommand( DataSourceBase* rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = dynamic_cast<DataSource<T>*>( r.get() );
        if ( ! t )
          throw bad_assignment();
        return new AssignContainerCommand<T>( data.get(), t );
      }
  };



  /**
   * This represents a constant value, does not allow assignment,
   * only initialization.
   * It inherits from TaskVariable, purely for implementation reuse,
   * not for semantic correctness :-).
   */
  template<typename T>
  class TaskConstant
    : public TaskVariable<T>
  {
  public:
    TaskConstant()
      : TaskVariable<T>()
      {
      }
    TaskConstant(T t)
      : TaskVariable<T>(t)
      {
      }
    TaskConstant( typename VariableDataSource<T>::shared_ptr d )
      : TaskVariable<T>( d )
      {
      }
    CommandInterface* assignCommand( DataSourceBase* rhs, bool init ) const
      {
        if ( init )
          return TaskVariable<T>::assignCommand( rhs, init );
        else return 0;
      }
    TaskConstant<T>* clone() const
      {
        return new TaskConstant<T>( this->data );
      }
    TaskConstant<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
      {
        return new TaskConstant<T>( this->data->copy( replacements ) );
      }
  };
}

#endif
