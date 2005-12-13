
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/BufferLockFree.hpp"

namespace ORO_CoreLib {
    // Force an instantiation, so that the compiler checks the syntax.
    template class BufferLockFree<double>;
}
