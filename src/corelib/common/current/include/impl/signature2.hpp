#define OROCOS_SIGNATURE_NUM_ARGS 2
#define OROCOS_SIGNATURE_TEMPLATE_PARMS typename T1, typename T2
#define OROCOS_SIGNATURE_TEMPLATE_ARGS T1, T2
#define OROCOS_SIGNATURE_PARMS arg1_type a1, arg2_type a2
#define OROCOS_SIGNATURE_ARGS a1, a2
#define OROCOS_SIGNATURE_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;
#define OROCOS_SIGNATURE_TYPEDEFS typedef typename function_type::arg1_type arg1_type;typedef typename function_type::arg2_type arg2_type;

#include "signature_template.hpp"

#undef OROCOS_SIGNATURE_TYPEDEFS
#undef OROCOS_SIGNATURE_ARG_TYPES
#undef OROCOS_SIGNATURE_ARGS
#undef OROCOS_SIGNATURE_PARMS
#undef OROCOS_SIGNATURE_TEMPLATE_ARGS
#undef OROCOS_SIGNATURE_TEMPLATE_PARMS
#undef OROCOS_SIGNATURE_NUM_ARGS
