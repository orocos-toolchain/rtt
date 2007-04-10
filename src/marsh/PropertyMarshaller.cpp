#include "PropertyMarshaller.hpp"
#include "rtt-config.h"

#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE

#include <Logger.hpp>
#include <fstream>

namespace RTT
{
    using namespace detail;

    PropertyMarshaller::PropertyMarshaller(const std::string& filename)
        : m( 0 )
    {
        Logger::In in("PropertyMarshaller");
        m = new OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream>( filename );
    }
    
    PropertyMarshaller::~PropertyMarshaller()
    {
        delete m;
    }

    void PropertyMarshaller::serialize(PropertyBase* v)
    {
        Logger::In in("PropertyMarshaller");
        if (m)
            m->serialize(v);
    }

    void PropertyMarshaller::serialize(const PropertyBag &v)
    {
        Logger::In in("PropertyMarshaller");
        if (m)
            m->serialize(v);
    }

    void PropertyMarshaller::flush()
    {
        Logger::In in("PropertyMarshaller");
        if (m)
            m->flush();
    }
}

