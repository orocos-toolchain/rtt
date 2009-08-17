#ifndef ORO_RTT_os_FWD_HPP
#define ORO_RTT_os_FWD_HPP

namespace RTT {
    namespace os {
        class AtomicInt;
        class MainThread;
        class Mutex;
        class MutexInterface;
        class MutexLock;
        class MutexRecursive;
        class MutexTimedLock;
        class MutexTryLock;
        class Semaphore;
        class StartStopManager;
        class ThreadInterface;
        class TimeService;
        struct CleanupFunction;
        struct InitFunction;
    }
    namespace detail {
        using namespace os;
    }
}
#endif
