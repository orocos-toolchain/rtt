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

#include <corelib/Attribute.hpp>
#include "AssignVariableCommand.hpp"
#include "DataSourceAdaptor.hpp"

namespace ORO_Execution
{
    namespace detail
    {
        /**
         * Allows 'index' assignment of a type T.
         */
  template<typename T, typename Index, typename SetType, typename Pred>
  class ParsedIndexVariable
    : public AttributeBase
  {
  protected:
    typename VariableDataSource<T>::shared_ptr data;
  public:
    ParsedIndexVariable()
        : data( new VariableDataSource<T>( ) )
      {
      }
    ParsedIndexVariable( T t)
        : data( new VariableDataSource<T>( t ) )
      {
      }
    ParsedIndexVariable( typename VariableDataSource<T>::shared_ptr d )
      : data( d )
      {
      }
    VariableDataSource<T>* toDataSource() const
      {
        return data.get();
      }

    CommandInterface* assignCommand( DataSourceBase::shared_ptr rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = AdaptDataSource<T>()( r );
        if ( ! t )
          throw bad_assignment();
        return new AssignVariableCommand<T>( data.get(), t );
      }

    CommandInterface* assignIndexCommand( DataSourceBase::shared_ptr index, DataSourceBase::shared_ptr rhs ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSourceBase::shared_ptr i( index );
        DataSource<SetType>* t = AdaptDataSource<SetType>()( r );
        if ( ! t )
          throw bad_assignment();
        DataSource<Index>* ind = AdaptDataSource<Index>()( i );
        if ( ! ind )
          throw bad_assignment();
        return new AssignIndexCommand<T, Index, SetType, Pred>(data.get(), ind ,t );
      }

    ParsedIndexVariable<T, Index, SetType,Pred>* clone() const
      {
        return new ParsedIndexVariable( data );
      }
    ParsedIndexVariable<T, Index, SetType,Pred>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
      {
          if (instantiate ) {
              detail::AttributeDataSource<T>* instds = new detail::AttributeDataSource<T>( data->get() );
              replacements[ data.get() ] = instds;
              return new ParsedIndexVariable( instds );
          }
        return new ParsedIndexVariable( data->copy( replacements ) );
      }
  };

    /**
     * Overload assignCommand to check for container size.
     */
  template<typename T, typename Index, typename SetType, typename Pred>
  struct ParsedIndexContainerVariable
      : public ParsedIndexVariable<T,Index,SetType,Pred>
  {
    ParsedIndexContainerVariable( T t)
        : ParsedIndexVariable<T,Index,SetType,Pred>( t )
      {
          this->data->set().reserve( t.capacity() );
      }
    ParsedIndexContainerVariable( typename VariableDataSource<T>::shared_ptr d )
        : ParsedIndexVariable<T,Index,SetType,Pred>( d )
      {
          this->data->set().reserve( d->get().capacity() );
      }
    CommandInterface* assignCommand( DataSourceBase::shared_ptr rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = AdaptDataSource<T>()( r );
        if ( ! t )
          throw bad_assignment();
        return new AssignContainerCommand<T>( this->data.get(), t );
      }

    ParsedIndexContainerVariable<T, Index, SetType,Pred>* clone() const
      {
        return new ParsedIndexContainerVariable( this->data );
      }
    ParsedIndexContainerVariable<T, Index, SetType,Pred>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements,bool instantiate) 
      {
          if (instantiate ) {
              detail::AttributeDataSource<T>* instds = new detail::AttributeDataSource<T>( this->data->get() );
              replacements[ this->data.get() ] = instds;
              return new ParsedIndexContainerVariable( instds );
          }
        return new ParsedIndexContainerVariable( this->data->copy( replacements ) );
      }
  };

  /**
   * This represents a constant value, does not allow assignment,
   * only initialization.
   * @deprecated by ORO_CoreLib::Constant, exactly the same implementation.
   */
  template<typename T>
  class ParsedConstant
    : public AttributeBase
  {
  public:
    typename ORO_CoreLib::ConstantDataSource<T>::shared_ptr cdata;
    ParsedConstant(T t)
      : cdata( new ORO_CoreLib::ConstantDataSource<T>( t ) )
      {
      }
    ParsedConstant( ORO_CoreLib::ConstantDataSource<T>* d )
      : cdata( d )
      {
      }
    ORO_CoreLib::ConstantDataSource<T>* toDataSource() const
      {
        return cdata.get();
      }
    ParsedConstant<T>* clone() const
      {
        return new ParsedConstant<T>( this->cdata.get() );
      }

    ParsedConstant<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool ) 
      {
          // no copy needed, pass on original
          return new ParsedConstant<T>( this->cdata.get() );
      }
  };
    }
}

#endif
