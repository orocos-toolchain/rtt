
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/ListLockFree.hpp"


void intrusive_ptr_add_ref( ORO_CoreLib::detail::IntrusiveStorage* p ) {
    atomic_inc( &p->ref );
}

void intrusive_ptr_release( ORO_CoreLib::detail::IntrusiveStorage* p ) {
    if (atomic_dec_and_test( &p->ref ) )
        delete p;
}

namespace ORO_CoreLib {
    // Force an instantiation, so that the compiler checks the syntax.
    template class ListLockFree<double>;
}
