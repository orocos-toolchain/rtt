#ifndef PI_PROPERTIES_XMLSERIALIZER
#define PI_PROPERTIES_XMLSERIALIZER

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{

	template<typename o_stream>
		class XMLMarshaller 
		: public Marshaller, public PropertyIntrospection, public StreamProcessor<o_stream>
		{
			public:
				typedef o_stream output_stream;
				typedef o_stream OutputStream;

				XMLMarshaller(output_stream &os) :
					StreamProcessor<o_stream>(os)
					{}

				virtual void flush() 
				{
				}

				virtual void serialize(const Property<bool> &v) 
				{ 
					*s << "<bool id=\""<< v.getName() 
						<<"\" description=\"" << v.getDescription() <<"\">"
						<< v.get() << "</bool>\n";
				}

				virtual void serialize(const Property<char> &v) 
				{ 
					*s << "<char id=\""<< v.getName() 
						<<"\" description=\"" << v.getDescription() <<"\">"
						<< v.get() << "</char>\n";
				}
				virtual void serialize(const Property<int> &v) 
				{ 
					*s << "<int id=\""<< v.getName() 
						<<"\" description=\"" << v.getDescription() <<"\">"
						<< v.get() << "</int>\n";
				}

				virtual void serialize(const Property<double> &v) 
				{
					*s << "<double id=\""<< v.getName() 
						<<"\" description=\"" << v.getDescription() <<"\">"
						<< v.get() << "</double>\n";
				}
				virtual void serialize(const Property<std::string> &v) 
				{
					*s << "<string id=\""<< v.getName() 
						<<"\" description=\"" << v.getDescription() <<"\">"
						<< v.get() << "</string>\n";
				}

				virtual void serialize(const PropertyBag &v, std::string name)
				{
					*s <<"<bag type=\""<<v.getType()<<"\" name=\"" << name << "\">"<< std::endl;
					for (
							PropertyBag::const_iterator i = v.getProperties().begin();
							i != v.getProperties().end();
							i++ )
					{
						(*i)->identify(this);
					}
					*s <<"</bag>\n";
				}
				
				virtual void serialize(const PropertyBag &v) 
				{
					*s <<"<bag type=\""<<v.getType()<<"\">"<< std::endl;
					for (
							PropertyBag::const_iterator i = v.getProperties().begin();
							i != v.getProperties().end();
							i++ )
					{
						(*i)->identify(this);
					}
					*s <<"</bag>\n";
				}
				virtual void serialize(const Property<PropertyBag> &v) 
				{
					serialize(v.get(), v.getName());
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
					serialize(v.get(),v.getName());
				}

		};
}
#endif
