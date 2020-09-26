/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Property.hpp

                        Property.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef ORO_PROPERTY_HPP
#define ORO_PROPERTY_HPP

#include "rtt/rtt-config.h"
#include "rtt/base/PropertyBase.hpp"
#include "rtt/PropertyBag.hpp"
#include "rtt/internal/DataSources.hpp"
#include <boost/type_traits/remove_const.hpp>
#include <boost/call_traits.hpp>
#include "rtt/Logger.hpp"

#include <string>
#include <ostream>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
	/**
	 * @brief A property represents a named value of any type with a description.
     *
	 * A property is a tuple of a name, a description and a variable of any
	 * type. It's purpose is to provide a hierarchical parameter of a
     * component. They are grouped in PropertyBag objects
     * and a Property can contain a PropertyBag itself.
     *
     * If you do not provide a name nor description when constructing the
     * Property object, it will be uninitialised and ready() will return false.
     * Such an object may not be used (set(), get(),...) until it has been
     * initialised from another Property. Otherwise, an exception (or assert) will be
     * thrown.
     *
     * @param T The type of the data contained within the Property.
     * @ingroup CoreLibProperties
	 */
    template<typename T>
    class Property
        : public base::PropertyBase
    {
    public:
        /**
         * The types of this property type.
         * value_t is always the 'bare' value type of T. From this type,
         * we derive the other (param, ref, ...) types.
         */
        typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type value_t;
        typedef typename boost::call_traits<value_t>::param_type param_t;
        typedef typename boost::call_traits<value_t>::reference reference_t;
        typedef typename boost::call_traits<value_t>::const_reference const_reference_t;
        typedef value_t DataSourceType;

        /**
         * Create an empty Property with no name, no description and no value.
         * @post ready() will return false. Initialise this Property with another Property object.
         */
        Property()
        {}

        /**
         * The constructor which initializes the property's value to the default.
         * @param name The name which will be used to refer to the
         * property.
         * @post ready() will always be true.
         */
        explicit Property(const std::string& name)
            : base::PropertyBase(name,""), _value( new internal::ValueDataSource<value_t>() )
        {
        }

        /**
         * The constructor which initializes the property's value.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param value The initial value of the property (optional).
         * @post ready() will always be true.
         */
        Property(const std::string& name, const std::string& description, param_t value = value_t() )
            : base::PropertyBase(name, description), _value( new internal::ValueDataSource<value_t>( value ) )
        {
        }

        /**
         * Copy constructors copies the name, description and value
         * as \b deep copies.
         * @post ready() will be true if orig.ready() is true.
         */
        Property( const Property<T>& orig)
            : base::PropertyBase(orig.getName(), orig.getDescription()),
              _value( orig._value ? orig._value->clone() : 0 )
        {
            // need to do this on the clone() in order to have access to set()/rvalue() of the data source.
            if (_value)
                _value->evaluate();
        }

        /**
         * Create a Property \b mirroring another PropertyBase.
         * It copies the name and description, and \b shallow copies
         * the value.
         * @see ready() to inspect if the creation succeeded.
         */
        Property( base::PropertyBase* source)
            : base::PropertyBase(source ? source->getName() : "", source ? source->getDescription() : "")
        {
            if ( source ) {
                base::DataSourceBase::shared_ptr dsb = source->getDataSource();
                if ( !setDataSource(dsb) ) {
                     log(Error) << "Cannot initialize Property from " << source->getName() << ": ";
                     if ( dsb ) {
                         log() << "incompatible type ( destination type: " << getType() << ", source type: " << dsb->getTypeName() << ")." << endlog();
                     } else {
                         log() << "source Property was not ready." << endlog();
                     }
                }
            }
        }

        /**
         * The constructor which initializes the property with a DataSource.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param datasource A new data source to be acquired by this property for
         * storing its data.
         * @post ready() will be true if datasource is a valid pointer.
         */
        Property(const std::string& name, const std::string& description,
                 const typename internal::AssignableDataSource<DataSourceType>::shared_ptr& datasource )
            : base::PropertyBase(name, description), _value( datasource )
        {
            // need to do this on the datasource in order to have access to set()/rvalue() of the data source.
            if (_value)
                _value->evaluate();
        }

        /**
         * Assignment copies the name, description and value
         * as \b deep copies.
         * @post ready() will be true if orig.ready() is true.
         */
        Property<T>& operator=( const Property<T>& orig)
        {
            if ( this == &orig )
                return *this;

            this->setName( orig.getName() );
            this->setDescription( orig.getDescription() );

            if (orig._value) {
                _value = orig._value->clone();
                // need to do this on the clone() in order to have access to set()/rvalue() of the data source.
                _value->evaluate();
            } else {
                _value = 0;
            }

            return *this;
        }

        /**
         * Set the property's value.
         * @param value The value to be set.
         * @return A reference to newly set property value.
         */
        Property<T>& operator=( param_t value )
        {
            if (_value)
                _value->set( value );
            return *this;
        }

        /**
         * Mirror another PropertyBase (name, description and value).
         * @param source A pointer to the property to mirror.
         */
        Property<T>& operator=( base::PropertyBase* source )
        {
            if ( this == source )
                return *this;

            if ( source ) {
                this->setName( source->getName() );
                this->setDescription( source->getDescription() );

                base::DataSourceBase::shared_ptr dsb = source->getDataSource();
                if ( this->setDataSource(dsb) ) {
                    return *this;
                }
            }

            // wrong assignment: mark not ready.
            this->setName( "" );
            this->setDescription( "" );
            _value = 0;
            return *this;
        }

        /**
         * Update the value of this property with
         * another property.
         */
        Property<T>& operator<<=(Property<T> &p)
        {
            this->update( p );
            return *this;
        }

        /**
         * Documents this property.
         * @param descr The description of this property
         * @return A reference to this object.
         */
        Property<T>& doc(const std::string& descr) {
            setDescription(descr);
            return *this;
        }

        /**
         * Get a copy of the value of the property.
         * @return A copy of the value of the property.
         */
        operator value_t() const
        {
            return _value->get();
        }

        /**
         * Get a copy of the value of the property.
         * @return The value of the property.
         */
        DataSourceType get() const
        {
            return _value->get();
        }

        /**
         * Access to the value of the Property. Identical to value().
         * @warning This function is not suitable
         * for remote (distributed) access of properties,
         * use operator=() or set( param_t v ) to assign a value.
         */
        reference_t set()
        {
            return _value->set();
        }

        /**
         * Set the value of the Property.
         */
        void set(param_t v)
        {
            _value->set(v);
        }

        /**
         * Access to the value of the Property. Identical to set().
         * @warning This function is not suitable
         * for remote (distributed) access of properties,
         * use operator=() or set( param_t v ) to assign a value.
         *
         */
        reference_t value()
        {
            return set();
        }

        /**
         * Read-only (const&) access to the value of the Property.
         */
        const_reference_t rvalue() const
        {
            return _value->rvalue();
        }

        /**
         * Use this method instead of dynamic_cast<> to cast from
         * base::PropertyBase to Property<T>. You need to delete the returned property
         * if it is no longer needed.
         * @param T The Desired property type, for example 'double'.
         * @param prop The property to narrow to Property<T>.
         * @return Null if prop is not convertible to type Property<T>,
         * a \b new Property<T> instance otherwise.
         */
        static Property<T>* narrow( base::PropertyBase* prop );

        virtual void identify( base::PropertyIntrospection* pi);

        virtual void identify( base::PropertyBagVisitor* pi);

        virtual bool update( const base::PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                return this->update( *origin );
            }
            return false;
        }

        virtual bool refresh( const base::PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 && _value ) {
                return this->refresh( *origin );
            }
            return false;
        }

        virtual bool copy( const base::PropertyBase* other )
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 && _value ) {
                return this->copy( *origin );
            }
            return false;
        }

        /**
         * Copy the value, complete overwrite of this Property with orig.
         */
        bool copy( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            _description = orig.getDescription();
            _name = orig.getName();
            _value->set( orig.rvalue() );
            return true;
        }

        /**
         * Update the value, optionally also the description if current
         * description is empty.
         */
        bool update( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            if ( _description.empty() )
                _description = orig.getDescription();
            _value->set( orig.rvalue() );
            return true;
        }

        /**
         * Refresh only the value from a Property.
         * This is a real-time operation.
         */
        bool refresh( const Property<T>& orig)
        {
            if ( !ready() )
                return false;
            _value->set( orig.rvalue() );
            return true;
        }

        virtual Property<T>* clone() const
        {
            return new Property<T>(*this);
        }

        virtual Property<T>* create() const
        {
            return new Property<T>( _name, _description, T() );
        }

        virtual Property<T>* create( const base::DataSourceBase::shared_ptr& datasource ) const
        {
            typename internal::AssignableDataSource<DataSourceType>::shared_ptr value
                    = internal::AssignableDataSource<DataSourceType>::narrow( datasource.get() );
            Property<T>* prop = new Property<T>( _name, _description, value );
            if ( datasource && !prop->ready() ) {
                log(Error) << "Cannot initialize Property: "
                           << "incompatible type ( destination type: " << getType() << ", source type: " << datasource->getTypeName() << ")." << endlog();
            }
            return prop;
        }

        virtual base::DataSourceBase::shared_ptr getDataSource() const {
            return _value;
        }

        typename internal::AssignableDataSource<DataSourceType>::shared_ptr getAssignableDataSource() const {
            return _value;
        }

        /**
         * Assign an external assignable base::DataSource to this property.
         * @param dsb The other data source
         * @return false if the properties are of different type.
         */
        virtual bool setDataSource( const base::DataSourceBase::shared_ptr& dsb )
        {
            typename internal::AssignableDataSource<DataSourceType>::shared_ptr vptr
                = internal::AssignableDataSource<DataSourceType>::narrow(dsb.get());
            if (vptr) {
                _value.swap(vptr);
                return true;
            }
            return false;
        }

        virtual std::string getType() const {
            return internal::DataSource<T>::GetType();
        }

        virtual const types::TypeInfo* getTypeInfo() const {
            return internal::DataSource<T>::GetTypeInfo();
        }
    protected:
        typename internal::AssignableDataSource<DataSourceType>::shared_ptr _value;
    };


    /**
     * Partial specialisations in case of PropertyBag.
     */
    template<>
    RTT_API bool Property<PropertyBag>::update( const Property<PropertyBag>& orig);

    template<>
    RTT_API bool Property<PropertyBag>::refresh( const Property<PropertyBag>& orig);

    template<>
    RTT_API bool Property<PropertyBag>::copy( const Property<PropertyBag>& orig);

    template<typename T>
    std::ostream& operator<<(std::ostream &os, Property<T> &p)
    {
#ifdef OS_HAVE_STREAMS
        os << p.getDataSource();
#endif
        return os;
    }

    template<class T>
    Property<T>* Property<T>::narrow( base::PropertyBase* prop ) {
        Property<T>* res = dynamic_cast<Property<T>*>( prop );
        return res;
    }
}

#include "rtt/base/PropertyIntrospection.hpp"

namespace RTT
{
    template< class T>
    void Property<T>::identify( base::PropertyIntrospection* pi)
    {
        pi->introspect( *this );
    }

    template< class T>
    void Property<T>::identify( base::PropertyBagVisitor* pi)
    {
        return base::PropertyBase::identify(pi);
    }

    template<>
    RTT_API void Property<PropertyBag>::identify( base::PropertyBagVisitor* pbi);
}

#endif
