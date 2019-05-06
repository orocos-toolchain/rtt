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


#include "../../rtt-config.h"
#ifndef __ARCH_MSVC_ORO_ATOMIC__
#define __ARCH_MSVC_ORO_ATOMIC__

#include <windows.h>
#undef interface
#include <intrin.h>

typedef volatile long oro_atomic_t;

#define ORO_ATOMIC_SETUP	oro_atomic_set
#define ORO_ATOMIC_CLEANUP(a_int)

#define oro_atomic_read(a_int)		(*(a_int))

#define oro_atomic_set(a_int,n)		(*(a_int) = (n))

static __forceinline void oro_atomic_add(oro_atomic_t *a_int, int n)
{
	_InterlockedExchangeAdd((long *)a_int, n);
}

static __forceinline void oro_atomic_sub(oro_atomic_t *a_int, int n)
{
	oro_atomic_add(a_int, -n);
}

static __forceinline int oro_atomic_sub_and_test(oro_atomic_t *a_int, int n)
{
	return ((_InterlockedExchangeAdd((long *)a_int, -n) - n) == 0);
}

static __forceinline void oro_atomic_inc(oro_atomic_t *a_int)
{
	_InterlockedIncrement((long *)a_int);
}

static __forceinline void oro_atomic_dec(oro_atomic_t *a_int)
{
	_InterlockedDecrement((long *)a_int);
}

static __forceinline int oro_atomic_dec_and_test(oro_atomic_t *a_int)
{
	return (_InterlockedDecrement((long *)a_int) == 0);
}

static __forceinline int oro_atomic_inc_and_test(oro_atomic_t *a_int)
{
	return (_InterlockedIncrement((long *)a_int) == 0);
}

static __forceinline int oro_atomic_add_negative(oro_atomic_t *a_int, int n)
{
	return ((_InterlockedExchangeAdd((long *)a_int, n) + n) < 0);
}

static __forceinline int oro_atomic_add_return(oro_atomic_t *a_int, int n)
{
	return _InterlockedExchangeAdd((long *)a_int, n) + n;
}

static __forceinline int oro_atomic_sub_return(oro_atomic_t *a_int, int n)
{
	return oro_atomic_add_return(a_int, -n);
}

static __forceinline int oro_atomic_inc_return(oro_atomic_t *a_int)
{
	return _InterlockedIncrement((long *)a_int);
}

static __forceinline int oro_atomic_dec_return(oro_atomic_t *a_int)
{
	return _InterlockedDecrement((long *)a_int);
}

static __forceinline int oro_atomic_clear_mask(oro_atomic_t *a_int, int mask)
{
	return _InterlockedAnd((long *)a_int, ~mask);
}

static __forceinline int oro_atomic_set_mask(oro_atomic_t *a_int, int mask)
{
	return _InterlockedOr((long *)a_int, mask);
}

#pragma warning(push)
#pragma warning(disable : 4715) // Disable warning on "specified function can potentially not return a value"

template<typename T> inline T oro_cmpxchg(volatile void * ptr, T old, T _new)
{
    switch(sizeof(T))
    {
    case 2:
        return (T)(_InterlockedCompareExchange16((short *)ptr, (short)_new, (short)old));
    case 4:
        return (T)(_InterlockedCompareExchange((long *)ptr, (long)_new, (long)old));
    case 8:
        return (T)(_InterlockedCompareExchange64((__int64 *)ptr, (__int64)_new, (__int64)old));
    }
}

#pragma warning(pop)

#endif
