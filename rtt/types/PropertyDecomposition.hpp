/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  PropertyDecomposition.hpp

                        PropertyDecomposition.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_PROPERTYDECOMPOSITION_HPP_
#define ORO_PROPERTYDECOMPOSITION_HPP_

#include "../base/PropertyBase.hpp"
#include "../PropertyBag.hpp"

namespace RTT {
    namespace types {

        /**
         * Uses the type decomposition to decompose a property
         * into a property bag that refers to all its parts.
         *
         * It will first try to use the user's TypeInfo::decomposeType function and return
         * that in targetbag if it generates a PropertyBag. If it generated something
         * else than a PropertyBag, this function returns false. If decomposeType
         * did not return anything, it will try the TypeInfo::getMember() approach to
         * decompose source into the targetbag. If no members are returned, this function
         * fails.
         *
         * In case the source type info supports it, each modification of a
         * part in the \a targetbag will modify \a source too. This is so for
         * typekits using the boost::serialization functions in combination with TypeInfo::getMember(). In case the type
         * decompositions were written manually using TypeInfo::decomposeType(), this relation can no longer be
         * guaranteed, and a composition step using TypeInfo::composeType() of the same type must be tried to update
         * \a source with the modifications in \a targetbag.
         *
         * For the TypeInfo::getMember() method: This function can only work if every part of the source
         * is known by the RTT type system. Only the parts of source that are
         * assignable will be decomposed. The read-only parts will be silently omitted.
         *
         * @param source Contains a C++ type to be decomposed into a hierarchy of properties.
         * @param targetbag The bag in which to place the result.
         * @return True on success, false otherwise.
         */
        bool RTT_API propertyDecomposition( base::PropertyBase* source, PropertyBag& targetbag, bool recurse = true );

        /**
         * Identical to RTT::types::propertyDecomposition(), but takes a DataSourceBase as source.
         * @param source Contains a C++ type to be decomposed into a hierarchy of properties.
         * @param targetbag The bag in which to place the result.
         * @return True on success, false otherwise.
         * @see RTT::types::propertyDecomposition
         */
        bool RTT_API typeDecomposition( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag, bool recurse = true);
    }
}

#endif /* ORO_PROPERTYDECOMPOSITION_HPP_ */
