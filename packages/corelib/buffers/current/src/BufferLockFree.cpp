
#include "corelib/BufferLockFree.hpp"

namespace ORO_CoreLib {
    // Force an instantiation, so that the compiler checks the syntax.
    template BufferLockFree<double>;
    template<>
    const unsigned int BufferLockFree<double>::MAX_THREADS;
}
