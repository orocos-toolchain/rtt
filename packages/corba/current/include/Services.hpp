#ifndef ORO_CORBA_SERVICES_HPP
#define ORO_CORBA_SERVICES_HPP

#include "orbsvcs/CosNamingC.h"


namespace RTT
{ namespace Corba {

    /**
     * Helper class which locates common CORBA services.
     */
    class Services
    {
    protected:
        //! Handle to the name service.
        static CosNaming::NamingContext_var naming_context_;

    public:

        /**
         * Set this variable to false if the Orb may not be
         * shutdown. The default is \a true.
         */
        static bool MayShutdown;

        static CosNaming::NamingContext_ptr getNamingContext();
    };

}
}

#endif
