#ifndef __ORO_ARCH_INTERFACE__
#define __ORO_ARCH_INTERFACE__

/** @file oro_arch_interface.h
 * @brief Please use this template to create new
 * oro_arch.h files for different target architectures or
 * compilers. You are free to make any of these functions
 * macros or to declare them inlined.
 *
 * Do not include this header, it serves only as an example
 * and the expected architecture-specific API of the RTT.
 * Include oro_arch.h instead.
 *
 * This interface has been inspired on the common intrinsics
 * API's of various compilers, operating systems and processor
 * architectures. Examples are: GCC, Visual Studio and the
 * Intel instruction set.
 */


/**
 * Structure that contains an int for atomic
 * operations.
 */
struct oro_atomic_t_interface;
typedef struct oro_atomic_t_interface oro_atomic_t;

/**
 * Initializes the uninitialized atomic structure a
 * with a counter value of 'n'.
 */
void ORO_ATOMIC_SETUP(oro_atomic_t* a, int n);

/**
 * Cleans up all resources allocated durint the
 * setup of atomic structure a.
 */
void ORO_ATOMIC_CLEANUP(oro_atomic_t* a);

/**
 * Returns the current counter value of the atomic
 * structure \a a.
 */
int oro_atomic_read(oro_atomic_t* a);

/**
 * Sets the current counter value of the atomic
 * structure \a a to \a n.
 */
void oro_atomic_set(oro_atomic_t* a, int n);

/**
 * Add n to a
 */
void oro_atomic_add(oro_atomic_t *a, int n);

/**
 * Add n to a and return the new value
 */
int oro_atomic_add_return(oro_atomic_t *a, int n);

/**
 * Subtract n from a
 */
void oro_atomic_sub(int n, oro_atomic_t *a, int n);

/**
 * Subtract n from a and return the new value
 */
int oro_atomic_sub_return(int n, oro_atomic_t *a, int n);

/**
 * Subtract n from a and test for zero
 */
int oro_atomic_sub_and_test(oro_atomic_t *a, int n);

/**
 * Increment a atomically
 */
void oro_atomic_inc(oro_atomic_t *a);

/**
 * Increment a atomically and return the new value
 */
int oro_atomic_inc_return(oro_atomic_t *a);

/**
 * Decrement a atomically
 */
void oro_atomic_dec(oro_atomic_t *a);

/**
 * Decrement a atomically and return the new value
 */
int oro_atomic_dec_return(oro_atomic_t *a);

/**
 * Decrement a atomically and test for zero.
 */
int oro_atomic_dec_and_test(oro_atomic_t *a);

/**
 * Increment a atomically and test for zero.
 */
int oro_atomic_inc_and_test(oro_atomic_t *a);

/**
 * Compare o with *ptr and swap with n if equal.
 * Note: you need to implement this function for
 * different sizes of pointers and payloads. You
 * will probably need to implement this function
 * as a macro to resolve this in
 * a way compatible with C.
 */
int oro_cmpxchg(void volatile* ptr, unsigned long o, unsigned long n);


#endif // __ORO_ARCH_INTERFACE__
