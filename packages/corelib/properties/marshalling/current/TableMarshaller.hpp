#ifndef PI_PROPERTIES_TABLESERIALIZER
#define PI_PROPERTIES_TABLESERIALIZER

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{

    /**
     * A Marshaller for generating a stream of numbers, ordered in
     * columns. A new row is created on each flush() command. The
     * TableHeaderMarshaller can create the appropriate heading for
     * the columns.
     */
    template<typename o_stream>
    class TableMarshaller 
    : public Marshaller, public PropertyIntrospection, public StreamProcessor<o_stream>
    {
        std::string sep;
        public:
            typedef o_stream output_stream;
            typedef o_stream OutputStream;
            
            TableMarshaller(output_stream &os, std::string _sep=" ") :
                    StreamProcessor<o_stream>(os), sep(_sep)
            {}

            virtual ~TableMarshaller() {}

			virtual void serialize(const Property<bool> &v) 
			{ 
                *s <<sep; 
                s->width( v.getName().length() );
                *s<< v.get();
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                *s <<sep; 
                s->width( v.getName().length() );
                *s<< v.get();
			}
			virtual void serialize(const Property<int> &v) 
			{ 
                *s <<sep; 
                s->width( v.getName().length() );
                *s<< v.get();
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                *s <<sep; 
                s->width( v.getName().length() );
                *s<< v.get();
            }
			virtual void serialize(const Property<std::string> &v) 
			{
                *s <<sep; 
                s->width( v.getName().length() );
                *s<< v.get();
			}
			
            virtual void serialize(const PropertyBag &v) 
			{
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    (*i)->identify(this);
                }
			}

            virtual void serialize(const Property<PropertyBag> &v) 
			{
				serialize(v.get());
            }
            
            virtual void flush() 
            {
                // TODO : buffer for formatting and flush here.
                *s <<sep<<std::endl;
                
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
	};
}
#endif
