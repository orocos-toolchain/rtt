/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:57 CEST 2004  PropertyBagIntrospector.hpp

                        PropertyBagIntrospector.hpp -  description
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

#ifndef PROPERTYBAGINTROSPECTOR_HPP
#define PROPERTYBAGINTROSPECTOR_HPP

#include "rtt/marsh/rtt-marsh-config.h"
#include "rtt/PropertyBag.hpp"
#include "rtt/Property.hpp"
#include "rtt/base/PropertyIntrospection.hpp"
#include "rtt/types/PropertyDecomposition.hpp"
#include <stack>

namespace RTT
{ namespace marsh {


    /**
     * @brief This class fills up a given bag with
     * the results of an introspection.
     * @deprecated DO NOT USE. Will be removed in 2.x release series.
     */
    class RTT_MARSH_API PropertyBagIntrospector
        : public base::PropertyBagVisitor
    {
        std::stack<PropertyBag*> mystack;
    public:
        /**
         * @brief Create a new Introspector, saving the results in a bag.
         *
         * The \a bag will contain decomposed clones of the original.
         *
         * @param bag The bag to safe the results in.
         */
        PropertyBagIntrospector( PropertyBag& bag );

        virtual ~PropertyBagIntrospector();

        /**
         * Reset to do a new introspection.
         * @param new_bag The new bag to save the results in.
         */
        void reset( PropertyBag& new_bag);

        /**
         * Use this entry function to inspect a bag.
         * @param v the properties of this bag will be inspected.
         */
        void introspect(const PropertyBag& v );

        virtual void introspect(base::PropertyBase* v);

        virtual void introspect(Property<PropertyBag> &v);
    };
}}

#endif
