#include "../../rtt-config.h"

#include <intrin.h>

#ifndef __ARCH_MSVC_ORO_SYSTEM__
#define __ARCH_MSVC_ORO_SYSTEM__

//static inline unsigned long __oro_cmpxchg(volatile void *ptr, unsigned long old,
//				      unsigned long _new, int size)
//{
//	unsigned long prev;
//	switch (size) {
//	case 2:
//		return _InterlockedCompareExchange16((short *)ptr, _new, old);
//	case 4:
//		return _InterlockedCompareExchange((long *)ptr, _new, old);
//	case 8:
//		return _InterlockedCompareExchange64((__int64 *)ptr, _new, old);
//	}
//	return old;
//}

#pragma warning(push)
#pragma warning(disable : 4715)	// Disable warning on "specified function can potentially not return a value"
#pragma intrinsic(_InterlockedCompareExchange16)

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

/*
#define oro_cmpxchg(ptr,o,n)\
	((__typeof__(*(ptr)))__oro_cmpxchg((ptr),(unsigned long)(o),\
					(unsigned long)(n),sizeof(*(ptr))))
*/

#endif
