
#pragma implementation
#include "control_kernel/ComponentInterfaces.hpp"
#include "control_kernel/KernelInterfaces.hpp"

using namespace ORO_ControlKernel;

bool ComponentBaseInterface::enableAspect(KernelBaseFunction* e)
{
    kern = e;
    return kern->addComponent(this);
}

void ComponentBaseInterface::disableAspect()
{
    if ( inKernel() )
        {
            kern->removeComponent(this);
            kern = 0;
        }
}

