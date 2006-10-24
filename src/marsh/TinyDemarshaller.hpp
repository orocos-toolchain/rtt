#ifndef PROPERTIES_TINY_DEMARSHALLER
#define PROPERTIES_TINY_DEMARSHALLER

#include <marsh/Marshaller.hpp>
#include <string>

namespace RTT
{
    /**
     * @brief A TinyXML demarshaller for extracting properties and property bags
     * from a Component Property File (CPF) following the CORBA 3 standard.
     * @see CPFMarshaller to create CPF files.
     */
    class TinyDemarshaller
        : public Demarshaller
    {
        class D;
        D* d;
    public:
        TinyDemarshaller( const std::string& filename );
        ~TinyDemarshaller();
        virtual bool deserialize( PropertyBag &v );
    };
}
#endif
