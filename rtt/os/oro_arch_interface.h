/***************************************************************************
  tag: Philippe Hamelin Wed Apr 13 13:02:01 2011 -0400 oro_arch_interface.h

                        oro_arch_interface.h -  description
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
 * Substract n from a
 */
void oro_atomic_sub(int n, oro_atomic_t *a, int n);

/**
 * Substract n from a and test for zero
 */
int oro_atomic_sub_and_test(oro_atomic_t *a, int n);

/**
 * Increment a atomically
 */
void oro_atomic_inc(oro_atomic_t *a);

/**
 * Decrement a atomically
 */
void oro_atomic_dec(oro_atomic_t *a);

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
