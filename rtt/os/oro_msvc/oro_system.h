/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  oro_system.h

                        oro_system.h -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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
