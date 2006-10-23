
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "BufferLockFree.hpp"

namespace RTT {
    // Force an instantiation, so that the compiler checks the syntax.
    template class BufferLockFree<double>;
}
