#ifndef ORO_APPLICATION_SERVER_HPP
#define ORO_APPLICATION_SERVER_HPP

#include <tao/corba.h>

namespace ORO_Corba
{
    /**
     * A class which an provides ORB to
     * the application process.
     */
    struct ApplicationServer
    {
        static CORBA::ORB_var orb;
    };
}

#endif
