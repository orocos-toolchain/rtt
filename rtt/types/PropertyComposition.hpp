/***************************************************************************
  tag: Peter Soetens Tue Jan 11 17:31:48 2011 +0100 PropertyComposition.hpp

                        PropertyComposition.hpp -  description
                           -------------------
    begin                : Tue Jan 11 2011
    copyright            : (C) 2011 Peter Soetens
    email                : peter@thesourceworks.com

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_PROPERTYCOMPOSITION_HPP_
#define ORO_PROPERTYCOMPOSITION_HPP_

#include "../base/PropertyBase.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {

        /**
         * Uses the type composition to compose all typed properties
         * from a property bag.
         *
         * For each PropertyBag found with a type not equal to 'PropertyBag', it
         * will use the type's TypeInfo::composeType function and return
         * that in target if it generates a Property.
         *
         * @param sourcebag Contains a PropertyBag to be composed into a known type.
         * @param target The target type to compose the source bag into.
         * @return true if all typed PropertyBags in sourcebag could be composed to a user type,
         * false if one or more compositions failed.
         */
        bool RTT_API composePropertyBag( PropertyBag const& sourcebag, PropertyBag& target );

        /**
         * Inverse of RTT::types::composePropertyBag().
         * This function effectively replaces the property introspection mechanism for
         * decomposition.
         * @param sourcebag The bag from which to compose the type.
         * @param target Contains a C++ type to be composed into a hierarchy of properties.
         * @return True on success, false otherwise.
         * @see RTT::types::propertyComposition
         */
        bool RTT_API decomposePropertyBag( PropertyBag const& sourcebag, PropertyBag&  target);
    }
}

#endif /* ORO_PROPERTYCOMPOSITION_HPP_ */
