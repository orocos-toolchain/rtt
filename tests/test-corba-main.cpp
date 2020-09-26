#include "rtt/os/main.h"
#include "rtt/Logger.hpp"
#include "rtt/transports/corba/TaskContextServer.hpp"
#include "rtt/transports/corba/TaskContextProxy.hpp"

using namespace RTT;
using namespace RTT::detail;


int ORO_main(int argc, char** argv)
{
    TaskContextServer::InitOrb(argc, argv);
    TaskContextServer::ThreadOrb();
    sleep(1);
    TaskContextServer::ShutdownOrb(true);
    TaskContextServer::DestroyOrb();
    return 0;
}
