/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CPFMarshaller.hpp 

                        CPFMarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_CPFSERIALIZER
#define PI_PROPERTIES_CPFSERIALIZER

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{
    /**
     * A class for marshalling a property or propertybag into a
     * component property description, following the CORBA 3 standard.
     */
    template<typename output_stream>
    class CPFMarshaller 
    : public Marshaller, public PropertyIntrospection, public StreamProcessor<output_stream>
    {
        public:
        /**
         * Construct a CPFMarshaller.
         */
            CPFMarshaller(output_stream &os) :
                    StreamProcessor<output_stream>(os)
            {
            }

			virtual void serialize(const Property<bool> &v) 
			{ 
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"boolean\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"char\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"long\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
			virtual void serialize(const Property<unsigned int> &v) 
			{ 
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"ulong\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"double\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
			virtual void serialize(const Property<std::string> &v) 
			{
                *(this->s) << "<simple name=\"" << v.getName() << "\" type=\"string\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                *(this->s) <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                           <<"<!DOCTYPE properties SYSTEM \"cpf.dtd\">\n";
                *(this->s) << "<properties>\n";

                for (
                    std::vector<PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
                *(this->s) << "\n</properties>\n";
			}
	
			virtual void serialize(const Property<PropertyBag> &b) 
			{
				PropertyBag v = b.get();
                *(this->s) <<"<struct name=\""<<b.getName()<<"\" type=\""<< v.getType()<< "\">\n"
                           <<"<description>"  <<b.getDescription() << "</description>\n";

                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    ++i )
                {
                    (*i)->identify(this);
                }
                *(this->s) <<"</struct>\n";
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
			{
                this->s->flush();
            }
            
	};
}
#endif
