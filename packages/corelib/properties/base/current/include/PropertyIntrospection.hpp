/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyIntrospection.hpp 

                        PropertyIntrospection.hpp -  description
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
 
#ifndef PI_PROPERTYINTROSPECTION_HPP
#define PI_PROPERTYINTROSPECTION_HPP

#include <string>

namespace ORO_CoreLib
{

    class PropertyBag;
    class PropertyIntrospection;
	template<typename T> class Property;
    
	template<typename T>
	void decomposeProperty(PropertyIntrospection *m, const Property<T> &b);

	/**
	 * An interface which all classes which wish to introspect
     * a Property should implement. When you call 
     * PropertyBase::introspect( PropertyIntrospection* ), The base
     * will call one of below methods to identify its type to the caller.
	 *
	 * @see Property
	 * @see PropertyBag
	 */
    class PropertyIntrospection
    {
        public:
        virtual ~PropertyIntrospection()
        {}
			/**
			 * introspect a property of type bool.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<bool> &v) = 0;
			
			/**
			 * introspect a property of type char.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<char> &v) = 0;
			
			/**
			 * introspect a property of type int.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<int> &v) = 0;
			
			/**
			 * introspect a property of type unsigned int.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<unsigned int> &v) = 0;
			
			/**
			 * introspect a property of type double.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<double> &v) = 0;
			
			/**
			 * introspect a property of type string.
			 * @param v The property to be introspectd.
			 */
			virtual void introspect(const Property<std::string> &v) = 0;
			
			/**
			 * introspect a property of type property bag. This enables
			 * nested property bags.
			 * @param v The property to be introspectd.
			 */
            virtual void introspect(const Property<PropertyBag> &v) = 0;

            /** 
             * Unknown types must decompose theirselves into the primitives.
             * For each type you define, you need to provide the function
             * 
             *    decomposeProperty( PropertyIntrospection*, const Property<YourType>& )
             *
             * An Property<std::string> will be generated containing an error message
             * when this function is not found.
             */
            template< class T >
            void introspect( const Property<T> &v )
            {
                decomposeProperty(this, v);
            }
	};
}

#endif
