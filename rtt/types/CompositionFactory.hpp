/***************************************************************************
  tag: Peter Soetens Fri Mar 16 21:32:02 2012 +0100 CompositionFactory.hpp

                        CompositionFactory.hpp -  description
                           -------------------
    begin                : Fri Mar 16 2012
    copyright            : (C) 2012 Peter Soetens
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


#ifndef RTT__COMPOSITION_FACTORY_HPP
#define RTT__COMPOSITION_FACTORY_HPP

#include "rtt/base/DataSourceBase.hpp"

namespace RTT {
    namespace types {

        /**
         * A factory for composing/decomposing and converting types to a form suitable for persistent storage, such as an XML file.
         */
        class RTT_API CompositionFactory
        {
        public:
        virtual ~CompositionFactory() {};

        /**
         * Compose a type (target) from a DataSourceBase (source) containing its members.
         * The default behavior tries to assign \a source to \a target. If that fails,
         * it tries to decompose \a target into its members and update the members of \a target with the contents of source.
         *
         * The default implementation in TemplateTypeInfo works for most types, but can be overridden in case there are
         * multiple versions/possibilities to make a \a target from a \a source. For example, in
         * order to support legacy formats or in order to do the inverse of decomposeType().
         *
         * @param source A data source of the same type as \a target OR a PropertyBag that contains the parts of \a target
         * to be refreshed.
         * @param target A data source of the same type as this TypeInfo object which contains the data to be updated from \a source.
         * @return true if source could be updated, false otherwise.
         *
         * @see types::propertyDecomposition and types::typeDecomposition for the inverse function, decomposing a type into
         * datasources and hierarchical properties.
         * @see decomposeType to do the inverse operation.
         */
        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr target) const = 0;

        /**
         * Specialize this function to return an alternate type which represents this one in a compatible way.
         * For example, a short converts to an long or an enum to an int or a string.
         * If your return a datasource containing a property bag, then this function should do the inverse of
         * composeType: the returned property bag contains all parts of the current type (\a source) which can be modified and merged back
         * into this type with composeType. Mathematically: composeType( decomposeType( A ), B); assert( A == B );
         * @return null in order to indicate that decomposition through getMember() may be tried. You may return \a source itself in order
         * to prevent any further decomposition of your type (using getMember(), which is used as fall-back by the rest
         * of the software. For example, to avoid that a string is decomposed
         * into a sequence of chars, or to avoid that a primitive type like 'int' is further queried.
         */
        virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const;

        /**
         * Specialize this function to return an alternate type which represents this one in a compatible way.
         * For example, a short converts to an long or an enum to an int or a string.
         * @return null if this type is not convertible to anything else.
         * @deprecated by decomposeType. We want to rename convertType to decomposeType. This function is left
         * here for transitional purposes.
         */
        virtual base::DataSourceBase::shared_ptr convertType(base::DataSourceBase::shared_ptr source) const;

        /**
         * @}
         */

        };

        typedef boost::shared_ptr<CompositionFactory> CompositionFactoryPtr;

    }
}

#endif
