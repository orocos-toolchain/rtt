/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Property.hpp 

                        Property.hpp -  description
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
 
 
#ifndef PI_PROPERTY
#define PI_PROPERTY

#include <pkgconf/corelib_properties.h>
#include "Marshaller.hpp"
#include "PropertyBase.hpp"
#include "PropertyBag.hpp"
#ifdef OROCLS_CORELIB_PROPERTIES_OPERATIONS
#include "PropertyOperation.hpp"
#endif
#include "PropertyDecomposition.hpp"

#ifdef HAVE_STRING
#include <string>
#endif

#include <iostream>

namespace ORO_CoreLib
{
#ifdef OROCLS_CORELIB_PROPERTIES_OPERATIONS
    using namespace detail;
#endif
    /**
     * @group helpers Helper functions for Property operations.
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
                update(value, p.get());
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

        virtual void mutate( PropertyMutatingIntrospection * pmi)
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            pmi->introspect( *this );
        }

#ifdef OROCLS_CORELIB_PROPERTIES_OPERATIONS
            virtual bool update( const PropertyBase* b)
            {
                // could do it like :
                //    FillOperation fil_op(this, &b);
                //    return fil_op.result();
                FillOperation fil_op;
                //std::cout <<"*******************command"<<std::endl;
                return fil_op.command(*this, b);
            }

            virtual bool copy( const PropertyBase* b)
            {
                DeepCopyOperation cop_op;
                return cop_op.command(*this, b);
            }


            virtual bool comply(FillOperation* op) const
            {
                return op->comply( this );
            }

            virtual bool comply(DeepCopyOperation* op) const
            {
                return op->comply( this );
            }

#endif
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

}

/** @example simple_hibernate.cpp
 * This example shows how to use simple properties in combination with a
 * marshaller. The SimpleMarshaller is used and the serialized data is
 * shown on standard output.
 */
/** @example simple_incarnate.cpp
 * This example shows how to demarshall a stream containing simple properties.
 * The SimpleDemarshaller is used and the serialized data is read from standard input.
 */

/** @example adding_property_types.cpp
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
