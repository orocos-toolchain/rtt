/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  SimpleMarshaller.hpp 

                        SimpleMarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef PI_PROPERTIES_SIMPLE_MARSHALLER
#define PI_PROPERTIES_SIMPLE_MARSHALLER

#include <corelib/Property.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

namespace ORO_CoreLib
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
    class SimpleMarshaller : public Marshaller, public PropertyIntrospection
    {
		static const char TYPECODE_BOOL = 'B';
		static const char TYPECODE_CHAR = 'C';
		static const char TYPECODE_INT  = 'I';
		static const char TYPECODE_DOUBLE = 'D';
		static const char TYPECODE_STRING = 'S';

        public:
            typedef o_stream output_stream;
            typedef o_stream OutputStream;

            SimpleMarshaller(output_stream &os) :
                    _os(os)
            {}

            virtual void flush()
            {}
            
			virtual void serialize(const Property<bool> &v) 
			{ 
				
                _os << TYPECODE_BOOL;
				_os.put(v.getName().size());
				_os	<< v.getName() << ":";
				_os.put( 0x01);
				_os << v.get() << ";";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                _os << TYPECODE_CHAR;
				_os.put(v.getName().size() );
				_os	<< v.getName() <<":";
				_os.put(0x01);
				_os << v.get() << ";";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
				std::stringstream buffer;
                std::string s;
				buffer << v.get();
				buffer >> s;	
                _os << TYPECODE_INT;
				_os.put(v.getName().size());
				_os	<< v.getName() << ":";
				_os.put(s.size());
				_os << s << ";";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                std::string s;
				std::stringstream buffer;
				buffer << v.get();
				buffer >> s;
                _os << TYPECODE_DOUBLE;
				_os.put(v.getName().size());
				_os << v.getName() <<":";
				_os.put(s.size());
				_os	<< s << ";";
			}

			virtual void serialize(const Property<std::string> &v) 
			{
                _os << TYPECODE_STRING;
				_os.put(v.getName().size());
				_os	<< v.getName() <<":";
				_os.put(v.get().size());
				_os << v.get() << ";";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    ++i )
                {
                    (*i)->identify(this);
                }
			}

            virtual void serialize(const Property<PropertyBag> &v) 
			{
				serialize(v.get());
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
				serialize(v.get());
            }

            output_stream &_os;
	};
}
#endif
