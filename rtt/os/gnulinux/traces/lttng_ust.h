
#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER orocos_rtt

#if !defined(_LTTNG_UST_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _LTTNG_UST_H

#include <lttng/tracepoint.h>

/* Allow to mapping a thread id to a name */
TRACEPOINT_EVENT
(
    /* provider name */
    orocos_rtt,
    /* tracepoint name */
    thread_init,
    /* arguments passed to the tracepoint */
    TP_ARGS(const char *, n),
    /* the fields */
    TP_FIELDS
    (
        ctf_string(name, n)
    )
)

/* Allow to emulate a scope */
TRACEPOINT_EVENT
(
    /* provider name */
    orocos_rtt,
    /* tracepoint name */
    thread_scope,
    /* arguments passed to the tracepoint */
    TP_ARGS(unsigned, v),
    /* the fields */
    TP_FIELDS
    (
        ctf_integer(unsigned, value, v)
    )
)

#endif /* _LTTNG_UST_H */

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./lttng_ust.h

#include <lttng/tracepoint-event.h>

