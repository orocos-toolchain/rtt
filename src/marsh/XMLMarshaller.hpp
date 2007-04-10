/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  XMLMarshaller.hpp 

                        XMLMarshaller.hpp -  description
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
 
#ifndef PI_PROPERTIES_XMLSERIALIZER
#define PI_PROPERTIES_XMLSERIALIZER

#include "../Property.hpp"
#include "../PropertyIntrospection.hpp"
#include "StreamProcessor.hpp"

namespace RTT
{

    /**
     * Marshalls a PropertyBag to a non standard XML format.
     * @warning: No Orocos class exists yet which can read the XML data back in.
     */
	template<typename o_stream>
    class XMLMarshaller 
		: public Marshaller, 
          protected PropertyIntrospection, 
          public StreamProcessor<o_stream>
    {
    protected:
        virtual void introspect(PropertyBase* pb)
        {
            PropertyIntrospection::introspect( pb );
        }

        virtual void introspect(Property<bool> &v) 
        { 
            *(this->s) << "<bool id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</bool>\n";
        }

        virtual void introspect(Property<char> &v) 
        { 
            *(this->s) << "<char id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</char>\n";
        }
        virtual void introspect(Property<int> &v) 
        { 
            *(this->s) << "<int id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</int>\n";
        }

        virtual void introspect(Property<unsigned int> &v) 
        { 
            *(this->s) << "<uint id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</uint>\n";
        }

        virtual void introspect(Property<double> &v) 
        {
            *(this->s) << "<double id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</double>\n";
        }
        virtual void introspect(Property<std::string> &v) 
        {
            *(this->s) << "<string id=\""<< v.getName() 
                       <<"\" description=\"" << v.getDescription() <<"\">"
                       << v.get() << "</string>\n";
        }

        virtual void introspect(Property<PropertyBag> &v) 
        {
            introspect(v.get(), v.getName());
        }

        virtual void introspect(const PropertyBag &v, std::string name)
        {
            *(this->s) <<"<bag type=\""<<v.getType()<<"\" name=\"" << name << "\">"<< std::endl;
            for (
                 PropertyBag::const_iterator i = v.getProperties().begin();
                 i != v.getProperties().end();
                 i++ )
                {
                    (*i)->identify(this);
                }
            *(this->s) <<"</bag>\n";
        }
    public:
        typedef o_stream output_stream;
        typedef o_stream OutputStream;

        XMLMarshaller(output_stream &os) :
            StreamProcessor<o_stream>(os)
        {}

        virtual void flush() {}

        virtual void serialize(PropertyBase* p) {
            p->identify(this);
        }
				
        virtual void serialize(const PropertyBag &v) 
        {
            *(this->s) <<"<bag type=\""<<v.getType()<<"\">"<< std::endl;
            for (
                 PropertyBag::const_iterator i = v.getProperties().begin();
                 i != v.getProperties().end();
                 i++ )
                {
                    (*i)->identify(this);
                }
            *(this->s) <<"</bag>\n";
        }
    };
}
#endif
