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
#include "PropertyOperation.hpp"
#include "PropertyDecomposition.hpp"

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
     * These two helper functions define for each type which
     * operators need to be applied to copy or update a type T.
     *
     * @{
     */
    template< class T>
    inline void copy(T& a, const T& b)
    {
        a = b;
    }

    template< class T>
    inline void update(T& a, const T& b)
    {
        a = b;
    }
    /**
     * @}
     */

    class PropertyMutatingIntrospection;

	/**
	 * @brief A property represents data of any type.
     *
	 * A property is a tuple of a name, a description and a variable of any
	 * type.
	 *
     * @param T The type of the data contained within the Property.
	 */
    template<typename T>
    class Property : public PropertyBase
    {
        public:

			/**
			 * The constructor which initializes the property's value.
			 * @param name The name which will be used to refer to the
			 * property.
			 * @param description The description of the property.
			 * @param value The initial value of the property.
			 */
            Property(const std::string& name, const std::string& description, const T& value)
				: PropertyBase(name, description), _value(value)
            {}

			/**
			 * The constructor which does not initialize the property's value.
			 * @param name The name which will be used to refer to the
			 * property.
			 * @param description The description of the property.
			 */
			Property(const std::string& name, const std::string& description)
				: PropertyBase(name, description)
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
            T& operator=(const T &value)
            {
                _value = value;
                return _value;
            }

			/**
			 * Set the property's value.
			 * @param value The value to be set.
			 * @return A reference to newly set property value.
			 */
            T& operator=(T &value)
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
            operator T() const
            {
                return _value;
            }

			/**
			 * Get the value of the property.
			 * @return The value of the property.
			 */
            const T& get() const
            {
                return _value;
            }

            /**
             * Acces to the value of the Property.
             */
            T& set()
            {
                return _value;
            }

            /**
             * Acces to the value of the Property.
             */
            T& value()
            {
                return _value;
            }

            virtual void identify( PropertyIntrospection* pi) const
            {
                pi->introspect( *this );
            }

//         virtual void mutate( PropertyMutatingIntrospection * pmi)
//         {
//             //std::cout << __PRETTY_FUNCTION__ << std::endl;
//             pmi->introspect( *this );
//         }

        virtual bool update( const PropertyBase* other) 
        {
            detail::FillOperation<T> fillop(this);
            return fillop.command( other );
        }

        virtual bool copy( const PropertyBase* other )
        {
            detail::DeepCopyOperation<T> copop(this);
            return copop.command( other );
        }

            virtual bool accept( detail::PropertyOperation* op ) const
            {
                return op->comply( this );
            }

            /**
             * Update the value, leave the rest.
             */
            void update( const Property<T>& orig)
            {
                ORO_CoreLib::update( _value, orig.get() );
            }

            /**
             * Make a full copy.
             */
            void copy( const Property<T>& orig)
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
        protected:
            T _value;
        private:
    };

#ifdef HAVE_IOSTREAM
    template<typename T>
    std::ostream& operator<<(std::ostream &os, Property<T> &p)
    {
        // os << T( p );
        os << p.get();
        return os;
    }
#endif

    extern template class Property<double>;
    extern template class Property<bool>;
    extern template class Property<float>;
    extern template class Property<int>;
    extern template class Property<unsigned int>;
    extern template class Property<std::string>;
    extern template class Property<PropertyBag>;

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
