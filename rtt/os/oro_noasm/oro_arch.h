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

/**
 * Structure that contains an int for atomic
 * operations.
 */
typedef struct {
    rt_mutex_t m;
    int volatile cnter;
} oro_atomic_t;

#define ORO_ATOMIC_SETUP(a_int,n)	rtos_mutex_init(&((a_int)->m)); (a_int)->cnter = (n)
#define ORO_ATOMIC_CLEANUP(a_int)	rtos_mutex_destroy(&((a_int)->m))

#define oro_atomic_read(a_int)		((a_int)->cnter)

#define oro_atomic_set(a_int,n)		(((a_int)->cnter) = (n))

static __inline__ void oro_atomic_add(oro_atomic_t *a_int, int n )
{
  rtos_mutex_lock(&((a_int)->m)); (a_int)->cnter += n; rtos_mutex_unlock(&((a_int)->m));
}

static __inline__ int oro_atomic_add_return(oro_atomic_t *a_int, int n )
{
  int new_value;
  rtos_mutex_lock(&((a_int)->m)); new_value = ((a_int)->cnter += n); rtos_mutex_unlock(&((a_int)->m));
  return new_value;
}

static __inline__ void oro_atomic_sub(oro_atomic_t *a_int, int n )
{
  rtos_mutex_lock(&((a_int)->m)); (a_int)->cnter -= n; rtos_mutex_unlock(&((a_int)->m));
}

static __inline__ int oro_atomic_sub_return(oro_atomic_t *a_int, int n )
{
  int new_value;
  rtos_mutex_lock(&((a_int)->m)); new_value = ((a_int)->cnter -= n); rtos_mutex_unlock(&((a_int)->m));
  return new_value;
}

static __inline__ int oro_atomic_add_and_test(oro_atomic_t *a_int, int n )
{
  int ret = 0;
  rtos_mutex_lock(&((a_int)->m)); (a_int)->cnter -= n;  ret = ((a_int)->cnter == 0); rtos_mutex_unlock(&((a_int)->m));
  return ret;
}

static __inline__ int oro_atomic_sub_and_test(oro_atomic_t *a_int, int n )
{
  int ret = 0;
  rtos_mutex_lock(&((a_int)->m)); (a_int)->cnter -= n;  ret = ((a_int)->cnter == 0); rtos_mutex_unlock(&((a_int)->m));
  return ret;
}

static __inline__ void oro_atomic_inc(oro_atomic_t *a_int)
{
  rtos_mutex_lock(&((a_int)->m)); ++((a_int)->cnter) ; rtos_mutex_unlock(&((a_int)->m));
}

static __inline__ int oro_atomic_inc_return(oro_atomic_t *a_int)
{
  int new_value;
  rtos_mutex_lock(&((a_int)->m)); new_value = (++((a_int)->cnter)) ; rtos_mutex_unlock(&((a_int)->m));
  return new_value;
}

static __inline__ void oro_atomic_dec(oro_atomic_t *a_int)
{
  rtos_mutex_lock(&((a_int)->m)); --((a_int)->cnter) ; rtos_mutex_unlock(&((a_int)->m));
}

static __inline__ int oro_atomic_dec_return(oro_atomic_t *a_int)
{
  int new_value;
  rtos_mutex_lock(&((a_int)->m)); new_value = (--((a_int)->cnter)) ; rtos_mutex_unlock(&((a_int)->m));
  return new_value;
}

static __inline__ int oro_atomic_dec_and_test(oro_atomic_t *a_int)
{
  int ret = 0;
  rtos_mutex_lock(&((a_int)->m)); --((a_int)->cnter);  ret = ((a_int)->cnter == 0); rtos_mutex_unlock(&((a_int)->m));
  return ret;
}

static __inline__ int oro_atomic_inc_and_test(oro_atomic_t *a_int)
{
  int ret = 0;
  rtos_mutex_lock(&((a_int)->m)); ++((a_int)->cnter);  ret = ((a_int)->cnter == 0); rtos_mutex_unlock(&((a_int)->m));
  return ret;
}

#endif
