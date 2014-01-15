
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

/* Triggering from activities */
TRACEPOINT_EVENT(orocos_rtt, Activity_trigger, TP_ARGS(), TP_FIELDS())

/* Trace state changes in TaskCore */
TRACEPOINT_EVENT(orocos_rtt, TaskContext_configureHook     , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_startHook         , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_prepareUpdateHook , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_updateHook        , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_errorHook         , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_stopHook          , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))
TRACEPOINT_EVENT(orocos_rtt, TaskContext_cleanupHook       , TP_ARGS(unsigned, v, const char *, s), TP_FIELDS(ctf_string(name, s) ctf_integer(unsigned, on, v)))

#define tracepoint_context(provider, name, ...) \
        struct TracepointContext__##provider__##name {            \
            TracepointContext__##provider__##name()               \
            {                                                     \
                tracepoint(provider, name, 1, ## __VA_ARGS__);    \
            }                                                     \
            ~TracepointContext__##provider__##name()              \
            {                                                     \
                tracepoint(provider, name, 0, ## __VA_ARGS__);    \
            }                                                     \
        } __tracepoint_context_##provider__##name;

#endif /* _LTTNG_UST_H */

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./lttng_ust.h

#include <lttng/tracepoint-event.h>

