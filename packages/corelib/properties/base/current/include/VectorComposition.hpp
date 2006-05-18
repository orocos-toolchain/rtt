/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  VectorComposition.hpp 

                        VectorComposition.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef VECTOR_COMPOSITION_HPP
#define VECTOR_COMPOSITION_HPP

#include <vector>

namespace ORO_CoreLib
{
    class PropertyIntrospection;
    class PropertyBag;
    template<class T>
    class Property;

    /**
     * A decomposeProperty method for decomposing a vector<double>
     * into a PropertyBag with Property<double>'s.
     * The dimension of the vector must be less than 100 if you want the
     * Property<double>'s to have a different name.
     */
    void decomposeProperty(const std::vector<double>& c, PropertyBag& targetbag);
    /**
     * A composeProperty method for composing a property of a vector<double>
     * The dimension of the vector must be less than 100.
     */
    bool composeProperty(const PropertyBag& bag, std::vector<double>& result);

}; // namespace ORO_CoreLib


#endif
