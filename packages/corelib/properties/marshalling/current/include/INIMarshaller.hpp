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
#include "corelib/Property.hpp"
#include "corelib/PropertyIntrospection.hpp"

namespace ORO_CoreLib
{
    template<typename output_stream>
    class INIMarshaller : public Marshaller, public PropertyIntrospection
    {
        public:
            INIMarshaller(output_stream &os) :
                    _os(os)
            {}

			virtual void serialize(const Property<bool> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
			virtual void serialize(const Property<unsigned int> &v) 
			{ 
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
			virtual void serialize(const Property<std::string> &v) 
			{
                _os << v.getName() << " = " << v.get() << "\n";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                _os << "[properties]\n";

                for (
                    std::vector<PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
                _os << "\n";
			}
	
			virtual void serialize(const Property<PropertyBag> &b) 
			{
				PropertyBag v = b.get();
                _os <<"[" << b.getName()<< "]\n";
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    ++i )
                {
                    (*i)->identify(this);
                }
                _os << "\n";
			}

			virtual void introspect(const Property<bool> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<char> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<int> &v) 
			{ 
                serialize(v);
			}
			
			virtual void introspect(const Property<unsigned int> &v) 
			{ 
                serialize(v);
			}
			
			virtual void introspect(const Property<double> &v) 
			{
                serialize(v);
			}

			virtual void introspect(const Property<std::string> &v) 
			{
                serialize(v);
			}
			
            virtual void introspect(const Property<PropertyBag> &v) 
			{
				serialize(v);
            }
		virtual void flush()
			{}
            output_stream &_os;
	};
}
#endif
