
#include "PropertyDemarshaller.hpp"
#include "rtt-config.h"

#include ORODAT_CORELIB_PROPERTIES_DEMARSHALLING_INCLUDE

#include <Property.hpp>
#include <Logger.hpp>

namespace RTT
{
    using namespace detail;

    PropertyDemarshaller::PropertyDemarshaller( const std::string& filename )
        : d( 0 )
    {
        Logger::In in("PropertyDemarshaller");
        try {
            d = new OROCLS_CORELIB_PROPERTIES_DEMARSHALLING_DRIVER(filename);
        } catch(...) {
            log(Error) << "Could not open file: " << filename <<  endlog();
        }
    }

    PropertyDemarshaller::~PropertyDemarshaller()
    {
        delete d;
    }
    
    bool PropertyDemarshaller::deserialize( PropertyBag &v )
    {
        Logger::In in("PropertyDemarshaller");
        if (d)
            return d->deserialize(v);
        return false;
    }
}

