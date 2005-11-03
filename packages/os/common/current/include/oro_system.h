#include <pkgconf/os.h>
#ifdef OROBLD_OS_AGNOSTIC
# if defined(OROBLD_OS_ARCH_i386)
#  include "oro_i386/oro_system.h"
# elif defined(OROBLD_OS_ARCH_x86_64)
#  include "oro_x86_64/oro_system.h"
# else
#  error "Unknown Processor Architecture"
# endif
#else
#define new _new
#include "asm/bitops.h"
#include "asm/system.h"
#undef new
#undef mb
#undef alternative
#undef alternative_input
#undef wmb
#undef rmb
#endif
