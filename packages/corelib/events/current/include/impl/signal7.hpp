
#ifndef OROCOS_SIGNAL_SIGNAL7_HEADER
#define OROCOS_SIGNAL_SIGNAL7_HEADER

#define OROCOS_SIGNAL_NUM_ARGS 7
#define OROCOS_SIGNAL_TEMPLATE_PARMS typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7
#define OROCOS_SIGNAL_TEMPLATE_ARGS T1, T2, T3, T4, T5, T6, T7
#define OROCOS_SIGNAL_PARMS T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7
#define OROCOS_SIGNAL_ARGS a1, a2, a3, a4, a5, a6, a7
#define OROCOS_SIGNAL_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;typedef T4 arg4_type;typedef T5 arg5_type;typedef T6 arg6_type;typedef T7 arg7_type;
#define OROCOS_SIGNAL_CONNECTION_TYPEDEFS typedef slot_function::arg1_type arg1_type;typedef slot_function::arg2_type arg2_type;typedef slot_function::arg3_type arg3_type;typedef slot_function::arg4_type arg4_type;typedef slot_function::arg5_type arg5_type;typedef slot_function::arg6_type arg6_type;typedef slot_function::arg7_type arg7_type;

#include "signal_template.hpp"

#undef OROCOS_SIGNAL_CONNECTION_TYPEDEFS
#undef OROCOS_SIGNAL_ARG_TYPES
#undef OROCOS_SIGNAL_ARGS
#undef OROCOS_SIGNAL_PARMS
#undef OROCOS_SIGNAL_TEMPLATE_ARGS
#undef OROCOS_SIGNAL_TEMPLATE_PARMS
#undef OROCOS_SIGNAL_NUM_ARGS

#endif // OROCOS_SIGNAL_SIGNAL7_HEADER
