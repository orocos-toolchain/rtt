#ifndef PROPERTYCOMPOSITION_HPP
#define PROPERTYCOMPOSITION_HPP


#include "Property.hpp"
#include "PropertyBag.hpp"

namespace ORO_CoreLib
{
	/** 
	 * The composeProperty function is a helper function which converts typed PropertyBags
	 * of primitive types into a single type <T>. It is the inverse of decomposeProperty.
	 *
	 * Every new type used as a property will automatically get a generated
	 * function like the one below. It is your task to copy this function and
     * rewrite it for your type so that <YourType> is can be composed from primitive
     * property types in a bag. This is fully complementary to the decomposeProperty function.
     * 
	 * To be able to compose this new type, you need to write a
	 * specialized implementation of this function for your specific type.
	 * Include then the file containing the new function in your program and
     * it will be used automatically.
     *
     * Example for type MyClass :
     * \code
     * namespace ORO_CoreLib {
     *
     *  void composeProperty(const PropertyBag& bag, Property< MyClass > &result)
     *  {
     *     PropertyBase* v_base = bag.find( result.getName() );
     *     PropertyBag* v_bag = dynamic_cast<PropertyBag*>(v_base);
     *     if (v_bag != 0)
     *     {
     *             Property<bool>* var1 = dynamic_cast< Property<bool>* >(v_bag->find("Var1"));
     *             Property<double>* var2 = dynamic_cast< Property<double>* >(v_bag->find("Var2));
     *             if (var1 != 0  && var2 != 0)
     *             {
     *                  result.var1 = var1.get();
     *                  result.var2 = var2.get();
     *             }
     *     }
     *     // done !
	 *  }
     * }
     * \endcode
     *  @see PropertyDecomposion.hpp
     *
     *  @note <b>IMPORTANT ! <br> 
     *        The PropertyComposition and PropertyDecomposition functions must reside
     *        in the ORO_CoreLib namespace, or the compiler will not find them !</b>
	 */
	template<typename T>
	bool composeProperty(const PropertyBag& bag, Property<T> &result)
	{
        Property<T>* newProp = dynamic_cast< Property<T>* >(bag.find( result.getName() ));
        if ( newProp !=0)
        {
            result.set() = newProp->get();
            return true;
        }
        return false;
	} 
	
}

#endif
