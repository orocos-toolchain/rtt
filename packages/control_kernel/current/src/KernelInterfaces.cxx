#include "control_kernel/KernelInterfaces.hpp"

using namespace ORO_ControlKernel;
using namespace ORO_CoreLib;

NameServer<ExtensionInterface*> ExtensionInterface::nameserver;

bool ComponentBaseInterface::enableAspect(KernelBaseFunction* e)
{
    kern = e;
    kern->addComponent(this);
    componentLoaded();
    return true;
}

void ComponentBaseInterface::disableAspect()
{
    if ( inKernel() )
    {
        kern->removeComponent(this);
        kern = 0;
        componentUnloaded();
    }
}
