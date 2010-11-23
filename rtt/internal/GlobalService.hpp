
#ifndef ORO_GLOBALSERVICE_HPP_
#define ORO_GLOBALSERVICE_HPP_

#include "../Service.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{

    namespace internal
    {

        /**
         * A process-wide services that hosts services not local
         * to a single component.
         */
        class GlobalService: public RTT::Service
        {
            GlobalService();
        public:
            virtual ~GlobalService();
            RTT_API static Service::shared_ptr Instance();
            RTT_API static void Release();

            /**
             * Require that a certain service is loaded in the global service.
             * You may require the same \a servicename multiple times. This function
             * will each subsequent time return the same value as the first time
             * it was called with \a servicename.
             * @param servicename The name of the service to load
             * @return true if the service was loaded, false if the servicename was
             * unknown.
             */
            bool require(const std::string servicename);
        };

    }

}

#endif /* ORO_GLOBALSERVICE_HPP_ */
