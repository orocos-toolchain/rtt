#include "ActivityInterface.hpp"
#include "RunnableInterface.hpp"

using namespace RTT;
using namespace RTT::base;

ActivityInterface::ActivityInterface(RunnableInterface* run) : runner(run) {
    if (runner)
        runner->setActivity(this);
    }

ActivityInterface::~ActivityInterface()
{
    if (runner) {
        runner->setActivity(0);
    }
}

bool ActivityInterface::run(RunnableInterface* _r)
{
    if ( this->isActive() )
        return false;
    if (runner) {
        runner->setActivity(0);
    }
    runner = _r;
    if (runner)
        runner->setActivity(this); // pass on our ptr.
    return true;
}


