/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  oro_atomic.h

                        oro_atomic.h -  description
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



#include "../../rtt-config.h"
#ifndef __ORO_ARCH_I386__
#define __ORO_ARCH_I386__

#ifndef CONFIG_FORCE_UP
#define ORO_LOCK "lock ; "
#else
#define ORO_LOCK ""
#endif

typedef struct { volatile int counter; } oro_atomic_t;

#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(v)

#define oro_atomic_read(v)		((v)->counter)

#define oro_atomic_set(v,i)		(((v)->counter) = (i))

static __inline__ void oro_atomic_add( oro_atomic_t *v, int i)
{
	__asm__ __volatile__(
		ORO_LOCK "addl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

static __inline__ int oro_atomic_add_return( oro_atomic_t *v, int i)
{
  /* Modern 486+ processor */
	int __i = i;
	__asm__ __volatile__(
		ORO_LOCK "xaddl %0, %1"
		: "+r" (i), "+m" (v->counter)
		: : "memory");
	return i + __i;
}

static __inline__ void oro_atomic_sub( oro_atomic_t *v, int i)
{
	__asm__ __volatile__(
		ORO_LOCK "subl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

static __inline__ int oro_atomic_sub_return( oro_atomic_t *v, int i)
{
	return oro_atomic_add_return( v, -i );
}

static __inline__ void oro_atomic_inc(oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

static __inline__ void oro_atomic_dec(oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

#define oro_atomic_inc_return(v) (oro_atomic_add_return(v, 1))
#define oro_atomic_dec_return(v) (oro_atomic_sub_return(v, 1))

static __inline__ int oro_atomic_dec_and_test(oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

static __inline__ int oro_atomic_inc_and_test(oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "incl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

#define smp_mb__before_oro_atomic_dec()	barrier()
#define smp_mb__after_oro_atomic_dec()	barrier()
#define smp_mb__before_oro_atomic_inc()	barrier()
#define smp_mb__after_oro_atomic_inc()	barrier()

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
        __asm__ __volatile__(ORO_LOCK_PREFIX "cmpxchgl %1,%2"
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

#undef ORO_LOCK
#undef ORO_LOCK_PREFIX
#endif
