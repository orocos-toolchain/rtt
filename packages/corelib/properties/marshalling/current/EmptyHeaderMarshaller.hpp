 
#ifndef PI_PROPERTIES_EMPTYHEADER_SERIALIZER
#define PI_PROPERTIES_EMPTYHEADER_SERIALIZER

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A Dummy Empty Header Marshaller.
     */
    template<typename o_stream>
    class EmptyHeaderMarshaller 
        : public Marshaller,
          public StreamProcessor<o_stream>
    {
    public:
        typedef o_stream output_stream;
        typedef o_stream OutputStream;
            
        EmptyHeaderMarshaller(output_stream &os) :
            StreamProcessor<o_stream>(os)
        {
        }

        virtual ~EmptyHeaderMarshaller() {}
            
        virtual void flush() {}
            
        virtual void serialize(const Property<bool> &v) 
        { 
        }

        virtual void serialize(const Property<char> &v) 
        { 
        }

        virtual void serialize(const Property<int> &v) 
        { 
        }
			
        virtual void serialize(const Property<double> &v) 
        {
        }

        virtual void serialize(const Property<std::string> &v) 
        {
        }
			
        virtual void serialize(const Property<PropertyBag> &v) 
        {
        }

        virtual void serialize(const PropertyBag &v) 
        {
        }
	};
}
#endif
