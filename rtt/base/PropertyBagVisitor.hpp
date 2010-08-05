/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyBagVisitor.hpp

                        PropertyBagVisitor.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef PI_PROPERTY_BAG_VISITOR_HPP
#define PI_PROPERTY_BAG_VISITOR_HPP

#include "../rtt-config.h"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace base {

    /**
     * A simple introspection interface to visit PropertyBags.
     * A class which implements this interface can call
     * <tt>bag.identify( this );</tt>. For each property,
     * introspect(PropertyBase* p) is called, unless
     * the property contains a bag, then
     * introspect(Property<PropertyBag>& p) is called,
     * upon which you may call identify again on it's value().
     * The marshallers use this technique.
     * @see PropertyIntrospection to have a detailed type lookup
     * of a property.
     * @deprecated DO NOT USE. Will be removed in 2.x release series.
     */
    class RTT_API PropertyBagVisitor
    {
    protected:
        /**
         * The default handler to execute when an unknown
         * type is being decomposed.
         * @return true when it could be decomposed or converted to int
         * or false otherwise.
         */
        bool introspectAndDecompose(PropertyBase* t);

    public:
        virtual ~PropertyBagVisitor()
        {}
        /**
         * Callback for a Property which is not a PropertyBag.
         */
        virtual void introspect(PropertyBase* p) = 0;

        /**
         * Callback for a Property which is a PropertyBag.
         */
        virtual void introspect(Property<PropertyBag>& p) = 0;
    };

}}
#endif

