#ifndef PI_PROPERTYINTROSPECTION_HPP
#define PI_PROPERTYINTROSPECTION_HPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
