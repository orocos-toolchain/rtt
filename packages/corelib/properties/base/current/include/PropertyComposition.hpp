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

namespace ORO_CoreLib
{
	/** 
     * Deprecated. Only here for backwards compatibility of older applications.
	 */
	template<typename T>
	bool composeProperty(const PropertyBag& bag, Property<T> &result)
	{
        // work around GCC < 3.4 bug : can not use getProperty<T> due to parse error.
        PropertyBase* newProp = bag.find( result.getName() );
        if ( newProp !=0 )
        {
            return result.getTypeInfo()->composeType( newProp->getDataSource(), result.getDataSource() );
        }
        return false;
	} 
	
}

#endif
