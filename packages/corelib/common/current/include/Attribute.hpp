/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  Attribute.hpp 

                        Attribute.hpp -  description
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
 
 
#ifndef ORO_CORELIB_ATTRIBUTE_HPP
#define ORO_CORELIB_ATTRIBUTE_HPP

#include "DataSource.hpp"
#include "DataSources.hpp"
#include "AttributeBase.hpp"
#include "BuildType.hpp"

namespace RTT
{
    /**
     * The Attribute contains data which can be set and get.
     * @param T The type of data this attribute holds.
     */
    template<typename T>
    class Attribute
        : public AttributeBase
    {
        typename AssignableDataSource<T>::shared_ptr data;
    public:

        /**
         * Create an Attribute with a default value.
         */
        Attribute()
            : data( detail::BuildType<T>::Value( T() ) )
        {
        }

        /**
         * Create an Attribute with a value \a t.
         */
        explicit Attribute(T t)
            : data( detail::BuildType<T>::Value( t ) )
        {
        }

        /**
         * Create an Attribute which uses a DataSource \a d.
         */
        Attribute( AssignableDataSource<T>* d)
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
        typename AssignableDataSource<T>::reference_t set() {
            return data->set();
        }

        DataSourceBase::shared_ptr getDataSource() const
        {
            return data;
        }

        typename AssignableDataSource<T>::shared_ptr getAssignableDataSource() const
        {
            return data;
        }

        Attribute<T>* clone() const
        {
            return new Attribute<T>( data.get() );
        }

        Attribute<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
        {
            if ( instantiate ) {
                // by taking a clone(), the DS has a chance to instantiate itself.
                // A clone() of an UnboundDataSource returns the bound type.
                AssignableDataSource<T>* instds = data->clone();
                replacements[ data.get() ] = instds;
                return new Attribute<T>( instds );
            }
            else {
                return new Attribute<T>( data->copy( replacements ) );
            }
        }
    };

    /**
     * As opposed to a Attribute, a Constant can not be assigned to a new value
     * after creation.
     */
    template<typename T>
    class Constant
        : public AttributeBase
    {
    public:
        typename DataSource<T>::shared_ptr data;

        Constant(T t)
            : data( new ConstantDataSource<T>( t ) )
        {
        }

        Constant( DataSource<T>* d )
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

        DataSourceBase::shared_ptr getDataSource() const
        {
            return data;
        }

        Constant<T>* clone() const
        {
            return new Constant<T>( data.get() );
        }

        Constant<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate ) 
        {
            // 'symbolic' copy, ConstantDataSource returns 'this' on copy...
            Constant<T>* ret = new Constant<T>( data.get() );
            return ret;
        }
    };

    /**
     * This class is the most basic Attribute implementation
     * (only suitable for reading a DataSource), does
     * not allow any assignment, just stores a DataSource<T>, and
     * returns it.  This also makes it useful as an alias of
     * temporary expressions like literal values, and rhs
     * expressions.
     */
    template<typename T>
    class Alias
        : public AttributeBase
    {
        typename DataSource<T>::shared_ptr data;
    public:
        Alias( typename DataSource<T>::shared_ptr d )
            : data( d )
        {
        }

        DataSourceBase::shared_ptr getDataSource() const
        {
            return data;
        }

        Alias<T>* clone() const
        {
            return new Alias<T>( data.get() );
        }
        Alias<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool )
        {
            // instantiate does not apply.
            return new Alias<T>( data->copy( replacements ) );
        }
    };
}
#endif
