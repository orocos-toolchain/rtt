/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyComposition.hpp 

                        PropertyComposition.hpp -  description
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
 
#ifndef PROPERTYCOMPOSITION_HPP
#define PROPERTYCOMPOSITION_HPP


#include "Property.hpp"
#include "PropertyBag.hpp"
#include "VectorComposition.hpp"
#include "MultiVectorComposition.hpp"

#include <pkgconf/corelib_properties.h>
#ifdef OROCLS_CORELIB_PROPERTIES_MOTIONPROPERTIES
#include <geometry/MotionProperties.hpp>
#endif

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
     *  bool composeProperty(const PropertyBag& bag, Property< MyClass > &result)
     *  {
     *     Property<PropertyBag>* v_bag = bag.getProperty<PropertyBag>( result.getName()  );
     *     if (v_bag != 0 && v_bag.getType() == "MyClass" ) // Bag type is extra sanity check
     *     {
     *             Property<bool>* var1 = v_bag->get()->getProperty<bool>("Var1");
     *             Property<double>* var2 = v_bag->get()->getProperty<double>("Var2");
     *             if (var1 != 0  && var2 != 0)
     *             {
     *                  result.var1 = var1.get();
     *                  result.var2 = var2.get();
     *                  return true; // Done !
     *             }
     *     }
     *     return false;
	 *  }
     * }
     * \endcode
     *  @see decomposeProperty for the inverse operation
     *
     *  @note <b>IMPORTANT ! <br> 
     *        The composeProperty and decomposeProperty functions must reside
     *        in the ORO_CoreLib namespace, or the compiler will not find them !</b>
	 */
	template<typename T>
	bool composeProperty(const PropertyBag& bag, Property<T> &result)
	{
        // work around GCC < 3.4 bug : can not use getProperty<T> due to parse error.
        Property<T>* newProp = dynamic_cast< Property<T>* >(bag.find( result.getName() ));
        if ( newProp !=0 )
        {
            result.set() = newProp->get();
            return true;
        }
        return false;
	} 
	
}

#endif
