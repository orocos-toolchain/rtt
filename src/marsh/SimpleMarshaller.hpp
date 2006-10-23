/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  SimpleMarshaller.hpp 

                        SimpleMarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_SIMPLE_MARSHALLER
#define PI_PROPERTIES_SIMPLE_MARSHALLER

#include <Property.hpp>
#include <PropertyIntrospection.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

namespace RTT
{

	/**
	 * A simple marshaller which flattens your property bag into simple
	 * types. The representation has the following structure:
     * <br>
	 * <i>typecode, name length, name, ':', value length, value, ';'</i>
	 * 
	 * For example, to declare a string with contents "value":<br>
	 * <i>Sktest:lvalue;</i><br>
	 * Except that the <i>k</i> and <i>l</i> would be replaced by byte values four and
	 * five.
	 *
	 * The <i>typecodes</i> are:
	 *   - B: booleans
	 *   - C: single characters
	 *   - I: ASCII-represented integers
	 *   - D: ASCII-represented doubles
	 *   - S: variable length strings 
	 *
	 * The <i>name length</i> is a single byte and thus limits the length
	 * of names to 255 characters. (Zero means zero length.)
	 * 
	 * The <i>name</i> is an ASCII string representing the name of the
	 * property.
	 *
	 * The <i>value length</i> is a single byte and thus limits the length
	 * of names to 255 characters. (Zero means zero length.)
	 *
	 *
	 */
    template<typename o_stream>
    class SimpleMarshaller
        : public Marshaller,
          protected PropertyIntrospection
    {
		static const char TYPECODE_BOOL = 'B';
		static const char TYPECODE_CHAR = 'C';
		static const char TYPECODE_INT  = 'I';
		static const char TYPECODE_UINT  = 'U';
		static const char TYPECODE_DOUBLE = 'D';
		static const char TYPECODE_STRING = 'S';

        template< class T >
        void writeOut(const char tc, const Property<T> &v) 
        { 
            std::stringstream buffer;
            buffer << v.get();
            _os << tc;
            _os.put(v.getName().size());
            _os	<< v.getName() << ":";
            _os.put( buffer.str().size() );
            _os << buffer.str() << ";";
        }

        virtual void introspect(PropertyBase* pb)
        {
            PropertyIntrospection::introspect( pb );
        }

        virtual void introspect(Property<bool> &v) 
        { 
            writeOut(TYPECODE_BOOL, v);
        }

        virtual void introspect(Property<char> &v) 
        { 
            writeOut(TYPECODE_CHAR, v);
        }

        virtual void introspect(Property<int> &v) 
        { 
            writeOut(TYPECODE_INT, v);
        }
			
        virtual void introspect(Property<unsigned int> &v) 
        { 
            writeOut(TYPECODE_UINT, v);
        }
			
        virtual void introspect(Property<double> &v) 
        {
            writeOut(TYPECODE_DOUBLE, v);
        }

        virtual void introspect(Property<std::string> &v) 
        {
            writeOut(TYPECODE_STRING, v);
        }
			
        virtual void introspect(Property<PropertyBag> &v) 
        {
            serialize(v.get());
        }
        typedef o_stream output_stream;
        typedef o_stream OutputStream;
        output_stream &_os;
    public:

        SimpleMarshaller(output_stream &os) :
            _os(os)
        {}

        virtual void flush()
        {}
          
        virtual void serialize(PropertyBase* p)
        {
            p->identify(this);
        }

        virtual void serialize(const PropertyBag &v) 
        {
            v.identify(this);
        }
	};
}
#endif
