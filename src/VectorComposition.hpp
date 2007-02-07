/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  VectorComposition.hpp 

                        VectorComposition.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef VECTOR_COMPOSITION_HPP
#define VECTOR_COMPOSITION_HPP

#include <vector>

namespace RTT
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

}; // namespace RTT


#endif
