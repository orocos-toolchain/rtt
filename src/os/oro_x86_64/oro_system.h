/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  oro_system.h 

                        oro_system.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 

/**
 * Header file for including the asm/system.h file
 * which contains the cmpxch function, which we need
 * for multithreaded data exchange.
 * 
 */

#include <rtt-config.h>
#ifdef OROBLD_OS_AGNOSTIC
// do not rely on system headers :
// x86_64 :

// this is required for multi-core cpus
// Export ORO_LOCK_PREFIX, just like the classical bitops.h does.
#ifndef CONFIG_FORCE_UP
#define ORO_LOCK_PREFIX "lock ; "
#else
#define ORO_LOCK_PREFIX ""
#endif

struct oro__xchg_dummy { unsigned long a[100]; };
#define oro__xg(x) ((struct oro__xchg_dummy *)(x))

static inline unsigned long __oro_cmpxchg(volatile void *ptr, unsigned long old,
				      unsigned long _new, int size)
{
	unsigned long prev;
	switch (size) {
	case 1:
		__asm__ __volatile__(ORO_LOCK_PREFIX "cmpxchgb %b1,%2"
				     : "=a"(prev)
				     : "q"(_new), "m"(*oro__xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 2:
		__asm__ __volatile__(ORO_LOCK_PREFIX "cmpxchgw %w1,%2"
				     : "=a"(prev)
				     : "q"(_new), "m"(*oro__xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 4:
		__asm__ __volatile__(ORO_LOCK_PREFIX "cmpxchgl %k1,%2"
				     : "=a"(prev)
				     : "q"(_new), "m"(*oro__xg(ptr)), "0"(old)
				     : "memory");
		return prev;
	case 8:
		__asm__ __volatile__(ORO_LOCK_PREFIX "cmpxchgq %1,%2"
				     : "=a"(prev)
				     : "q"(_new), "m"(*oro__xg(ptr)), "0"(old)
				     : "memory");
		return prev;

	}
	return old;
}

#define oro_cmpxchg(ptr,o,n)\
	((__typeof__(*(ptr)))__oro_cmpxchg((ptr),(unsigned long)(o),\
					(unsigned long)(n),sizeof(*(ptr))))



#else // OROBLD_OS_AGNOSTIC

// use system headers
#define new _new
#ifndef CONFIG_X86_CMPXCHG
#define CONFIG_X86_CMPXCHG
#endif
#include "asm/bitops.h"
#include "asm/system.h"
#undef new
#undef mb
#undef alternative
#undef alternative_input
#undef wmb
#undef rmb

#endif // OROBLD_OS_AGNOSTIC
