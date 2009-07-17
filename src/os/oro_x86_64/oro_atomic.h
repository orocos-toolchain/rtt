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
#ifndef __ARCH_x86_64_ORO_ATOMIC__
#define __ARCH_x86_64_ORO_ATOMIC__

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

#ifndef CONFIG_FORCE_UP
#define ORO_LOCK "lock ; "
#else
#define ORO_LOCK ""
#endif

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } oro_atomic_t;

#define ORO_ATOMIC_INIT(i)	{ (i) }
#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(v)	

/**
 * oro_atomic_read - read atomic variable
 * @v: pointer of type oro_atomic_t
 *
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
#define oro_atomic_read(v)		((v)->counter)

/**
 * oro_atomic_set - set atomic variable
 * @v: pointer of type oro_atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
#define oro_atomic_set(v,i)		(((v)->counter) = (i))

/**
 * oro_atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type oro_atomic_t
 *
 * Atomically adds @i to @v.  Note that the guaranteed useful range
 * of an oro_atomic_t is only 24 bits.
 */
static __inline__ void oro_atomic_add(int i, oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "addl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * oro_atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type oro_atomic_t
 *
 * Atomically subtracts @i from @v.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ void oro_atomic_sub(int i, oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "subl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * oro_atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type oro_atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ int oro_atomic_sub_and_test(int i, oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "subl %2,%0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * oro_atomic_inc - increment atomic variable
 * @v: pointer of type oro_atomic_t
 *
 * Atomically increments @v by 1.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ void oro_atomic_inc(oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * oro_atomic_dec - decrement atomic variable
 * @v: pointer of type oro_atomic_t
 *
 * Atomically decrements @v by 1.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ void oro_atomic_dec(oro_atomic_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * oro_atomic_dec_and_test - decrement and test
 * @v: pointer of type oro_atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ int oro_atomic_dec_and_test(oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * oro_atomic_inc_and_test - increment and test
 * @v: pointer of type oro_atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ int oro_atomic_inc_and_test(oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "incl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * oro_atomic_add_negative - add and test if negative
 * @v: pointer of type oro_atomic_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.  Note that the guaranteed
 * useful range of an oro_atomic_t is only 24 bits.
 */
static __inline__ int oro_atomic_add_negative(int i, oro_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "addl %2,%0; sets %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/* An 64bit atomic type */

typedef struct { volatile long counter; } atomic64_t;

#define ATOMIC64_INIT(i)	{ (i) }

/**
 * atomic64_read - read atomic64 variable
 * @v: pointer of type atomic64_t
 *
 * Atomically reads the value of @v.
 * Doesn't imply a read memory barrier.
 */
#define atomic64_read(v)		((v)->counter)

/**
 * atomic64_set - set atomic64 variable
 * @v: pointer to type atomic64_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
#define atomic64_set(v,i)		(((v)->counter) = (i))

/**
 * atomic64_add - add integer to atomic64 variable
 * @i: integer value to add
 * @v: pointer to type atomic64_t
 *
 * Atomically adds @i to @v.
 */
static __inline__ void atomic64_add(long i, atomic64_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "addq %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * atomic64_sub - subtract the atomic64 variable
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v.
 */
static __inline__ void atomic64_sub(long i, atomic64_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "subq %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * atomic64_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer to type atomic64_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static __inline__ int atomic64_sub_and_test(long i, atomic64_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "subq %2,%0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * atomic64_inc - increment atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1.
 */
static __inline__ void atomic64_inc(atomic64_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "incq %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic64_dec - decrement atomic64 variable
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1.
 */
static __inline__ void atomic64_dec(atomic64_t *v)
{
	__asm__ __volatile__(
		ORO_LOCK "decq %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic64_dec_and_test - decrement and test
 * @v: pointer to type atomic64_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static __inline__ int atomic64_dec_and_test(atomic64_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "decq %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic64_inc_and_test - increment and test
 * @v: pointer to type atomic64_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static __inline__ int atomic64_inc_and_test(atomic64_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "incq %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic64_add_negative - add and test if negative
 * @v: pointer to atomic64_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static __inline__ long atomic64_add_negative(long i, atomic64_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		ORO_LOCK "addq %2,%0; sets %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * oro_atomic_add_return - add and return
 * @v: pointer of type oro_atomic_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static __inline__ int oro_atomic_add_return(int i, oro_atomic_t *v)
{
	int __i = i;
	__asm__ __volatile__(
		ORO_LOCK "xaddl %0, %1;"
		:"=r"(i)
		:"m"(v->counter), "0"(i));
	return i + __i;
}

static __inline__ int oro_atomic_sub_return(int i, oro_atomic_t *v)
{
	return oro_atomic_add_return(-i,v);
}

#define oro_atomic_inc_return(v)  (oro_atomic_add_return(1,v))
#define oro_atomic_dec_return(v)  (oro_atomic_sub_return(1,v))

/* These are x86-specific, used by some header files */
#define oro_atomic_clear_mask(mask, addr) \
__asm__ __volatile__(ORO_LOCK "andl %0,%1" \
: : "r" (~(mask)),"m" (*addr) : "memory")

#define oro_atomic_set_mask(mask, addr) \
__asm__ __volatile__(ORO_LOCK "orl %0,%1" \
: : "r" (mask),"m" (*(addr)) : "memory")

/* Atomic operations are already serializing on x86 */
#define smp_mb__before_oro_atomic_dec()	barrier()
#define smp_mb__after_oro_atomic_dec()	barrier()
#define smp_mb__before_oro_atomic_inc()	barrier()
#define smp_mb__after_oro_atomic_inc()	barrier()

#undef ORO_LOCK
#endif
