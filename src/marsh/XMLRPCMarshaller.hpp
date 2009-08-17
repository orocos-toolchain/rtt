/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  XMLRPCMarshaller.hpp

                        XMLRPCMarshaller.hpp -  description
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

#ifndef PI_PROPERTIES_XMLRPCSERIALIZER
#define PI_PROPERTIES_XMLRPCSERIALIZER

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../Property.hpp"
#include "../base/PropertyIntrospection.hpp"
#include "Marshaller.hpp"
#include "StreamProcessor.hpp"


namespace RTT
{ namespace marsh {

    using namespace std;

    /**
     * Outputs a Property or PropertyBag into a text stream in the
     * XML-RPC format.
     */
    template<typename output_stream>
    class XMLRPCMarshaller
        : public Marshaller, public base::PropertyIntrospection,
        public StreamProcessor<output_stream>
    {
        public:
            XMLRPCMarshaller(output_stream &os) :
                    StreamProcessor<output_stream>(os)
            {}

			virtual void serialize(const Property<bool> &v)
			{
                *(this->s) << "<name>" << v.getName() << "</name>"
					<< "<value><boolean>" << v.get() << "</boolean></value>\n";
			}

			virtual void serialize(const Property<char> &v)
			{
                *(this->s) << "<name>" << v.getName() << "</name>"
					<< "<value><string>" << v.get() << "</string></value>\n";
			}

			virtual void serialize(const Property<int> &v)
			{
                *(this->s) << "<name>" << v.getName() << "</name>"
					<< "<value><int>" << v.get() << "</int></value>\n";
			}

			virtual void serialize(const Property<double> &v)
			{
                *(this->s) << "<name>" << v.getName() << "</name>"
					<< "<value><double>" << v.get() << "</double></value>\n";
			}

			virtual void serialize(const Property<std::string> &v)
			{
                *(this->s) << "<name>" << v.getName() << "</name>"
					<< "<value><string>" << v.get() << "</string></value>\n";
			}

            virtual void serialize(const PropertyBag &v)
			{
                *(this->s) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
                *(this->s) << "<xmlrpc>\n";

                for (
                    std::vector<base::PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
                *(this->s) << "\n</xmlrpc>\n";
			}
            virtual void serialize(const Property<PropertyBag> &b)
			{
                //   cout << "double: " << v;
                *(this->s) <<"<struct><name>"<<b.getName()<<"</name>\n";
				PropertyBag v = b.get();
                for (
                    std::vector<base::PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                	*(this->s) <<"<member>\n";
                    (*i)->identify(this);
	                *(this->s) <<"</member>";
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
	};
}}
#endif
