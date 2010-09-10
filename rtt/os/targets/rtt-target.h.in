/**
 * This is a helper file in the targets subdirectory
 * It includes the correct target, on behalf of rtt-config.h
 */

#define ORO_RTT_CONFIG_str(s) ORO_RTT_CONFIG__str(s-config.h)
#define ORO_RTT_CONFIG_OS_str(s) ORO_RTT_CONFIG__str(../s/s.h)
#define ORO_RTT_CONFIG__str(s) #s

#define OROCOS_TARGET_HEADER ORO_RTT_CONFIG_str(OROCOS_TARGET)
#define OROCOS_TARGET_HEADER_OS ORO_RTT_CONFIG_OS_str(OROCOS_TARGET)

#include OROCOS_TARGET_HEADER
#include OROCOS_TARGET_HEADER_OS

#undef ORO_RTT_CONFIG_str
#undef ORO_RTT_CONFIG__str
