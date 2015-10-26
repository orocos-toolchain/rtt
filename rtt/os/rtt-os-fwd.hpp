#ifndef ORO_RTT_os_FWD_HPP
#define ORO_RTT_os_FWD_HPP

namespace RTT {
    namespace os {
        class AtomicInt;
        class Condition;
        class MainThread;
        class Mutex;
        class MutexInterface;
        class MutexLock;
        class MutexRecursive;
        class MutexTimedLock;
        class MutexTryLock;
        class SharedMutex;
        class SharedMutexLock;
        class Semaphore;
        class StartStopManager;
        class Thread;
        class ThreadInterface;
        class TimeService;
        class Timer;
        struct CleanupFunction;
        struct InitFunction;
    }
    namespace detail {
        using namespace os;
    }
}
#endif
