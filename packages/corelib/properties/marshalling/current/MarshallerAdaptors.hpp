#ifndef MARSHALLERADAPTOR_HPP
#define MARSHALLERADAPTOR_HPP


namespace ORO_CoreLib
{

    /**
     * Converts one marshaller type to another marshaller.
     */
    template< class InputStream, class DeMarshaller, class Marshaller, class OutputStream>
    class MarshallerAdaptor
    {
    public:
    };

    
    template< class Marshaller, class OutputStream>
    class PropertyAdaptor
    {
        public:
    };


    /**
     * Contains the required classes for a full marshalling operation.
     */
    template< class OutputStream, class HeaderMarshaller, class BodyMarshaller>
    class MarshallConfiguration
    {
        public:
        MarshallConfiguration( OutputStream& o_stream ) 
            : header_marshaller(o_stream), body_marshaller(o_stream), output(o_stream)
            {}

        HeaderMarshaller& header() { return header_marshaller; }
        BodyMarshaller&   body()   { return body_marshaller;   }
        OutputStream&     stream() { return output;            }
        
        protected:
        HeaderMarshaller header_marshaller;
        BodyMarshaller   body_marshaller;

        OutputStream& output;
    };

    /**
     * Contains the required classes for a full demarshalling operation.
     */
    template< class InputStream, class HeaderDemarshaller, class BodyDemarshaller>
    class DemarshallConfiguration
    {
        public:
        DemarshallConfiguration( InputStream& i_stream ) 
            : header_demarshaller(i_stream), body_demarshaller(i_stream), input(i_stream)
            {}

        HeaderDemarshaller& header() { return header_demarshaller; }
        BodyDemarshaller&   body()   { return body_demarshaller;   }
        InputStream&        stream() { return input;               }
        
        protected:
        HeaderDemarshaller header_demarshaller;
        BodyDemarshaller   body_demarshaller;

        InputStream& input;
    };


}

#endif
