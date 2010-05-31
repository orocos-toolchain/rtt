#ifndef ORO_RTT_marsh_FWD_HPP
#define ORO_RTT_marsh_FWD_HPP

namespace RTT {
    class Marshalling;
    namespace marsh {
        class CPFDemarshaller;
        class DemarshallInterface;
        class MarshallInterface;
        class MarshallingService;
        class PropertyBagIntrospector;
        class PropertyDemarshaller;
        class PropertyLoader;
        class PropertyMarshaller;
        class SAX2XMLRPCHandler;
        class TinyDemarshaller;
        class XMLRPCDemarshaller;
        template<class Stream>
        class StreamProcessor;
        template<typename output_stream>
        class XMLRPCMarshaller;
    }
    namespace detail {
        using namespace marsh;
    }
}
#endif
