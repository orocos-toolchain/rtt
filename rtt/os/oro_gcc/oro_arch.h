/***************************************************************************
  tag: Philippe Hamelin Wed Apr 13 13:02:01 2011 -0400 oro_arch.h

                        oro_arch.h -  description
                           -------------------
    begin                : Wed Apr 13 2011
    copyright            : (C) 2005 Peter Soetens
    copyright            : (C) 2011 Philippe Hamelin
    email                : philippe.hamelin@gmail.com

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


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
    (__sync_val_compare_and_swap((ptr),(o),(n)))


#endif // __GCC_ORO_ARCH__
