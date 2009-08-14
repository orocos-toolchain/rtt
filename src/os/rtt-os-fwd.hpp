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
        class basic_iostreams;
        class basic_istreams;
        class basic_ostreams;
        class basic_streams;
        class printbufs;
        class printstream;
        class streambufs;
        class stringbufs;
        class stringstreams;
        struct CleanupFunction;
        struct InitFunction;
        struct print_helper;
        struct rt_list;
        struct string_helper;
    }
}
#endif
