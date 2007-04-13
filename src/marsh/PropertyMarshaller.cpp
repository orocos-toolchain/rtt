#include "PropertyMarshaller.hpp"
#include "rtt-config.h"

#ifdef ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#include ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
#endif

#include <Logger.hpp>
#include <fstream>

namespace RTT
{
    PropertyMarshaller::PropertyMarshaller(const std::string& filename)
        : m( 0 )
    {
        Logger::In in("PropertyMarshaller");
#ifdef ORODAT_CORELIB_PROPERTIES_MARSHALLING_INCLUDE
        m = new OROCLS_CORELIB_PROPERTIES_MARSHALLING_DRIVER<std::ostream>( filename );
#else
        log(Error) << "Orocos RTT was configured without Property Marshalling support !"<<endlog();
#endif
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

