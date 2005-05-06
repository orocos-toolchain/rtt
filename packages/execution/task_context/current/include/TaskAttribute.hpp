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

#include <corelib/Property.hpp>
#include <corelib/CommandInterface.hpp>

#include "DataSource.hpp"
#include "AssignVariableCommand.hpp"

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
    virtual TaskAttributeBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) = 0;

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

    namespace detail
    {
        /**
         * A special DataSource only to be used for the TaskAttribute class.
         * It has special copy semantics in comparison to 'local' DataSources,
         * being that, a TaskAttributeDataSource can not be copied, but returns
         * itself as the copy. This is because this DataSource is tied to the
         * task, and must keep pointing to the same data of this TaskAttribute.
         *
         */
        template<typename T>
        class TaskAttributeDataSource
            : public VariableDataSource<T>
        {
        public:
            typedef boost::intrusive_ptr<TaskAttributeDataSource<T> > shared_ptr;

            TaskAttributeDataSource( T data )
                : VariableDataSource<T>( data )
            {
            }

            TaskAttributeDataSource( )
            {
            }

            virtual TaskAttributeDataSource<T>* clone() const
            {
                return new TaskAttributeDataSource<T>( this->get() );
            }

            virtual TaskAttributeDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replace) {
                // if somehow a copy exists, return the copy, otherwise return this (see TaskAttribute copy)
                if ( replace[this] != 0 )
                    return dynamic_cast<TaskAttributeDataSource<T>*>( replace[this] );
              
                replace[this] = this;
                // return this instead of a copy.
                return this;
            }
        };

    }

    /**
     * The Attribute of a TaskContext, which can be stored in an AttributeRepository.
     * It is accessible from the scrip parsers (through its TaskAttributeBase parent).
     * @param T The type of data this attribute holds.
     */
  template<typename T>
  class TaskAttribute
    : public TaskAttributeBase
  {
  public:
    typename AssignableDataSource<T>::shared_ptr data;

    TaskAttribute()
      : data( new detail::TaskAttributeDataSource<T>() )
      {
      }
    TaskAttribute(T t)
      : data( new detail::TaskAttributeDataSource<T>( t ) )
      {
      }
    TaskAttribute( AssignableDataSource<T>* d )
      : data( d )
      {
      }

      /**
       * Get the value of this Attribute.
       */
      T get() const
      {
          return data->get();
      }

      /**
       * Set the value of this Attribute.
       */
      void set( T t )
      {
          data->set(t);
      }

      /**
       * Set the value of this Attribute.
       */
      T& set() {
          return data->set();
      }

    AssignableDataSource<T>* toDataSource() const
      {
        return data.get();
      }
    CommandInterface* assignCommand( DataSourceBase* rhs, bool ) const
      {
        DataSourceBase::shared_ptr r( rhs );
        DataSource<T>* t = dynamic_cast<DataSource<T>*>( r.get() );
        if ( t )
            return new AssignVariableCommand<T>( data.get(), t );
        DataSource< typename AssignableDataSource<T>::const_reference_t >* ct
            = dynamic_cast< DataSource< typename AssignableDataSource<T>::const_reference_t >*>( r.get() );
        if ( ct )
            return new AssignVariableCommand<T, typename AssignableDataSource<T>::const_reference_t >( data.get(), ct );
        throw bad_assignment();
      }
    TaskAttribute<T>* clone() const
      {
        return new TaskAttribute<T>( data.get() );
      }
    TaskAttribute<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements )
      {
          // a bit special... The TaskAttribute's DataSource is designed to be not copyable,
          // but if the copy request is on the TaskAttribute itself, forcefully make a copy
          // of the underlying DataSource. Hence, TaskAttribute must be copied before all
          // other DataSources...
          TaskAttribute<T>* ret = new TaskAttribute<T>( data->get() );
          replacements[ data.get() ] = ret->toDataSource();
          return ret;
      }
  };

    namespace detail {
        /**
         * Member-from-base idiom.
         */
        template<class T>
        struct MemberFromBase {
            MemberFromBase(T t) : data(t) {}
            T data;
        };
    }

#if 0
#warning "does not compile under gcc 3.3 (unimplemented feature, clone() in bases causes it)"
    /**
     * The ORO_CoreLib::Property based Attribute of a TaskContext,
     * which can be stored in an AttributeRepository.  It is
     * accessible from the scrip parsers (through its
     * TaskAttributeBase parent), and from the ORO_CoreLib Property
     * system.
     * @param T The type of data this attribute holds.
     */
  template<typename T>
  class TaskProperty
      : public TaskAttributeBase,
        public Property<T>
  {
  public:
    typename ORO_CoreLib::detail::PropertyDataSource<T>::shared_ptr data;

    TaskProperty(const std::string& name, const std::string& description)
        :  Property<T>(name, description ),
           data( new ORO_CoreLib::detail::PropertyDataSource<T>(this) )
      {
      }
    TaskProperty(const std::string& name, const std::string& description, T t)
        :  Property<T>(name, description, t ),
           data( new ORO_CoreLib::detail::PropertyDataSource<T>( this ) )
      {
      }
      /*
    TaskProperty(const std::string& name, const std::string& description, ORO_CoreLib::detail::PropertyDataSource<T>* d )
        :  Property<T>(name, description ),
           : data( d )
      {
      }
      */
      AssignableDataSource<T>* toDataSource() const
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
      }
    TaskProperty<T>* clone() const
      {
        return new TaskProperty<T>( data.get() );
      }
    TaskProperty<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements )
      {
          // a bit special... The TaskProperty's DataSource is designed to be not copyable,
          // but if the copy request is on the TaskProperty itself, forcefully make a copy
          // of the underlying DataSource. Hence, TaskProperty must be copied before all
          // other DataSources...
          TaskProperty<T>* ret = new TaskProperty<T>( data->get() );
          replacements[ data.get() ] = ret->toDataSource();
          return ret;
      }
  };
#endif 

    /**
     * As opposed to a TaskAttribute, a TaskConstant can not be assigned to a new value
     * after creation.
     */
  template<typename T>
  class TaskConstant
    : public TaskAttributeBase
  {
  public:
    typename detail::TaskAttributeDataSource<T>::shared_ptr data;

    TaskConstant()
      : data( new detail::TaskAttributeDataSource<T>() )
      {
      }
    TaskConstant(T t)
      : data( new detail::TaskAttributeDataSource<T>( t ) )
      {
      }
    TaskConstant( detail::TaskAttributeDataSource<T>* d )
      : data( d )
      {
      }

      /**
       * Get the value of this Constant.
       */
      T get() const
      {
          return data->get();
      }

    DataSource<T>* toDataSource() const
      {
        return data.get();
      }
    TaskConstant<T>* clone() const
      {
        return new TaskConstant<T>( data.get() );
      }
    TaskConstant<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) 
      {
          // a bit special... The TaskConstant's DataSource is designed to be not copyable,
          // but if the copy request is on the TaskConstant itself, forcefully make a copy
          // of the underlying DataSource. Hence, TaskConstant must be copied before all
          // other DataSources...
          TaskConstant<T>* ret = new TaskConstant<T>( data->get() );
          replacements[ data.get() ] = ret->toDataSource();
          return ret;
      }
  };

    namespace detail
    {
        /**
         * This class is the most basic TaskAttribute implementation
         * (only suitable for reading a DataSource), does
         * not allow any assignment, just stores a DataSource<T>, and
         * returns it.  This also makes it useful as an alias of
         * temporary expressions like literal values, and rhs
         * expressions.
         */
        template<typename T>
        class ParsedAlias
            : public TaskAttributeBase
        {
            typename DataSource<T>::shared_ptr data;
        public:
            ParsedAlias( DataSource<T>* d )
                : data( d )
            {
            }

            DataSource<T>* toDataSource() const
            {
                return data.get();
            }
            ParsedAlias<T>* clone() const
            {
                return new ParsedAlias<T>( data.get() );
            }
            ParsedAlias<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements )
            {
                return new ParsedAlias<T>( data->copy( replacements ) );
            }
        };
    }
}
#endif
