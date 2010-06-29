#include <rtt/interface/ServiceProvider.hpp>
#include <rtt/plugin/ServicePlugin.hpp>

using namespace RTT;
using namespace interface;

class ServicePluginTest : public ServiceProvider
{
public:
    ServicePluginTest(TaskContext* tc) : ServiceProvider("the_service", tc) {}
};

ORO_SERVICE_NAMED_PLUGIN( ServicePluginTest, "serviceplugin")
