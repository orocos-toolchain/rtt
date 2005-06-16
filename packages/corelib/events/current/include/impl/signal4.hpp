
#ifndef OROCOS_SIGNAL_SIGNAL4_HEADER
#define OROCOS_SIGNAL_SIGNAL4_HEADER

#define OROCOS_SIGNAL_NUM_ARGS 4
#define OROCOS_SIGNAL_TEMPLATE_PARMS typename T1, typename T2, typename T3, typename T4
#define OROCOS_SIGNAL_TEMPLATE_ARGS T1, T2, T3, T4
#define OROCOS_SIGNAL_PARMS arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4
#define OROCOS_SIGNAL_ARGS a1, a2, a3, a4
#define OROCOS_SIGNAL_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;typedef T4 arg4_type;
#define OROCOS_SIGNAL_CONNECTION_TYPEDEFS typedef typename slot_function::arg1_type arg1_type;typedef typename slot_function::arg2_type arg2_type;typedef typename slot_function::arg3_type arg3_type;typedef typename slot_function::arg4_type arg4_type;

#include "signal_template.hpp"

#undef OROCOS_SIGNAL_CONNECTION_TYPEDEFS
#undef OROCOS_SIGNAL_ARG_TYPES
#undef OROCOS_SIGNAL_ARGS
#undef OROCOS_SIGNAL_PARMS
#undef OROCOS_SIGNAL_TEMPLATE_ARGS
#undef OROCOS_SIGNAL_TEMPLATE_PARMS
#undef OROCOS_SIGNAL_NUM_ARGS

#endif // OROCOS_SIGNAL_SIGNAL4_HEADER
