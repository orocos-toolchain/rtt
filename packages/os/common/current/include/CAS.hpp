#ifndef ORO_OS_COMMON_CAS_HPP
#define ORO_OS_COMMON_CAS_HPP

#include "oro_system.h"

namespace ORO_OS
{
    /**
     * Compare And Swap.
     * Also known as Compare Exchange (cmpxch).
     * If \a addr contains \a expected, replace
     * it with \a value, and return \a true. Otherwise,
     * return \a false.
     */
    template< class T >
    bool CAS( volatile T* addr, const T& expected, const T& value) {
        return expected == cmpxchg(addr, expected, value);
    }

}

#endif
