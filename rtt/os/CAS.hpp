/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  CAS.hpp 

                        CAS.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 
#ifndef OS_COMMON_CAS_HPP
#define OS_COMMON_CAS_HPP

#include "oro_arch.h"

namespace RTT
{ namespace os {
    /**
     * Compare And Swap.
     * Also known as Compare Exchange (cmpxch).
     * If \a addr contains \a expected, replace
     * it with \a value, and return \a true. Otherwise,
     * return \a false.
     */
    template< class T, class V, class W >
    inline bool CAS( volatile T* addr, const V& expected, const W& value) {
        return expected == oro_cmpxchg(addr, expected, value);
    }

    /**
     * Overload of Compare And Swap for oro_atomic_t.
     */
    inline bool CAS( volatile oro_atomic_t* addr, const int expected, const int value) {
        // assume that oro_atomic_read(addr) returns a reference
        volatile int &ref = oro_atomic_read(addr);
        return expected == oro_cmpxchg(&ref, expected, value);
    }

}}

#endif
