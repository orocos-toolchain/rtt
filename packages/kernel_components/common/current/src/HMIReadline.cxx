
#pragma implementation
#include "kernel_components/HMIReadline.hpp"

namespace ORO_ControlKernel
{
#if 1
    std::vector<std::string> HMIReadline::completes;
    std::vector<std::string>::iterator HMIReadline::complete_iter;
    std::string HMIReadline::component;
    std::string HMIReadline::method;
    std::string HMIReadline::datasource;
    std::string HMIReadline::text;
    ExecutionExtension* HMIReadline::ee = 0;
#endif
}
