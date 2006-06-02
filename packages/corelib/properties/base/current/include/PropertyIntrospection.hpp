/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  PropertyIntrospection.hpp 

                        PropertyIntrospection.hpp -  description
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
 
#ifndef PI_PROPERTYINTROSPECTION_HPP
#define PI_PROPERTYINTROSPECTION_HPP

#include <string>
#include "Property.hpp"
#include "Logger.hpp"

namespace ORO_CoreLib
{

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
     */
    class PropertyBagVisitor
    {
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
    class PropertyIntrospection
        : public PropertyBagVisitor
    {
    protected:
        virtual void introspect(PropertyBase* p )
        {
            p->identify( this );
        }

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
         * @see TemplateTypeInfo
         */
        template< class T >
        void introspect(Property<T> &v )
        {
            Property<PropertyBag> res(v.getName(), v.getDescription() );
            if ( v.getTypeInfo()->decomposeType( v.getDataSource(), res.value() ) ) {
                this->introspect( res );
                deletePropertyBag( res.value() );
            }else
                Logger::log() << Logger::Warning<< "Could not decompose "<< v.getName() << Logger::endl;
            // drop.
        }
	};
}

#endif
