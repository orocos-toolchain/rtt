
#pragma implementation
#include "control_kernel/ComponentInterfaces.hpp"
#include "control_kernel/KernelInterfaces.hpp"

using namespace ORO_ControlKernel;

bool ComponentBaseInterface::enableAspect(KernelBaseFunction* e)
{
    kern = e;
    kern->addComponent(this);
    if ( ! componentLoaded() )
        {
            kern->removeComponent(this);
            return false;
        }
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

