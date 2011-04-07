#ifndef __ARCH_arm_ORO_SYSTEM__
#define __ARCH_arm_ORO_SYSTEM__

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

#include "oro_atomic.h"

#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

/*
 * cmpxchg only support 32-bits operands on ARMv6.
 */
static inline unsigned long __oro_cmpxchg(volatile void *ptr, unsigned long old,
				      unsigned long newv, int size)
{
	unsigned long oldval, res;

	switch (size) {
#ifdef USE_ARMV7_INSTRUCTIONS
	case 1:
		do {
			asm volatile("@ __cmpxchg1\n"
			"	ldrexb	%1, [%2]\n"
			"	mov	%0, #0\n"
			"	teq	%1, %3\n"
			"	strexbeq %0, %4, [%2]\n"
				: "=&r" (res), "=&r" (oldval)
				: "r" (ptr), "Ir" (old), "r" (newv)
				: "memory", "cc");
		} while (res);
		break;
	case 2:
		do {
			asm volatile("@ __cmpxchg1\n"
			"	ldrexh	%1, [%2]\n"
			"	mov	%0, #0\n"
			"	teq	%1, %3\n"
			"	strexheq %0, %4, [%2]\n"
				: "=&r" (res), "=&r" (oldval)
				: "r" (ptr), "Ir" (old), "r" (newv)
				: "memory", "cc");
		} while (res);
		break;
#endif /* USE_ARMV7_INSTRUCTIONS */
	case 4:
		do {
			asm volatile("@ __cmpxchg4\n"
			"	ldrex	%1, [%2]\n"
			"	mov	%0, #0\n"
			"	teq	%1, %3\n"
			"	strexeq %0, %4, [%2]\n"
				: "=&r" (res), "=&r" (oldval)
				: "r" (ptr), "Ir" (old), "r" (newv)
				: "memory", "cc");
		} while (res);
		break;
	default:
		oldval = 0;
	}

	return oldval;
}

static inline unsigned long __oro_cmpxchg_mb(volatile void *ptr, unsigned long old,
					 unsigned long newv, int size)
{
	unsigned long ret;

	oro_smp_mb();
	ret = __oro_cmpxchg(ptr, old, newv, size);
	oro_smp_mb();

	return ret;
}

#define oro_cmpxchg(ptr,o,n)                    \
	((__typeof__(*(ptr)))__oro_cmpxchg_mb((ptr),\
					  (unsigned long)(o),       \
					  (unsigned long)(n),       \
					  sizeof(*(ptr))))

#ifdef _cplusplus
} // end extern "C"
#endif // _cplusplus

#endif // __ARCH_arm_ORO_SYSTEM__

