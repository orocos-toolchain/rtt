#ifndef __ARCH_arm_ORO_ATOMIC__
#define __ARCH_arm_ORO_ATOMIC__

/*
 * Philippe Hamelin (philippe.hamelin@gmail.com)
 * Expertise robotique et civil
 * Institut de recherche d'Hydro-Quebec
 * Quebec, Canada
 *
 * These atomic operations have been mostly copied from the Linux kernel in
 * arch/arm/include/asm/atomic.h and arch/arm/include/asm/system.h. There's
 * a lot of variants depending on the instruction set version. This ARM port
 * only support ARMv6 and ARMv7 cpu, which includes Cortex-A8 and Cortex-A9.
 */

#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

// Detect instruction set version
#if defined(__ARM_ARCH_6__) ||  \
    defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7__)
# define USE_ARMV6_INSTRUCTIONS 1
#endif

#if defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7__)
# define USE_ARMV7_INSTRUCTIONS 1
#endif

// The barrier for SMP is different on ARMv6 and ARMv7. 
#if defined(USE_ARMV7_INSTRUCTIONS)
# define oro_dmb() __asm__ __volatile__ ("dmb" : : : "memory")
#elif defined(USE_ARMV6_INSTRUCTIONS)
# define oro_dmb() __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 5" \
                                        : : "r" (0) : "memory")
#else
# error This ARM architecture is not supported.
#endif

#define oro_barrier() __asm__ __volatile__("": : :"memory")

// CONFIG_FORCE_UP optimize for single cpu
#ifndef CONFIG_FORCE_UP
#define oro_smp_mb()        oro_dmb()
#else
#define oro_smp_mb()        oro_barrier()
#endif

typedef struct { volatile int counter; } oro_atomic_t;

#define ORO_ATOMIC_INIT(i)	{ (i) }
#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(v)	

/*
 * On ARM, ordinary assignment (str instruction) doesn't clear the local
 * strex/ldrex monitor on some implementations. The reason we can use it for
 * atomic_set() is the clrex or dummy strex done on every exception return.
 */
#define oro_atomic_read(v)	(*(volatile int *)&(v)->counter)
#define oro_atomic_set(v,i)	(((v)->counter) = (i))

/*
 * ARMv6 UP and SMP safe atomic ops.  We use load exclusive and
 * store exclusive to ensure that these are atomic.  We may loop
 * to ensure that the update happens.
 */
static inline void oro_atomic_add(int i, oro_atomic_t *v)
{
	unsigned long tmp;
	int result;

	__asm__ __volatile__("@ atomic_add\n"
"1:	ldrex	%0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");
}

static inline int oro_atomic_add_return(int i, oro_atomic_t *v)
{
	unsigned long tmp;
	int result;

	oro_smp_mb();

	__asm__ __volatile__("@ atomic_add_return\n"
"1:	ldrex	%0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	oro_smp_mb();

	return result;
}

static inline void oro_atomic_sub(int i, oro_atomic_t *v)
{
	unsigned long tmp;
	int result;

	__asm__ __volatile__("@ atomic_sub\n"
"1:	ldrex	%0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");
}

static inline int oro_atomic_sub_return(int i, oro_atomic_t *v)
{
	unsigned long tmp;
	int result;

	oro_smp_mb();

	__asm__ __volatile__("@ atomic_sub_return\n"
"1:	ldrex	%0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	oro_smp_mb();

	return result;
}

static inline int oro_atomic_cmpxchg(oro_atomic_t *ptr, int old, int newv)
{
	unsigned long oldval, res;

	oro_smp_mb();

	do {
		__asm__ __volatile__("@ atomic_cmpxchg\n"
		"ldrex	%1, [%3]\n"
		"mov	%0, #0\n"
		"teq	%1, %4\n"
		"strexeq %0, %5, [%3]\n"
		    : "=&r" (res), "=&r" (oldval), "+Qo" (ptr->counter)
		    : "r" (&ptr->counter), "Ir" (old), "r" (newv)
		    : "cc");
	} while (res);

	oro_smp_mb();

	return oldval;
}

static inline void oro_atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long tmp, tmp2;

	__asm__ __volatile__("@ atomic_clear_mask\n"
"1:	ldrex	%0, [%3]\n"
"	bic	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (tmp), "=&r" (tmp2), "+Qo" (*addr)
	: "r" (addr), "Ir" (mask)
	: "cc");
}

#define oro_atomic_xchg(v, newv)         (xchg(&((v)->counter), newv))

#define oro_atomic_inc(v)		        oro_atomic_add(1, v)
#define oro_atomic_dec(v)		        oro_atomic_sub(1, v)

#define oro_atomic_inc_and_test(v)	    (oro_atomic_add_return(1, v) == 0)
#define oro_atomic_dec_and_test(v)	    (oro_atomic_sub_return(1, v) == 0)
#define oro_atomic_inc_return(v)        (oro_atomic_add_return(1, v))
#define oro_atomic_dec_return(v)        (oro_atomic_sub_return(1, v))
#define oro_atomic_sub_and_test(i, v)   (oro_atomic_sub_return(i, v) == 0)

#define oro_atomic_add_negative(i,v)    (oro_atomic_add_return(i, v) < 0)

#define smp_mb__before_oro_atomic_dec() oro_smp_mb()
#define smp_mb__after_oro_atomic_dec()	oro_smp_mb()
#define smp_mb__before_oro_atomic_inc()	oro_smp_mb()
#define smp_mb__after_oro_atomic_inc()	oro_smp_mb()

#ifdef _cplusplus
} // end extern "C"
#endif // _cplusplus

#endif // __ARCH_arm_ORO_atomic__

