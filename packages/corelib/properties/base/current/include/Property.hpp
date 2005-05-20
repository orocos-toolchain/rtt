/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Property.hpp 

                        Property.hpp -  description
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
 
#ifndef ORO_PROPERTY_HPP
#define ORO_PROPERTY_HPP

#include <pkgconf/corelib_properties.h>
#include "Marshaller.hpp"
#include "PropertyBase.hpp"
#include "PropertyBag.hpp"
#include "PropertyCommands.hpp"
#include "PropertyDataSource.hpp"
#include <boost/type_traits.hpp>

#ifdef HAVE_STRING
#include <string>
#endif

#include <iostream>

#pragma interface

namespace ORO_CoreLib
{

    /**
     * @brief Helper functions for Property operations.
     *
     * These helper functions define for each type which
     * operators need to be applied to copy, refresh or update a type T.
     *
     * Remark that PropertyBag for example overloads these functions.
     * Standard STL Containers (like vector) don't need an overload ( operator= ).
     * All overloads must be in namespace ORO_CoreLib.
     *
     * @{
     */
    template< class T, class S>
    inline void copy(T& a, const S& b)
    {
        a = b;
    }

    template< class T, class S>
    inline void update(T& a, const S& b)
    {
        a = b;
    }

    template< class T, class S>
    inline void refresh(T& a, const S& b)
    {
        a = b;
    }
    /**
     * @}
     */

	/**
	 * @brief A property represents a value of any type.
     *
	 * A property is a tuple of a name, a description and a variable of any
	 * type. It's purpose is to provide an easy to manipulate parameter of an 
     * object by external entities. They can be grouped in PropertyBag objects
     * and a Property can contain a PropertyBag itself. See the configuration tool
     * to influence how the compiler should generate code for unkown
     * (external to Orocos) classes \a T.
     *
	 *
     * @param T The type of the data contained within the Property.
     * @see decomposeProperty and composeProperty if you want to use properties of
     * external types \a T.
	 */
    template<typename T>
    class Property
        : public PropertyBase
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

        /**
         * The constructor which initializes the property's value.
         * @param name The name which will be used to refer to the
         * property.
         * @param description The description of the property.
         * @param value The initial value of the property (optional).
         */
        Property(const std::string& name, const std::string& description, param_t value = value_t() )
            : PropertyBase(name, description), _value(value)
        {}

        /**
         * Copy constructors copies the name, description and value
         * as deep copies.
         */
        Property( const Property<T>& orig)
            : PropertyBase(orig.getName(), orig.getDescription()),
              _value( orig.get() )
        {}

        /**
         * Set the property's value.
         * @param value The value to be set.
         * @return A reference to newly set property value.
         */
        const_reference_t operator=( param_t value)
        {
            _value = value;
            return _value;
        }

        /**
         * Update the value of this property with
         * another property.
         */
        Property<T>& operator<<=(Property<T> &p)
        {
            ORO_CoreLib::update(_value, p.get());
            return *this;
        }

        /**
         * Get a copy of the value of the property.
         * @return A copy of the value of the property.
         */
        operator value_t() const
        {
            return _value;
        }

        /**
         * Get the value of the property.
         * @return The value of the property.
         */
        const_reference_t get() const
        {
            return _value;
        }

        /**
         * Access to the value of the Property.
         */
        reference_t set()
        {
            return _value;
        }

        /**
         * Set the value of the Property.
         */
        void set(param_t v)
        {
            _value = v;
        }

        /**
         * Access to the value of the Property.
         */
        value_t& value()
        {
            return _value;
        }

        virtual void identify( PropertyIntrospection* pi) const;
        
        virtual bool update( const PropertyBase* other) 
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                this->update( *origin );
                return true;
            }
            return false;
        }

        virtual CommandInterface* updateCommand( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast<const Property<T>* >( other );
            if ( origin != 0 )
                return new detail::UpdatePropertyCommand<T>(this, origin);
            const Property< const_reference_t >* corigin
                = dynamic_cast<const Property<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::UpdatePropertyCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        virtual bool refresh( const PropertyBase* other) 
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                this->refresh( *origin );
                return true;
            }
            return false;
        }

        virtual CommandInterface* refreshCommand( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast<const  Property<T>* >( other );
            if ( origin != 0 )
                return new detail::RefreshPropertyCommand<T>(this, origin);
            const Property< const_reference_t >* corigin
                = dynamic_cast<const Property<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::RefreshPropertyCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        virtual bool refresh( const DataSourceBase* other ) {
            const DataSource<T>* origin = dynamic_cast< const DataSource<T>* >( other );
            if (origin == 0 )
                return false;
            _value = origin->get();
            return true;
        }

        virtual CommandInterface* refreshCommand( DataSourceBase* other) {
            DataSource<T>* origin = dynamic_cast< DataSource<T>* >( other );
            if ( origin != 0 )
                return new detail::RefreshPropertyFromDSCommand<T>(this, origin);
            DataSource< const_reference_t >* corigin
                = dynamic_cast<DataSource<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::RefreshPropertyFromDSCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        virtual bool copy( const PropertyBase* other )
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 ) {
                this->copy( *origin );
                return true;
            }
            return false;
        }

        virtual CommandInterface* copyCommand( const PropertyBase* other)
        {
            const Property<T>* origin = dynamic_cast< const Property<T>* >( other );
            if ( origin != 0 )
                return new detail::CopyPropertyCommand<T>(this, origin);
            const Property< const_reference_t >* corigin
                = dynamic_cast<const Property<const_reference_t >* >( other );
            if ( corigin != 0 )
                return new detail::CopyPropertyCommand<T, const_reference_t>(this, corigin);
            return 0;
        }

        /**
         * Update the value, optionally also the description if current
         * description is empty.
         */
        template<class S>
        void update( const Property<S>& orig)
        {
            if ( _description.empty() )
                _description = orig.getDescription();
            ORO_CoreLib::update( _value, orig.get() );
        }

        /**
         * Refresh only the value from a Property.
         */
        template<class S>
        void refresh( const Property<S>& orig)
        {
            ORO_CoreLib::refresh( _value, orig.get() );
        }

        /**
         * Refresh only the value from a DataSource.
         */
        template<class S>
        void refresh( const DataSource<S>& orig)
        {
            ORO_CoreLib::refresh( _value, orig.get() );
        }

        /**
         * Make a full copy.
         */
        template<class S>
        void copy( const Property<S>& orig)
        {
            _name = orig.getName();
            _description = orig.getDescription();
            ORO_CoreLib::copy( _value, orig.get() );
        }

        virtual PropertyBase* clone() const
        {
            return new Property<T>(*this);
        }

        virtual PropertyBase* create() const
        {
            return new Property<T>( _name, _description );
        }

        virtual AssignableDataSource<T>* createDataSource() {
            return new detail::PropertyDataSource<T>( this );
        }

        virtual std::string getType() const {
            return DataSource<T>::GetType();
        }
    protected:
        value_t _value;
    private:
    };

    template<typename T>
    std::ostream& operator<<(std::ostream &os, Property<T> &p)
    {
        os << p.get();
        return os;
    }

    extern template class Property<double>;
    extern template class Property<bool>;
    extern template class Property<float>;
    extern template class Property<int>;
    extern template class Property<unsigned int>;
    extern template class Property<std::string>;
    extern template class Property<const std::string &>;
    extern template class Property<PropertyBag>;

}

#include "PropertyIntrospection.hpp"

namespace ORO_CoreLib
{
    template< class T>
    void Property<T>::identify( PropertyIntrospection* pi) const
    {
        pi->introspect( *this );
    }

}

// By PS : these are confusing, thus removed.

/* example simple_hibernate.cpp
 * This example shows how to use simple properties in combination with a
 * marshaller. The SimpleMarshaller is used and the serialized data is
 * shown on standard output.
 */
/* example simple_incarnate.cpp
 * This example shows how to demarshall a stream containing simple properties.
 * The SimpleDemarshaller is used and the serialized data is read from standard input.
 */

/* example adding_property_types.cpp
 * This example shows that properties can be of any type, including user
 * defined types. When you want to be able to marshall these properties you
 * need to add support for decomposing objects of your newly defined type
 * into a treestructure of simple properties.
 * <br>
 * The example shows the class Rectangle being used as a property without
 * adding support for marshalling it.
 * The other added type, namely Vector, is being used as a property and
 * the decompose function was added so properties of type Vector can be
 * marshalled with any Marshaller.
 */
#endif
