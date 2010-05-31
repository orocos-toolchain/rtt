/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  oro_atomic.h

                        oro_atomic.h -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#ifndef __ARCH_NOASM_ORO_ATOMIC__
#define __ARCH_NOASM_ORO_ATOMIC__

/*
 * @file Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc. We emulate it using mutexes.
 */

#include "../../rtt-config.h"
#include "../fosi.h"

#ifndef __GNUC__
#define __inline__
#endif

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { rt_mutex_t m; volatile int counter; } oro_atomic_t;

#define ORO_ATOMIC_INIT(i)	{ (i) }
#define ORO_ATOMIC_SETUP(v,i)	rtos_mutex_init(&((v)->m)); (v)->counter = (i);
#define ORO_ATOMIC_CLEANUP(v)	rtos_mutex_destroy(&((v)->m));

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
  rtos_mutex_lock(&((v)->m)); (v)->counter += i; rtos_mutex_unlock(&((v)->m));
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
  rtos_mutex_lock(&((v)->m)); (v)->counter -= i; rtos_mutex_unlock(&((v)->m));
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
  int ret = 0;
  rtos_mutex_lock(&((v)->m)); (v)->counter -= i;  ret = ((v)->counter == 0); rtos_mutex_unlock(&((v)->m));
  return ret;
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
  rtos_mutex_lock(&((v)->m)); ++((v)->counter) ; rtos_mutex_unlock(&((v)->m));
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
  rtos_mutex_lock(&((v)->m)); --((v)->counter) ; rtos_mutex_unlock(&((v)->m));
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
  int ret = 0;
  rtos_mutex_lock(&((v)->m)); --((v)->counter);  ret = ((v)->counter == 0); rtos_mutex_unlock(&((v)->m));
  return ret;
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
  int ret = 0;
  rtos_mutex_lock(&((v)->m)); ++((v)->counter);  ret = ((v)->counter == 0); rtos_mutex_unlock(&((v)->m));
  return ret;
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
  int ret = 0;
  rtos_mutex_lock(&((v)->m)); (v)->counter += i;  ret = ((v)->counter < 0); rtos_mutex_unlock(&((v)->m));
  return ret;
}

#endif
