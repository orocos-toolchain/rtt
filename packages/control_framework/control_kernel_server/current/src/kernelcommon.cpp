
#include "control_kernel/kernelserver.hpp"
#include "directkernelinterface.hpp"

#include "control_kernel/ExecutionExtension.hpp"
#include "control_kernel/ReportingExtension.hpp"
#include "control_kernel/KernelInterfaces.hpp"

//using namespace std;
using namespace ORO_ControlKernel;
using namespace ExecutionClient;

void setKernel(ControlKernelInterface* k)
{
    KernelInterface::setKernelInterfaceInstance( new DirectKernelInterface(dynamic_cast<ExecutionExtension*>(k),
                                                                           dynamic_cast<ReportingExtension*>(k),
                                                                           dynamic_cast<KernelBaseFunction*>(k) ) );
}



