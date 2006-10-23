/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  INIMarshaller.hpp 

                        INIMarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_INISERIALIZER
#define PI_PROPERTIES_INISERIALIZER

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <Property.hpp>
#include <PropertyIntrospection.hpp>

namespace RTT
{
    /**
     * A Marshaller which writes out the properties in MS .INI format.
     * @warning: No Orocos class exists yet which can read the INI file back in.
     */
    template<typename output_stream>
    class INIMarshaller
        : public Marshaller,
          protected PropertyBagVisitor
    {
    protected:
        virtual void introspect(const PropertyBase* v) 
        { 
            _os << v->getName() << " = " << v << "\n";
        }

        virtual void introspect(const Property<PropertyBag> &b) 
        {
            _os <<"[" << b.getName()<< "]\n";
            b.value().identify(this);
            _os << "\n";
        }

        output_stream &_os;
    public:
        INIMarshaller(output_stream &os) :
            _os(os)
        {}

        virtual void serialize(const PropertyBase* p) {
            p->identify(this);
        }

        virtual void serialize(const PropertyBag &v) 
        {
            _os << "[properties]\n";
            v.identify(this);
            _os << "\n";
        }
	
		virtual void flush()
        {
            _os.flush();
        }
	};
}
#endif
