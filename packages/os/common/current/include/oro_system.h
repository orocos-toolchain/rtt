
/**
 * Header file for including the asm/system.h file
 * which contains the cmpxch function, which we need
 * for multithreaded data exchange.
 * 
 */

#define new _new
#define CONFIG_X86_CMPXCHG
#include <asm/system.h>
#undef new
#undef mb
#undef alternative
#undef alternative_input
#undef wmb
#undef rmb
