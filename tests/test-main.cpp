#include <os/main.h>
#include <Logger.hpp>

int ORO_main(int argc, char** argv)
{
    RTT::log(RTT::Info) << "Hello ORO_main()" << RTT::endlog();
    return 0;
}
