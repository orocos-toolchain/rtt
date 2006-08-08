
// Include the appropriate functionN header
#define OROCOS_SIGNATURE_FUNCTION_N BOOST_JOIN(boost::function,OROCOS_SIGNATURE_NUM_ARGS)
#define OROCOS_SIGNATURE_FUNCTION_N_HEADER BOOST_JOIN(<boost/function/function,OROCOS_SIGNATURE_NUM_ARGS.hpp>)
#include OROCOS_SIGNATURE_FUNCTION_N_HEADER

// Determine if a comma should follow a listing of the arguments/parameters
#if OROCOS_SIGNATURE_NUM_ARGS == 0
#  define OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS
#else
#  define OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS ,
#endif // OROCOS_NUM_ARGS > 0

// Include user specified template.
#include OROCOS_SIGNATURE_TEMPLATE_HEADER

#undef OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS
#undef OROCOS_SIGNATURE_FUNCTION_N_HEADER
#undef OROCOS_SIGNATURE_FUNCTION_N 

