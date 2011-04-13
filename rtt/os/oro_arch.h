#include "../rtt-config.h"

# if defined(OROBLD_OS_NO_ASM)
#  include "oro_noasm/oro_arch.h"
# else
#  if defined(_MSC_VER)
#   include "oro_msvc/oro_arch.h"
#  elif defined(__GNUC__)
#    if ( _OROBLD_GCC_VERSION < 40100 )
#      if defined( OROBLD_OS_ARCH_i386 )
#         include "oro_i386/oro_arch.h"
#      elif defined( OROBLD_OS_ARCH_x86_64 )
#         include "oro_x86_64/oro_arch.h"
#      elif defined( OROBLD_OS_ARCH_ppc )
#         include "oro_powerpc/oro_arch.h"
#      else
#         error "Unsupported architecture for GCC < 4.1.0"
#      endif
#    else // GCC >= 4.1.0 has intrinsics
#      include "oro_gcc/oro_arch.h"
#    endif
#  else
#   error "Unsupported architecture or compiler"
#  endif
# endif
