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
    using  std::string;

    /**
     * A class for marshalling a property or propertybag into a
     * component property description, following the CORBA 3 standard.
     */
    template<typename output_stream>
    class CPFMarshaller 
    : public Marshaller, public PropertyIntrospection, public StreamProcessor<output_stream>
    {
        public:
            CPFMarshaller(output_stream &os) :
                    StreamProcessor<output_stream>(os)
            {}

			virtual void serialize(const Property<bool> &v) 
			{ 
                *s << "<simple name=\"" << v.getName() << "\" type=\"boolean\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                *s << "<simple name=\"" << v.getName() << "\" type=\"char\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}

			virtual void serialize(const Property<int> &v) 
			{ 
                *s << "<simple name=\"" << v.getName() << "\" type=\"long\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                *s << "<simple name=\"" << v.getName() << "\" type=\"double\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
			virtual void serialize(const Property<std::string> &v) 
			{
                *s << "<simple name=\"" << v.getName() << "\" type=\"string\">"
					<< "<description>"<<v.getDescription() << "</description>"
					<< "<value>" << v.get() << "</value></simple>\n";
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                *s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
                *s << "<properties>\n";

                for (
                    std::vector<PropertyBase*>::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
                *s << "\n</properties>\n";
			}
	
			virtual void serialize(const Property<PropertyBag> &b) 
			{
				PropertyBag v = b.get();
                *s <<"<struct name=\""<<b.getName()<<"\" type=\""<< v.getType()<< "\">\n";
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    ++i )
                {
                    (*i)->identify(this);
                }
                *s <<"</struct>\n";
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
}
#endif
