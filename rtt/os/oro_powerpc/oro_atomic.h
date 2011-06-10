/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:19 CET 2008  oro_atomic.h

                        oro_atomic.h -  description
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


#ifndef __ARCH_powerpc_ORO_ATOMIC__
#define __ARCH_powerpc_ORO_ATOMIC__

/* Klaas Gadeyne, copied the 32 bit part from
 * /usr/src/linux-2.6.16/include/asm-powerpc/atomic.h
 * /usr/src/linux-2.6.16/include/asm-powerpc/asm-compat.h
 * /usr/src/linux-2.6.16/include/asm-powerpc/synch.h
 * So will currently only work for 32 bit ppc
 */

// ==================================================
// asm/synch.h
#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

#ifdef __SUBARCH_HAS_LWSYNC
#    define LWSYNC	lwsync
#else
#    define LWSYNC	sync
#endif

#define ISYNC_ON_SMP
#define LWSYNC_ON_SMP

static inline void eieio(void)
{
	__asm__ __volatile__ ("eieio" : : : "memory");
}

static inline void isync(void)
{
	__asm__ __volatile__ ("isync" : : : "memory");
}

// ==================================================
// asm/asm-compat.h

#ifdef __ASSEMBLY__
#  define stringify_in_c(...)   __VA_ARGS__
#  define ASM_CONST(x)          x
#else
/* This version of stringify will deal with commas... */
#  define __stringify_in_c(...) #__VA_ARGS__
#  define stringify_in_c(...)   __stringify_in_c(__VA_ARGS__) " "
#  define __ASM_CONST(x)        x##UL
#  define ASM_CONST(x)          __ASM_CONST(x)
#endif

/* operations for longs and pointers */
#define PPC_LL          stringify_in_c(lwz)
#define PPC_STL         stringify_in_c(stw)
#define PPC_LCMPI       stringify_in_c(cmpwi)
#define PPC_LONG        stringify_in_c(.long)
#define PPC_TLNEI       stringify_in_c(twnei)
#define PPC_LLARX       stringify_in_c(lwarx)
#define PPC_STLCX       stringify_in_c(stwcx.)
#define PPC_CNTLZL      stringify_in_c(cntlzw)

#ifdef CONFIG_IBM405_ERR77
/* Erratum #77 on the 405 means we need a sync or dcbt before every
 * stwcx.  The old ATOMIC_SYNC_FIX covered some but not all of this.
 */
#define PPC405_ERR77(ra,rb)     stringify_in_c(dcbt     ra, rb;)
#define PPC405_ERR77_SYNC       stringify_in_c(sync;)
#else
#define PPC405_ERR77(ra,rb)
#define PPC405_ERR77_SYNC
#endif

// ==================================================
// asm/atomic.h

/*
 * PowerPC atomic operations
 */

typedef struct { volatile int counter; } oro_atomic_t;

#define ORO_ATOMIC_INIT(i)		{ (i) }
#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(v)	

#define oro_atomic_read(v)		((v)->counter)
#define oro_atomic_set(v,i)		(((v)->counter) = (i))

  static __inline__ void oro_atomic_add(oro_atomic_t *v, int n)
{
	int t;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%3		# oro_atomic_add\n\
	add	%0,%2,%0\n"
	PPC405_ERR77(0,%3)
"	stwcx.	%0,0,%3 \n\
	bne-	1b"
	: "=&r" (t), "=m" (v->counter)
	: "r" (a), "r" (&v->counter), "m" (v->counter)
	: "cc");
}

  static __inline__ int oro_atomic_add_return(oro_atomic_t *v, int n)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%2		# oro_atomic_add_return\n\
	add	%0,%1,%0\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%0,0,%2 \n\
	bne-	1b"
	ISYNC_ON_SMP
	: "=&r" (t)
	: "r" (a), "r" (&v->counter)
	: "cc", "memory");

	return t;
}

#define oro_atomic_add_negative(a, v)	(oro_atomic_add_return((a), (v)) < 0)

static __inline__ void oro_atomic_sub(int a, oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%3		# oro_atomic_sub\n\
	subf	%0,%2,%0\n"
	PPC405_ERR77(0,%3)
"	stwcx.	%0,0,%3 \n\
	bne-	1b"
	: "=&r" (t), "=m" (v->counter)
	: "r" (a), "r" (&v->counter), "m" (v->counter)
	: "cc");
}

static __inline__ int oro_atomic_sub_return(oro_atomic_t *v, int n)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%2		# oro_atomic_sub_return\n\
	subf	%0,%1,%0\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%0,0,%2 \n\
	bne-	1b"
	ISYNC_ON_SMP
	: "=&r" (t)
	: "r" (a), "r" (&v->counter)
	: "cc", "memory");

	return t;
}

static __inline__ void oro_atomic_inc(oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%2		# oro_atomic_inc\n\
	addic	%0,%0,1\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%0,0,%2 \n\
	bne-	1b"
	: "=&r" (t), "=m" (v->counter)
	: "r" (&v->counter), "m" (v->counter)
	: "cc");
}

static __inline__ int oro_atomic_inc_return(oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%1		# oro_atomic_inc_return\n\
	addic	%0,%0,1\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%0,0,%1 \n\
	bne-	1b"
	ISYNC_ON_SMP
	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

	return t;
}

/*
 * oro_atomic_inc_and_test - increment and test
 * @v: pointer of type oro_atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
#define oro_atomic_inc_and_test(v) (oro_atomic_inc_return(v) == 0)

static __inline__ void oro_atomic_dec(oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%2		# oro_atomic_dec\n\
	addic	%0,%0,-1\n"
	PPC405_ERR77(0,%2)\
"	stwcx.	%0,0,%2\n\
	bne-	1b"
	: "=&r" (t), "=m" (v->counter)
	: "r" (&v->counter), "m" (v->counter)
	: "cc");
}

static __inline__ int oro_atomic_dec_return(oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%1		# oro_atomic_dec_return\n\
	addic	%0,%0,-1\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%0,0,%1\n\
	bne-	1b"
	ISYNC_ON_SMP
	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

	return t;
}

#define oro_atomic_oro_cmpxchg(v, o, n) ((int)oro_cmpxchg(&((v)->counter), (o), (n)))
#define oro_atomic_xchg(v, new) (xchg(&((v)->counter), new))

/**
 * oro_atomic_add_unless - add unless the number is a given value
 * @v: pointer of type oro_atomic_t
 * @a: the amount to add to v...
 * @u: ...unless v is equal to u.
 *
 * Atomically adds @a to @v, so long as it was not @u.
 * Returns non-zero if @v was not @u, and zero otherwise.
 */
#define oro_atomic_add_unless(v, a, u)			\
({							\
	int c, old;					\
	c = oro_atomic_read(v);				\
	for (;;) {					\
		if (unlikely(c == (u)))			\
			break;				\
		old = oro_atomic_oro_cmpxchg((v), c, c + (a));	\
		if (likely(old == c))			\
			break;				\
		c = old;				\
	}						\
	c != (u);					\
})
#define oro_atomic_inc_not_zero(v) oro_atomic_add_unless((v), 1, 0)

#define oro_atomic_sub_and_test(a, v)	(oro_atomic_sub_return((a), (v)) == 0)
#define oro_atomic_dec_and_test(v)		(oro_atomic_dec_return((v)) == 0)

/*
 * Atomically test *v and decrement if it is greater than 0.
 * The function returns the old value of *v minus 1.
 */
static __inline__ int oro_atomic_dec_if_positive(oro_atomic_t *v)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%1		# oro_atomic_dec_if_positive\n\
	addic.	%0,%0,-1\n\
	blt-	2f\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%0,0,%1\n\
	bne-	1b"
	ISYNC_ON_SMP
	"\n\
2:"	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

	return t;
}

#define smp_mb__before_oro_atomic_dec()     smp_mb()
#define smp_mb__after_oro_atomic_dec()      smp_mb()
#define smp_mb__before_oro_atomic_inc()     smp_mb()
#define smp_mb__after_oro_atomic_inc()      smp_mb()

#undef ORO_LOCK

#ifdef _cplusplus
} // end extern "C"
#endif // _cplusplus

#endif // __ARCH_powerpc_ORO_ATOMIC__

