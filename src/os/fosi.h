#include "../rtt-config.h"

#ifdef OROPKG_OS_GNULINUX
 #include "gnulinux/fosi.h"
#endif

#ifdef OROPKG_OS_LXRT
 #include "lxrt/fosi.h"
#endif


#ifdef OROPKG_OS_XENOMAI
 #include "xenomai/fosi.h"
#endif
