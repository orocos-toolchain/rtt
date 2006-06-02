/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  Orocos1Marshaller.hpp 

                        Orocos1Marshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_OROCOS1_MARSHALLER
#define PI_PROPERTIES_OROCOS1_MARSHALLER

#include <corelib/Property.hpp>
#include "corelib/PropertyIntrospection.hpp"

#include <sstream>

namespace ORO_CoreLib
{

	/**
	 * A marshaller which generates data compatible with the previous Orocos property system.
	 * @bug Not working yet :-)
	 */
    template<typename output_stream>
    class Orocos1Marshaller
        : public Marshaller,
          protected PropertyIntrospection
    {
    protected:
        output_stream &_os;
        virtual void introspect(const Property<bool> &v) 
        { 
				
            _os << v.getName() 
                <<":1>"
                << v.get() << ";";
        }

        virtual void introspect(const Property<char> &v) 
        { 
            _os << v.getName() 
                <<":1>"
                << v.get() << ";";
        }

        virtual void introspect(const Property<int> &v) 
        { 
            std::stringstream buffer;
            std::string s;
            buffer << v.get();
            buffer >> s;	
            _os << v.getName() 
                <<":"<<s.size() << ">"
                << s << ";";
        }
			
        virtual void introspect(const Property<unsigned int> &v) 
        { 
            std::stringstream buffer;
            std::string s;
            buffer << v.get();
            buffer >> s;	
            _os << v.getName() 
                <<":"<<s.size() << ">"
                << s << ";";
        }
			
        virtual void introspect(const Property<double> &v) 
        {
            std::stringstream buffer;
            std::string s;
            buffer << v.get();
            buffer >> s;
            _os << v.getName() 
                <<":" << s.size() << ">"
                << s << ";";
        }
        virtual void introspect(const Property<std::string> &v) 
        {
            _os << v.getName() 
                <<":"<<v.get().size() << ">"
                << v.get() << ";";
        }
			
        virtual void introspect(const Property<PropertyBag> &v) 
        {
            //   cout << "double: " << v;
            _os << v.getName();
            serialize(v.get());
        }

    public:
        Orocos1Marshaller(output_stream &os) :
            _os(os)
        {}

        virtual void flush() { _os.flush(); }

        virtual void serialize(const PropertyBag &v) 
        {
            _os <<"{";

            v.identify(this);

            _os <<"}";
        }

        virtual void serialize(const PropertyBase* b)
        {
            b->identify(this);
        }
	};
}
#endif
