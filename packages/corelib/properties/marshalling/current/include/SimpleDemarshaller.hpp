/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  SimpleDemarshaller.hpp 

                        SimpleDemarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_SIMPLE_DEMARSHALLER
#define PI_PROPERTIES_SIMPLE_DEMARSHALLER

#include "Property.hpp"

#ifdef HAVE_VECTOR
#include <vector>
#endif

#ifdef HAVE_MAP
#include <map>
#endif

#ifdef HAVE_STRING
#include <string>
#endif

#ifdef HAVE_IOSTREAM
#include <iostream>
#else
#include "rtstl/rtstl.h"
#endif

namespace ORO_CoreLib
{

	/**
	 * A simple demarshaller.
	 *
	 * @see SimpleMarshaller
	 */
template<typename input_stream>
    class SimpleDemarshaller : public Demarshaller
    {
        static const char TYPECODE_BOOL = 'B';
		static const char TYPECODE_CHAR = 'C';
		static const char TYPECODE_INT = 'I';
		static const char TYPECODE_DOUBLE = 'D';
		static const char TYPECODE_STRING = 'S';

public:
            SimpleDemarshaller(input_stream &is) :
                    _is(is)
            {}
	
            virtual bool deserialize(PropertyBag &v) 
			{
				using namespace std;
#if 0
				char begin_token, end_token;
				_is >> begin_token;
				if(begin_token == '{')
					cerr << "read begin";
#endif
			
				char c;
				while( _is.read(&c,1) )
				{
					unsigned char name_length, value_length;
					char name[256];					
					char value[256];				
					int intvalue;
					double doublevalue;
					_is.read(reinterpret_cast<char*>(&name_length),1);
					_is.read(name,name_length);
					name[name_length] = 0;
					_is.ignore();
					_is.read(reinterpret_cast<char*>(&value_length),1);
					_is.read(value,value_length);
					switch(c)
					{
						case TYPECODE_BOOL:
							if(value[0])
								v.add(new Property<bool>(name,"",false));
							else
								v.add(new Property<bool>(name,"",true));
							cerr << "bool: ";
							_is.ignore();
							break;
						case TYPECODE_CHAR:
							v.add(new Property<char>(name,"",value[0]));
							cerr << "char: ";
							_is.ignore();
							break;
						case TYPECODE_INT:
							value[value_length]=0;
							sscanf(value, "%d", &intvalue);
							v.add(new Property<int>(name,"",intvalue));
							cerr << "int: ";
							_is.ignore();
							break;
						case TYPECODE_DOUBLE:
							value[value_length]=0;
							sscanf(value, "%lf", &doublevalue);
							v.add(new Property<double>(name,"",doublevalue));
							cerr << "double: ";
							_is.ignore();
							break;
						case TYPECODE_STRING:
							value[value_length]=0;
							cerr << value<<"\n";
							v.add(new Property<string>(name,"",value));
							_is.ignore();
							cerr << "string: ";
							break;
                        default:
							cerr << "lost synchro" << endl;
					}
					cerr << "\n"<<(int)name_length<<"\n"<<name<<"\n"<<(int)value_length<<endl;
				}
#if 0
                for (
                    vector<PropertyBase*>::iterator i = v._properties.begin();
                    i != v._properties.end();
                    i++ )
                {
                    (*i)->serialize(*this);
                }
#endif
//                _os <<"</Bag>\n";
	
#if 0		
				_is >> end_token;
				if(end_token == '}')
					cerr << "read end";
#endif
                return true;
			}

			
            input_stream &_is;
			

    };
}
#endif
