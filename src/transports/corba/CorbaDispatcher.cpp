#include "CorbaDispatcher.hpp"

namespace RTT {
    using namespace corba;
    CorbaDispatcher::DispatchMap CorbaDispatcher::DispatchI;
    os::Mutex CorbaDispatcher::mlock;
}
