#include "GlobalService.hpp"
#include "../plugin/PluginLoader.hpp"

namespace RTT
{
    namespace internal
    {
        static Service::shared_ptr mserv;

        GlobalService::GlobalService()
            : Service( "GlobalService" )
        {
            addOperation("require", &GlobalService::require, this)
                    .doc("Require that a certain service is loaded in the global service.")
                    .arg("service_name","The name of the service to load globally.");
        }

        GlobalService::~GlobalService()
        {
        }

        Service::shared_ptr GlobalService::Instance() {
            if ( !mserv ) {
                mserv.reset( new GlobalService() );
            }
            return mserv;
        }
        void GlobalService::Release() {
            mserv.reset();
        }

        bool GlobalService::require(const std::string servicename) {
            return hasService(servicename) || plugin::PluginLoader::Instance()->loadService(servicename, 0); // load globally.
        }
    }

}
