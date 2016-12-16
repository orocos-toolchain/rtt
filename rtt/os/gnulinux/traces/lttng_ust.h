
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

#ifndef __cplusplus
#define TRACEPOINT_CONTEXT(provider, event, ...) \
    TRACEPOINT_EVENT(provider, event, TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
#else
#include "../../../rtt/rt_string.hpp"
#define TRACEPOINT_CONTEXT(provider, event, ...) \
    TRACEPOINT_EVENT(provider, event, TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v))) \
    struct TracepointContext__##provider__##event {            \
        const char* name; \
        TracepointContext__##provider__##event(const char* name) \
            : name(name)                                      \
        {                                                     \
            tracepoint(provider, event, 1, name);             \
        }                                                     \
        ~TracepointContext__##provider__##event()             \
        {                                                     \
            tracepoint(provider, event, 0, name);             \
        }                                                     \
    };
#endif

#define TRACEPOINT_NAMED_EVENT(provider, event) \
        TRACEPOINT_EVENT(provider, event, TP_ARGS(const char *, s), TP_FIELDS(ctf_string(name, s)))

/* Triggering from activities */
TRACEPOINT_NAMED_EVENT(orocos_rtt, Activity_trigger)
TRACEPOINT_NAMED_EVENT(orocos_rtt, InputPort_clear)
TRACEPOINT_NAMED_EVENT(orocos_rtt, OutputPort_write)

/* Trace state changes in TaskCore */
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_configureHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_startHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_prepareUpdateHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_updateHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_errorHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_stopHook)
TRACEPOINT_CONTEXT(orocos_rtt, TaskContext_cleanupHook)

#define tracepoint_context(provider, event, name) \
   TracepointContext__##provider__##event __tracepoint_context_##provider__##event(name);

#endif /* _LTTNG_UST_H */

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./lttng_ust.h

#include <lttng/tracepoint-event.h>

