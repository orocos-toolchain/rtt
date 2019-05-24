/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyIntrospection.hpp

                        PropertyIntrospection.hpp -  description
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

#ifndef PI_PROPERTYINTROSPECTION_HPP
#define PI_PROPERTYINTROSPECTION_HPP

#include <string>
#include "../rtt-config.h"
#include "PropertyBagVisitor.hpp"

namespace RTT
{ namespace base {

	/**
	 * An interface which all classes which wish to visit
     * a Property should implement. When you call
     * PropertyBase::identify( PropertyIntrospection* ), the object
     * will call one of below methods to expose its type to the caller.
	 *
	 * @see Property
	 * @see PropertyBag
     * @see PropertyBagVisitor to only browse the contents of a PropertyBag
     * hierarchy.
     * @deprecated DO NOT USE. Will be removed in 2.x release series.
	 */
    class RTT_API PropertyIntrospection
        : public PropertyBagVisitor
    {
    protected:
        /**
         * Fall-back function of the last resort when
         * the type is not decomposable and not known
         * to the introspection interface. Called
         * by introspect_T and does nothing by default.
         * You can put code inhere to handle unknown types.
         */
        virtual void introspect(PropertyBase* p );

        /**
         * This function is called for any Property unknown
         * to the introspection interface. It will basically
         * call the generic decomposition function, and if
         * that fails, pass \a p to introspect() where the
         * subclass has a last chance to handle it.
         */
        void introspect_T(PropertyBase* p);
    public:
        virtual ~PropertyIntrospection()
        {}

        using PropertyBagVisitor::introspect;

        /**
         * introspect a property of type bool.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<bool> &v) = 0;

        /**
         * introspect a property of type char.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<char> &v) = 0;

        /**
         * introspect a property of type int.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<int> &v) = 0;

        /**
         * introspect a property of type unsigned int.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<unsigned int> &v) = 0;

        /**
         * introspect a property of type long long.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<long long> &v) = 0;

        /**
         * introspect a property of type unsigned long long.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<unsigned long long> &v) = 0;

        /**
         * introspect a property of type double.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<double> &v) = 0;

        /**
         * introspect a property of type string.
         * @param v The property to be introspected.
         */
        virtual void introspect(Property<std::string> &v) = 0;

        /**
         * Unknown types must decompose theirselves into the primitives.
         * @see types::TemplateTypeInfo
         */
        template< class T >
        void introspect(Property<T> &v );
	};
}}
#endif

#include "PropertyIntrospection.inl"
