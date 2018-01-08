#ifndef __GCC_ORO_ARCH__
#define __GCC_ORO_ARCH__

#include "../../rtt-config.h"

/**
 * Structure that contains an int for atomic
 * operations.
 */
typedef struct {
    int volatile cnt;
} oro_atomic_t;

#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(a_int)

#define oro_atomic_read(a_int)		((a_int)->cnt)

#define oro_atomic_set(a_int,n)		(((a_int)->cnt) = (n))

/**
 * Add n to a_int
 */
static __inline__ void oro_atomic_add(oro_atomic_t *a_int, int n)
{
    (void)__sync_add_and_fetch(&a_int->cnt, n);
}

/**
 * Add n to a_int and return the new value
 */
static __inline__ int oro_atomic_add_return(oro_atomic_t *a_int, int n)
{
    return __sync_add_and_fetch(&a_int->cnt, n);
}

/**
 * Subtract n from a_int
 */
static __inline__ void oro_atomic_sub(oro_atomic_t *a_int, int n)
{
    (void)__sync_sub_and_fetch(&a_int->cnt, n);
}

/**
 * Subtract n from a_int and return the new value
 */
static __inline__ int oro_atomic_sub_return(oro_atomic_t *a_int, int n)
{
    return __sync_sub_and_fetch(&a_int->cnt, n);
}

/**
 * Subtract n from a_int and test for zero
 */
static __inline__ int oro_atomic_sub_and_test(oro_atomic_t *a_int, int n)
{
    return !(__sync_sub_and_fetch(&a_int->cnt, n));
}

/**
 * Increment a_int atomically
 */
static __inline__ void oro_atomic_inc(oro_atomic_t *a_int)
{
    (void)__sync_fetch_and_add(&a_int->cnt, 1);
}

/**
 * Increment a_int atomically and return the new value
 */
static __inline__ int oro_atomic_inc_return(oro_atomic_t *a_int)
{
    return __sync_fetch_and_add(&a_int->cnt, 1);
}

/**
 * Decrement a_int atomically
 */
static __inline__ void oro_atomic_dec(oro_atomic_t *a_int)
{
    (void)__sync_fetch_and_sub(&a_int->cnt, 1);
}

/**
 * Decrement a_int atomically and return the new value
 */
static __inline__ int oro_atomic_dec_return(oro_atomic_t *a_int)
{
    return __sync_fetch_and_sub(&a_int->cnt, 1);
}

/**
 * Decrement a_int atomically and test for zero.
 */
static __inline__ int oro_atomic_dec_and_test(oro_atomic_t *a_int)
{
    return !(__sync_sub_and_fetch(&a_int->cnt, 1));
}

/**
 * Increment a_int atomically and test for zero.
 */
static __inline__ int oro_atomic_inc_and_test(oro_atomic_t *a_int)
{
    return !(__sync_add_and_fetch(&a_int->cnt, 1));
}

/**
 * Compare o with *ptr and swap with n if equal.
 */
#define oro_cmpxchg(ptr,o,n)\
    ((__typeof__(*(ptr)))__sync_val_compare_and_swap((ptr),(o),(n)))


#endif // __GCC_ORO_ARCH__
