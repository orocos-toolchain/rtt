#define OROCOS_SIGNATURE_NUM_ARGS 4
#define OROCOS_SIGNATURE_TEMPLATE_PARMS typename T1, typename T2, typename T3, typename T4
#define OROCOS_SIGNATURE_TEMPLATE_ARGS T1, T2, T3, T4
#define OROCOS_SIGNATURE_PARMS arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4
#define OROCOS_SIGNATURE_ARGS a1, a2, a3, a4
#define OROCOS_SIGNATURE_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;typedef T4 arg4_type;
#define OROCOS_SIGNATURE_CONNECTION_TYPEDEFS typedef typename function_type::arg1_type arg1_type;typedef typename function_type::arg2_type arg2_type;typedef typename function_type::arg3_type arg3_type;typedef typename function_type::arg4_type arg4_type;

#include "signature_template.hpp"

#undef OROCOS_SIGNATURE_CONNECTION_TYPEDEFS
#undef OROCOS_SIGNATURE_ARG_TYPES
#undef OROCOS_SIGNATURE_ARGS
#undef OROCOS_SIGNATURE_PARMS
#undef OROCOS_SIGNATURE_TEMPLATE_ARGS
#undef OROCOS_SIGNATURE_TEMPLATE_PARMS
#undef OROCOS_SIGNATURE_NUM_ARGS

