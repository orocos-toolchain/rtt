/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  Attribute.hpp

                        Attribute.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CORELIB_ATTRIBUTE_HPP
#define ORO_CORELIB_ATTRIBUTE_HPP

#include "rtt/internal/DataSource.hpp"
#include "rtt/internal/DataSources.hpp"
#include "rtt/base/AttributeBase.hpp"

namespace RTT
{
    /**
     * An Attribute has a name and contains data which can be set and get.
     * Accessing an attribute (reading/writing) is \b not thread-safe.
     * It is used to expose a C/C++ variable to the interface of a TaskContext
     * in order to allow plugins or external tools to read and write it.
     * @param T The type of data this attribute holds.
     */
    template<typename T>
    class Attribute
        : public base::AttributeBase
    {
        typename internal::AssignableDataSource<T>::shared_ptr data;
    public:

        /**
         * Create an Attribute with no name and \b no value.
         */
        Attribute()
        {
        }

        /**
         * Create an Attribute with a given name and a \b default value.
         *
         * @param name The name of this instance.
         */
        explicit Attribute(const std::string& name)
            : base::AttributeBase(name),
              data( new internal::ValueDataSource<T>( T() ) )
        {}

        /**
         * Create an Attribute with a name and a given value \a t.
         *
         * @param name The name of this instance.
         * @param t The value for initialisation.
         */
        Attribute(const std::string& name, T t)
            : base::AttributeBase(name),
              data( new internal::ValueDataSource<T>( t ) )
        {
        }

        /**
         * Create an Attribute with a name, a given value \a t and an owner.
         * The owner is used to register this attribute to and is supposed
         * to be a pointer (or shared pointer).
         *
         * @param name The name of this instance.
         * @param t The value for initialisation.
         * @param o The owner, which has a function 'addAttribute(AttributeBase*)'.
         */
        template<class Owner>
        Attribute(const std::string& name, T t, Owner o)
            : base::AttributeBase(name),
              data( new internal::ValueDataSource<T>( t ) )
        {
            o->addAttribute(this);
        }

        /**
         * Create an Attribute which uses a internal::DataSource \a d.
         *
         * @param name The name
         * @param d The data source to read from and write to.
         */
        Attribute( const std::string& name, internal::AssignableDataSource<T>* d)
            : base::AttributeBase(name),
              data( d )
        {
        }

        /**
         * Copy constructor copies both name and \b deep copy of the data.
         */
        Attribute( const Attribute<T>& a)
            : base::AttributeBase( a.mname ),
              data( a.data->clone() )
        {
        }

        /**
         * Assignment copies both name and \b deep copy of the data.
         */
        Attribute<T>& operator=( const Attribute<T>& a)
        {
            if ( this == &a )
                return *this;
            mname = a.mname;
            data = a.data->clone();
            return *this;
        }

        /**
         * Create an Attribute which \b mirrors a base::AttributeBase \a ab.
         * If successful, this attribute will always have the same
         * value as \a ab and vice versa.
         *
         * @param ab The Attribute to mirror.  If null, this will clear
         * this attribute and clear its name.
         * @see ready() to check if \a ab was accepted.
         */
        Attribute( base::AttributeBase* ab)
            : base::AttributeBase( ab ? ab->getName() : "" ),
              data( ab ? internal::AssignableDataSource<T>::narrow( ab->getDataSource().get() ) : 0 )
        {
        }

        /**
         * Initialise an Attribute which \a mirrors an base::AttributeBase \a ab.
         * If successful, this attribute will always have the same
         * value as \a ab and vice versa.
         * @param ab The attribute to mirror. If null, this will clear
         * this attribute and clear its name.
         * @see ready() to check if ab was accepted.
         */
        Attribute<T>& operator=(base::AttributeBase* ab)
        {
            if ( ab == this )
                return *this;

            if (!ab) {
                data = 0;
                mname.clear();
                return *this;
            }
            typename internal::AssignableDataSource<T>::shared_ptr a
                = boost::dynamic_pointer_cast<internal::AssignableDataSource<T> >( ab->getDataSource() );
            if (a) {
                data = a;
                mname = ab->getName();
            } else {
                data = 0;
            }
            return *this;
        }

        /**
         * Get the value of this Attribute.
         */
        T const& get() const
        {
            data->evaluate();
            return data->rvalue();
        }

        /**
         * Set the value of this Attribute.
         * In case the attribute is owned by a remote component,
         * the value will be transfered.
         */
        void set( T const& t )
        {
            data->set(t);
        }

        /**
         * Allow to set the value of this Attribute by reference.
         * Don't use this function to set attributes of remote components,
         * since set() will return in that case a copy of the data and
         * the remote side will not be updated.
         */
        typename internal::AssignableDataSource<T>::reference_t set() {
            return data->set();
        }

        base::DataSourceBase::shared_ptr getDataSource() const
        {
            return data;
        }

        typename internal::AssignableDataSource<T>::shared_ptr getAssignableDataSource() const
        {
            return data;
        }

        Attribute<T>* clone() const
        {
            return new Attribute<T>( mname, data.get() );
        }

        Attribute<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool instantiate )
        {
            if ( instantiate ) {
                // by taking a clone(), the DS has a chance to instantiate itself.
                // A clone() of an internal::UnboundDataSource returns the bound type.
                internal::AssignableDataSource<T>* instds = data->clone();
                replacements[ data.get() ] = instds;
                return new Attribute<T>( mname, instds );
            }
            else {
                return new Attribute<T>( mname, data->copy( replacements ) );
            }
        }
    };

    /**
     * As opposed to a Attribute, a Constant can not be assigned to a new value
     * after creation.
     */
    template<typename T>
    class Constant
        : public base::AttributeBase
    {
    public:
        typename internal::DataSource<T>::shared_ptr data;

        /**
         * Create a Constant with no name and \b no value.
         */
        Constant()
        {
        }

        /**
         * Create a constant with a fixed value \a t.
         * @param name The name of this instance.
         * @param t The value for initialisation.
         */
        Constant(const std::string& name, T t)
            : base::AttributeBase(name),
              data( new internal::ConstantDataSource<T>( t ) )
        {
        }

        /**
         * Create a constant with a fixed value \a t and an owner.
         * The owner is used to register this attribute to and is supposed
         * to be a pointer (or shared pointer).
         *
         * @param name The name of this instance.
         * @param t The value for initialisation.
         * @param o The owner, which has a function 'addAttribute(AttributeBase*)'.
         */
        template<class Owner>
        Constant(const std::string& name, T t, Owner owner)
            : base::AttributeBase(name),
              data( new internal::ConstantDataSource<T>( t ) )
        {
            owner->addAttribute( this );
        }

        /**
         * Create a constant wich holds a internal::DataSource \a d.
         */
        Constant(const std::string& name, internal::DataSource<T>* d )
            : base::AttributeBase(name),
              data( d )
        {
        }

        /**
         * Create a constant which mirrors an Attribute.
         * If successful, this constant will always have the same
         * value as \a ab.
         * @param ab The attribute to mirror. If null, this will clear
         * this attribute and clear its name.
         * @see ready() to check if ab was accepted.
         */
        Constant( base::AttributeBase* ab )
            : base::AttributeBase( ab ? ab->getName() : ""),
              data( ab ? internal::DataSource<T>::narrow( ab->getDataSource().get() ) : 0 )
        {
        }

        /**
         * Initialise an Attribute which mirrors an AttributeBase.
         * If successful, this constant will always have the same
         * value as \a ab.
         * @param ab The attribute to mirror. If null, this will clear
         * this attribute and clear its name.
         * @see ready() to check if ab was accepted.
         */
        Constant<T>& operator=(base::AttributeBase* ab)
        {
            if ( ab == this)
                return *this;
            if (!ab) {
                data = 0;
                mname.clear();
                return *this;
            }
            typename internal::DataSource<T>::shared_ptr a
                = boost::dynamic_pointer_cast<internal::DataSource<T> >( ab->getDataSource() );
            if (a) {
                data = a;
                mname = ab->getName();
            } else {
                data = 0;
            }
            return *this;
        }

        /**
         * Get the value of this Constant.
         */
        T get() const
        {
            return data->get();
        }

        base::DataSourceBase::shared_ptr getDataSource() const
        {
            return data;
        }

        Constant<T>* clone() const
        {
            return new Constant<T>( mname, data.get() );
        }

        Constant<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool instantiate )
        {
            // 'symbolic' copy, internal::ConstantDataSource returns 'this' on copy...
            Constant<T>* ret = new Constant<T>( mname, data.get() );
            return ret;
        }
    };

    /**
     * This class is the most basic Attribute implementation
     * (only suitable for reading a internal::DataSource), does
     * not allow any assignment, just stores a internal::DataSourceBase, and
     * returns it.  This also makes it useful as an alias of
     * temporary expressions like literal values, and rhs
     * expressions.
     */
    class RTT_API Alias
        : public base::AttributeBase
    {
        base::DataSourceBase::shared_ptr data;
    public:
        Alias(const std::string& name, base::DataSourceBase::shared_ptr d );

        /**
         * Create an alias from a datasource with an owner.
         * The owner is used to register this attribute to and is supposed
         * to be a pointer (or shared pointer).
         *
         * @param name The name of this instance.
         * @param d The aliased data source.
         * @param o The owner, which has a function 'addAttribute(AttributeBase*)'.
         */
        template<class Owner>
        Alias(const std::string& name, base::DataSourceBase::shared_ptr d, Owner owner)
            : base::AttributeBase(name),
              data( d )
        {
            owner->addAttribute( this );
        }

        base::DataSourceBase::shared_ptr getDataSource() const;

        Alias* clone() const;

        Alias* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool );
    };
}
#endif
