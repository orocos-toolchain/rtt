#ifndef ORO_OS_THREADS_HPP
#define ORO_OS_THREADS_HPP

#include "Atomic.hpp"

namespace ORO_OS
{
    /**
     * The number of threads in addition to the main() thread.
     * The main thread is thread zero (0). \a threads is
     * increased by one for each PeriodicThread or SingleThread
     * object created and is never decreased.
     */
    extern AtomicInt threads;

}
#endif
