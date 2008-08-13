/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:19 CET 2008  oro_system.h

                        oro_system.h -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

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


#include "oro_atomic.h"

#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

#define __HAVE_ARCH_CMPXCHG	1

static __inline__ unsigned long
__cmpxchg_u32(volatile unsigned int *p, unsigned long old, unsigned long newp)
{
	unsigned int prev;

	__asm__ __volatile__ (
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%2		# __cmpxchg_u32\n\
	cmpw	0,%0,%3\n\
	bne-	2f\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%4,0,%2\n\
	bne-	1b"
	ISYNC_ON_SMP
	"\n\
2:"
	: "=&r" (prev), "=m" (*p)
	: "r" (p), "r" (old), "r" (newp), "m" (*p)
	: "cc", "memory");

	return prev;
}

  // KG did explicit cast to remove following error
  /*
  /kgad/orocos-port-branch/ports/ecos/arch/powerpc/boa5200/orecos-library/install/include/os/oro_powerpc/oro_system.h:
  In function `long unsigned int __oro_cmpxchg(volatile void*, long
  unsigned int, long unsigned int, unsigned int)':
  /kgad/orocos-port-branch/ports/ecos/arch/powerpc/boa5200/orecos-library/install/include/os/oro_powerpc/oro_system.h:52:
  error: invalid conversion from `volatile void*' to `volatile
  unsigned int*'
  /kgad/orocos-port-branch/ports/ecos/arch/powerpc/boa5200/orecos-library/install/include/os/oro_powerpc/oro_system.h:52:
  error:   initializing argument 1 of `long unsigned int
  __cmpxchg_u32(volatile unsigned int*, long unsigned int, long
  unsigned int)' */

static __inline__ unsigned long
__oro_cmpxchg(volatile void *ptr, unsigned long old, unsigned long newp, unsigned int size)
{
	switch (size) {
	case 4:
		return __cmpxchg_u32((volatile unsigned int *) ptr, old, newp);
	}
	return old;
}

#define oro_cmpxchg(ptr,o,n)						 \
  ({									 \
     __typeof__(*(ptr)) _o_ = (o);					 \
     __typeof__(*(ptr)) _n_ = (n);					 \
     (__typeof__(*(ptr))) __oro_cmpxchg((ptr), (unsigned long)_o_,		 \
				    (unsigned long)_n_, sizeof(*(ptr))); \
  })

#ifdef _cplusplus
} // end extern "C"
#endif // _cplusplus
