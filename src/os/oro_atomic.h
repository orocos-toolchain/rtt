/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  oro_atomic.h 

                        oro_atomic.h -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#include "../rtt-config.h"
#if defined(OROBLD_OS_AGNOSTIC) || defined(OROBLD_OS_NO_ASM)
# if defined(OROBLD_OS_NO_ASM)
#  include "oro_noasm/oro_atomic.h"
# else
#  if defined( OROBLD_OS_ARCH_i386 )
#	if defined(_MSC_VER)
#	 include "oro_msvc/oro_atomic.h"
#	else
#    include "oro_i386/oro_atomic.h"
#   endif
#  elif defined( OROBLD_OS_ARCH_x86_64 )
#   include "oro_x86_64/oro_atomic.h"
#  elif defined( OROBLD_OS_ARCH_ppc )
#   include "oro_powerpc/oro_atomic.h"
#  endif
# endif
#else

/**
 * Warning: you're including a system header here, which may
 * not be fit for user-space compilation and may give compile
 * errors, warnings or not working code.
 */
#include <asm/atomic.h>

// Orocos API:
#define oro_atomic_inc atomic_inc
#define oro_atomic_t atomic_t
#define ORO_ATOMIC_INIT ATOMIC_INIT

// These two are Orocos specific:
#define ORO_ATOMIC_SETUP atomic_set
#define ORO_ATOMIC_CLEANUP( v ) 

#define oro_atomic_read atomic_read
#define oro_atomic_set atomic_set
#define oro_atomic_add atomic_add
#define oro_atomic_sub atomic_sub
#define oro_atomic_sub_and_test atomic_sub_and_test
#define oro_atomic_inc atomic_inc
#define oro_atomic_dec atomic_dec
#define oro_atomic_inc_and_test atomic_inc_and_test
#define oro_atomic_dec_and_test atomic_dec_and_test
#define oro_atomic_add_negative atomic_add_negative

#endif
