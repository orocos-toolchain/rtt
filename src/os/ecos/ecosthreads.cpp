#include "os_ecos.h"
#include <rtt/os/threads.hpp>

namespace RTT
{ namespace OS {
    const int LowestPriority = ORONUM_OS_RTOS_LOWEST_PRIORITY;
    const int HighestPriority = ORONUM_OS_RTOS_HIGHEST_PRIORITY;
    const int IncreasePriority = -1;
}}
