#include "os/ThreadInterface.hpp"
#include "os/threads.hpp"

using namespace ORO_OS;

ThreadInterface::ThreadInterface()
{
    threads.inc();
    threadnb = threads.read();
}

ThreadInterface::~ThreadInterface()
{
    // we can not decrement, if a thread is
    // destroyed, the number becomes unused.
    //threads.dec();
}
