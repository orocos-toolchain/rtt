#include "kernelinterface.hpp"

namespace ExecutionClient
{
    using namespace ORO_ControlKernel;
  KernelInterface* KernelInterface::sinstance = 0;

  void KernelInterface::setKernelInterfaceInstance( KernelInterface* kint )
  {
    delete sinstance;
    sinstance = kint;
  }

  KernelInterface* KernelInterface::instance()
  {
    return sinstance;
  }

  KernelInterface::~KernelInterface()
  {
  }
}
