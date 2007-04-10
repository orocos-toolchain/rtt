
#ifndef PROPERTY_DEMARSHALLER
#define PROPERTY_DEMARSHALLER

#include "../Marshaller.hpp"
#include <string>

namespace RTT
{
    /**
     * @brief The default Orocos demarshaller for extracting properties and property bags
     * from a property file.
     * @see PropertyMarshaller to create property files.
     */
    class PropertyDemarshaller
        : public Demarshaller
    {
        Demarshaller* d;
        PropertyDemarshaller(const PropertyDemarshaller& );
    public:
        PropertyDemarshaller( const std::string& filename );
        ~PropertyDemarshaller();
        virtual bool deserialize( PropertyBag &v );
    };
}
#endif
