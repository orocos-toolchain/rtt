#ifndef RTT_OS_TRACES_H
#define RTT_OS_TRACES_H

#include "../rtt-config.h"

#if defined(HAVE_LTTNG_UST) && defined(OROPKG_OS_GNULINUX)
#include <rtt/os/gnulinux/traces/lttng_ust.h>
#else
#error
#define tracepoint(provider, tracepoint, ...)
#define tracepoint_context(provider, tracepoint, ...)
#endif

#endif
