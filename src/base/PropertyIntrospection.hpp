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
	 */
    class RTT_API PropertyIntrospection
        : public PropertyBagVisitor
    {
    protected:
        virtual void introspect(PropertyBase* p );

        /**
         * The default handler to execute when an unknown
         * type is being decomposed.
         */
        void introspect_T(PropertyBase* t);
    public:
        virtual ~PropertyIntrospection()
        {}

        using PropertyBagVisitor::introspect;

        /**
         * introspect a property of type bool.
         * @param v The property to be introspectd.
         */
        virtual void introspect(Property<bool> &v) = 0;

        /**
         * introspect a property of type char.
         * @param v The property to be introspectd.
         */
        virtual void introspect(Property<char> &v) = 0;

        /**
         * introspect a property of type int.
         * @param v The property to be introspectd.
         */
        virtual void introspect(Property<int> &v) = 0;

        /**
         * introspect a property of type unsigned int.
         * @param v The property to be introspectd.
         */
        virtual void introspect(Property<unsigned int> &v) = 0;

        /**
         * introspect a property of type double.
         * @param v The property to be introspectd.
         */
        virtual void introspect(Property<double> &v) = 0;

        /**
         * introspect a property of type string.
         * @param v The property to be introspectd.
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
