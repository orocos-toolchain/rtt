#include <rtt-config.h>

#ifdef OROPKG_OS_GNULINUX
 #include "gnulinux/fosi_internal.hpp"
#endif

#ifdef OROPKG_OS_LXRT
 #include "lxrt/fosi_internal.hpp"
#endif


#ifdef OROPKG_OS_XENOMAI
 #include "xenomai/fosi_internal.hpp"
#endif
