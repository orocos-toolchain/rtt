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
