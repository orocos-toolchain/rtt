
#pragma implementation
#include "corelib/BufferLockFree.hpp"

namespace ORO_CoreLib {
    // Force an instantiation, so that the compiler checks the syntax.
    template class BufferLockFree<double>;
    template<>
    const unsigned int BufferLockFree<double>::MAX_THREADS;
}
