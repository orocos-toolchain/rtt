/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <rtt-config.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <bitset>

#include <boost/lexical_cast.hpp>

#include <vector>
#include <boost/array.hpp>

#include <boost/test/unit_test.hpp>

#define SAMPLE_SIZE 10000
#define SAMPLE_TYPE boost::array<double,SAMPLE_SIZE>
// #define SAMPLE_TYPE std::vector<double>
#define NUMBER_OF_CYCLES 10000

enum PortTypes { DataPortType = 1, BufferPortType = 2 };
template <typename T, PortTypes> struct Adaptor;


#if (RTT_VERSION_MAJOR == 1)

    #define RTT_VERSION_GTE(major,minor,patch) \
        ((RTT_VERSION_MAJOR > major) || (RTT_VERSION_MAJOR == major && \
         (RTT_VERSION_MINOR > minor) || (RTT_VERSION_MINOR == minor && \
         (RTT_VERSION_PATCH >= patch))))

    #include "TaskContext.hpp"
    #include "DataPort.hpp"
    #include "BufferPort.hpp"
    #include "ConnPolicy.hpp"
    #include "Activity.hpp"
    #include "SlaveActivity.hpp"

    using RTT::TaskContext;
    using RTT::OS::HighestPriority;
    typedef enum { NoData, OldData, NewData } FlowStatus;
    typedef enum { WriteSuccess, WriteFailure, NotConnected } WriteStatus;
    typedef enum { UnspecifiedReadPolicy, ReadUnordered, ReadShared } ReadPolicy;
    typedef enum { UnspecifiedWritePolicy, WritePrivate, WriteShared } WritePolicy;

    // dummy RTT v2.8.99 compatible ConnPolicy
    struct ConnPolicy {
        static const int UNBUFFERED = -1;
        static const int DATA   = 0;
        static const int BUFFER = 1;
        static const int CIRCULAR_BUFFER = 2;

        static const int UNSYNC    = 0;
        static const int LOCKED    = 1;
        static const int LOCK_FREE = 2;

        static const bool PUSH = false;
        static const bool PULL = true;

        ConnPolicy() : type(), size(), lock_policy(), init(), pull(), read_policy(), write_policy(), max_threads(), mandatory(), transport(), data_size(), name_id() {}

        int    type;
        int    size;
        int    lock_policy;
        bool   init;
        bool   pull;
        ReadPolicy read_policy;
        WritePolicy write_policy;
        int max_threads;
        bool mandatory;
        int    transport;
        mutable int    data_size;
        mutable std::string name_id;
    };

    // alternative Mutex/MutexLock/Condition variable implementation for worker synchronization only
    #include <boost/thread/mutex.hpp>
    #include <boost/thread/condition_variable.hpp>
    class Mutex
    {
    public:
        void lock() { m.lock(); }
        void unlock() { m.unlock(); }
        boost::mutex m;
    };

    class MutexLock
    {
    public:
        MutexLock(Mutex &mutex) : m(mutex.m) { m.lock(); }
        ~MutexLock() { m.unlock(); }
        boost::mutex &m;
    };

    class Condition
    {
    public:
        void broadcast() { c.notify_all(); }
        void wait(Mutex &m) { c.wait(m.m); }

    private:
        boost::condition_variable_any c;
    };

    using RTT::SlaveActivity;
    using RTT::OS::ThreadInterface;

    #include <pthread.h>
    namespace adaptor
    {
        static bool trigger(TaskContext *tc) {
            return tc->getActivity()->trigger();
        }

        static void yield(TaskContext *tc) {
            tc->getActivity()->thread()->yield();
        }

        static bool update(TaskContext *tc) {
            return tc->getActivity()->execute();
        }

        static bool setCpuAffinity(ThreadInterface *thread, std::bitset<16> cpu_affinity) {
            if ( cpu_affinity.none() )
                cpu_affinity = ~cpu_affinity;
            RTOS_TASK *task = thread->getTask();
            if( task && task->thread != 0 ) {
                cpu_set_t cs;
                CPU_ZERO(&cs);
                for(std::size_t i = 0; i < cpu_affinity.size(); i++)
                    {
                        if(cpu_affinity[i]) { CPU_SET(i, &cs); }
                    }
                return 0 == pthread_setaffinity_np(task->thread, sizeof(cs), &cs);
            }
            return false;
        }

        static std::bitset<16> getCpuAffinity(ThreadInterface *thread) {
            RTOS_TASK *task = thread->getTask();
            if( task && task->thread != 0) {
                std::bitset<16> cpu_affinity;
                cpu_set_t cs;
                pthread_getaffinity_np(task->thread, sizeof(cs), &cs);
                for(std::size_t i = 0; i < cpu_affinity.size(); i++)
                {
                    if(CPU_ISSET(i, &cs)) { cpu_affinity[i] = true; }
                }
                return cpu_affinity;
            }
            return ~std::bitset<16>();
        }

        static RTT::ConnectionInterface::shared_ptr getOrCreateConnection(RTT::PortInterface &output_port, RTT::PortInterface &input_port, const ConnPolicy &cp) {
            RTT::ConnectionTypes::ConnectionType con_type;
            switch(cp.lock_policy) {
                case ConnPolicy::LOCKED:    con_type = RTT::ConnectionTypes::locked; break;
                case ConnPolicy::LOCK_FREE: con_type = RTT::ConnectionTypes::lockfree; break;
                default:                    return RTT::ConnectionInterface::shared_ptr();
            }
            RTT::ConnectionInterface::shared_ptr connection = output_port.connection();
            if (!connection) connection = input_port.connection();
            if (!connection) {
                connection = output_port.createConnection(con_type);
                connection->connect();
            }
            return connection;
        }
    }

    template <typename T, PortTypes>
    struct Adaptor {};

    template <typename T>
    struct Adaptor<T,DataPortType>
    {
        typedef RTT::DataPortBase<T> Port;
        typedef RTT::WriteDataPort<T> OutputPort;
        typedef RTT::ReadDataPort<T> InputPort;

        static void addPort(TaskContext *tc, Port &port) {
            tc->ports()->addPort(&port);
        }

        static void addEventPort(TaskContext *tc, InputPort &port) {
            tc->ports()->addEventPort(&port);
        }

        static void setDataSample(OutputPort &port, const T &sample) {
        }

        static WriteStatus write(OutputPort &port, const T &sample) {
            if (!port.connected()) return NotConnected;
            port.Set(sample);
            return WriteSuccess;
        }

        static FlowStatus read(InputPort &port, T &sample, bool copy_old_data) {
            if (!port.connected()) return NoData;
            port.Get(sample);
            return NewData;
        }

        static bool connect(OutputPort &output_port, InputPort &input_port, const ConnPolicy &cp) {
            if (cp.type != ConnPolicy::DATA) return false;
            RTT::ConnectionInterface::shared_ptr connection = adaptor::getOrCreateConnection(output_port, input_port, cp);
            if (!connection) return false;
            if (!output_port.connected() && !output_port.connectTo(connection)) return false;
            if (!input_port.connected() && !input_port.connectTo(connection)) return false;
            return true;
        }
    };

    template <typename T>
    struct Adaptor<T,BufferPortType>
    {
        typedef RTT::BufferPortBase<T> Port;
        typedef RTT::ReadBufferPort<T> InputPort;

        // derive from RTT::WriteBufferPort<T> to adapt constructor signature
        class OutputPort : public RTT::WriteBufferPort<T>
        {
        public:
            OutputPort(const std::string &name, const T &initial_value = T()) : RTT::WriteBufferPort<T>(name, 0, initial_value) {}
        };

        static void addPort(TaskContext *tc, Port &port) {
            tc->ports()->addPort(&port);
        }

        static void addEventPort(TaskContext *tc, InputPort &port) {
            tc->ports()->addEventPort(&port);
        }

        static void setDataSample(OutputPort &port, const T &sample) {
        }

        static WriteStatus write(OutputPort &port, const T &sample) {
            if (!port.connected()) return NotConnected;
            return port.Push(sample) ? WriteSuccess : WriteFailure;
        }

        static FlowStatus read(InputPort &port, T &sample, bool copy_old_data) {
            if (!port.connected()) return NoData;
            return port.Pop(sample) ? NewData : NoData;
        }

        static bool connect(OutputPort &output_port, InputPort &input_port, const ConnPolicy &cp) {
            if (cp.type != ConnPolicy::BUFFER) return false;
            output_port.setBufferSize(cp.size);
            RTT::ConnectionInterface::shared_ptr connection = adaptor::getOrCreateConnection(output_port, input_port, cp);
            if (!connection) return false;
            if (!output_port.connected() && !output_port.connectTo(connection)) return false;
            if (!input_port.connected() && !input_port.connectTo(connection)) return false;
            return true;
        }
    };

    class Activity : public RTT::Activity
    {
    public:
        Activity() {}
        Activity(int scheduler, int priority, RTT::Seconds period, unsigned cpu_affinity, RTT::RunnableInterface *r, const std::string &name)
            : RTT::Activity(scheduler, priority, period, r, name)
        {
            adaptor::setCpuAffinity(this->thread(), cpu_affinity);
            usleep(1000); // getScheduler() and getPriority() may return wrong values if called directly after construction!
        }
    };

#else
    #include <rtt/TaskContext.hpp>
    #include <rtt/InputPort.hpp>
    #include <rtt/OutputPort.hpp>
    #include <rtt/extras/SlaveActivity.hpp>
    #include <rtt/Activity.hpp>

    using RTT::TaskContext;
    using RTT::FlowStatus;
    using RTT::NoData;
    using RTT::OldData;
    using RTT::NewData;
    using RTT::Activity;
    using RTT::os::HighestPriority;

#if !RTT_VERSION_GTE(2,8,99)
    typedef enum { WriteSuccess, WriteFailure, NotConnected } WriteStatus;
    typedef enum { UnspecifiedBufferPolicy, PerConnection, PerInputPort, PerOutputPort, Shared } BufferPolicy;

    // dummy RTT v2.8.99 compatible ConnPolicy
    struct ConnPolicy : public RTT::ConnPolicy {
        static const bool PUSH = false;
        static const bool PULL = true;

        int buffer_policy;
        int max_threads;
        bool mandatory;

        ConnPolicy() :  RTT::ConnPolicy(), buffer_policy(), max_threads(), mandatory() {}
        explicit ConnPolicy(int type) : RTT::ConnPolicy(type), buffer_policy(), max_threads(), mandatory() {}
        explicit ConnPolicy(int type, int lock_policy) : RTT::ConnPolicy(type, lock_policy), buffer_policy(), max_threads(), mandatory() {}

        ConnPolicy &operator==(const RTT::ConnPolicy &other) {
            static_cast<RTT::ConnPolicy&>(*this) = other;
            buffer_policy = 0;
            max_threads = 0;
            mandatory = false;
            return *this;
        }
    };

#else
    using namespace RTT;
#endif

    using RTT::os::Mutex;
    using RTT::os::MutexLock;
    using RTT::os::Condition;
    using RTT::os::ThreadInterface;

    using RTT::extras::SlaveActivity;

    namespace adaptor
    {
        static bool trigger(TaskContext *tc) {
            return tc->trigger();
        }

        static void yield(TaskContext *tc) {
            tc->getActivity()->thread()->yield();
        }

        static bool update(TaskContext *tc) {
            return tc->update();
        }

        RTT_UNUSED static bool setCpuAffinity(ThreadInterface *thread, const std::bitset<16> &cpu_affinity) {
            RTT::os::Thread *t = dynamic_cast<RTT::os::Thread *>(thread);
            if (!t) return false;
            return t->setCpuAffinity(cpu_affinity.to_ulong());
        }

        static std::bitset<16> getCpuAffinity(ThreadInterface *thread) {
            RTT::os::Thread *t = dynamic_cast<RTT::os::Thread *>(thread);
            if (!t) return ~std::bitset<16>();
            return std::bitset<16>(t->getCpuAffinity());
        }
    }

    template <typename T, PortTypes>
    struct Adaptor
    {
        typedef RTT::base::PortInterface Port;
        typedef RTT::OutputPort<T> OutputPort;
        typedef RTT::InputPort<T> InputPort;

        static void addPort(TaskContext *tc, Port &port) {
            tc->addPort(port);
        }

        static void addEventPort(TaskContext *tc, InputPort &port) {
            tc->addEventPort(port);
        }

        static void setDataSample(OutputPort &port, const T &sample) {
            port.setDataSample(sample);
        }

#if RTT_VERSION_GTE(2,8,99)
        static WriteStatus write(OutputPort &port, const T &sample) {
            return port.write(sample);
        }
#else
        static WriteStatus write(OutputPort &port, const T &sample) {
            port.write(sample);
            return WriteSuccess;
        }
#endif

        static FlowStatus read(InputPort &port, T &sample, bool copy_old_data = true) {
            return port.read(sample, copy_old_data);
        }

        static bool connect(OutputPort &output_port, InputPort &input_port, const ConnPolicy &cp) {
            return output_port.connectTo(&input_port, cp);
        }
    };
#endif

#if !RTT_VERSION_GTE(2,8,99)
    std::ostream &operator<<(std::ostream &os, const ConnPolicy &cp)
    {
        std::string type;
        switch(cp.type) {
            case ConnPolicy::UNBUFFERED:      type = "UNBUFFERED"; break;
            case ConnPolicy::DATA:            type = "DATA"; break;
            case ConnPolicy::BUFFER:          type = "BUFFER"; break;
            case ConnPolicy::CIRCULAR_BUFFER: type = "CIRCULAR_BUFFER"; break;
            default:                          type = "(unknown type)"; break;
        }
        if (cp.size > 0) {
            type += "[" + boost::lexical_cast<std::string>(cp.size) + "]";
        }

        std::string lock_policy;
        switch(cp.lock_policy) {
            case ConnPolicy::UNSYNC:    lock_policy = "UNSYNC"; break;
            case ConnPolicy::LOCKED:    lock_policy = "LOCKED"; break;
            case ConnPolicy::LOCK_FREE: lock_policy = "LOCK_FREE"; break;
            default:                    lock_policy = "(unknown lock policy)"; break;
        }

        std::string pull;
        switch(int(cp.pull)) {
            case int(ConnPolicy::PUSH): pull = "PUSH"; break;
            case int(ConnPolicy::PULL): pull = "PULL"; break;
        }

        os << pull << " ";
        os << lock_policy << " ";
        os << type;
        if (!cp.name_id.empty()) os << " (name_id=" << cp.name_id << ")";

        return os;
    }
#endif

#ifndef _stringify
  #define _stringify(x) _stringify2(x)
  #ifndef _stringify2
    #define _stringify2(x...) #x
  #endif
#endif

struct CopyAndAssignmentCountedDetails {
    CopyAndAssignmentCountedDetails() {
        oro_atomic_set(&copies, 0);
        oro_atomic_set(&assignments, 0);
        oro_atomic_set(&refcount, 0);
    }
    oro_atomic_t copies;
    oro_atomic_t assignments;
    oro_atomic_t refcount;
};

void intrusive_ptr_add_ref(CopyAndAssignmentCountedDetails *x) { oro_atomic_inc(&(x->refcount)); }
void intrusive_ptr_release(CopyAndAssignmentCountedDetails *x) { if (oro_atomic_dec_and_test(&(x->refcount))) delete x; }

template <typename Derived>
class CopyAndAssignmentCounted : public Derived
{
public:
    typedef Derived value_type;
    typedef CopyAndAssignmentCounted<Derived> this_type;

    CopyAndAssignmentCounted()
        : Derived(), sleep_usecs_during_assignment(0), _counter_details(new CopyAndAssignmentCountedDetails)
    {
        ORO_ATOMIC_SETUP(&_write_guard, 1);
        ORO_ATOMIC_SETUP(&_read_guard, 1);
    }
    CopyAndAssignmentCounted(const value_type &value)
        : Derived(value), sleep_usecs_during_assignment(0), _counter_details(new CopyAndAssignmentCountedDetails)
    {
        ORO_ATOMIC_SETUP(&_write_guard, 1);
        ORO_ATOMIC_SETUP(&_read_guard, 1);
    }
    CopyAndAssignmentCounted(const this_type &other)
        : Derived(other), sleep_usecs_during_assignment(0), _counter_details(other._counter_details)
    {
        ORO_ATOMIC_SETUP(&_write_guard, 1);
        ORO_ATOMIC_SETUP(&_read_guard, 1);
        oro_atomic_inc(&(_counter_details->copies));
    }
    ~CopyAndAssignmentCounted() {
        ORO_ATOMIC_CLEANUP(&_write_guard);
        ORO_ATOMIC_CLEANUP(&_read_guard);
    }

    this_type &operator=(const value_type &) { throw std::runtime_error("Cannot assign CopyAndAssignmentCounted type from its value_type directly!"); }
    this_type &operator=(const this_type &other) {
        if (this == &other) return *this;

        // lock guards to detect concurrent reading and writing
        if (!oro_atomic_dec_and_test(&_write_guard))
            throw std::runtime_error("Conflicting assignment detected: instance is concurrently being assigned by another thread!");
        oro_atomic_inc(&other._read_guard);
        if (!oro_atomic_dec_and_test(&_read_guard))
            throw std::runtime_error("Conflicting assignment detected: instance being assigned is concurrently read by another thread!");

        // copy
        static_cast<value_type &>(*this) = other;
        _counter_details = other._counter_details;
        oro_atomic_inc(&(_counter_details->assignments));
        if (sleep_usecs_during_assignment > 0) usleep(sleep_usecs_during_assignment);
        if (other.sleep_usecs_during_assignment > 0) usleep(other.sleep_usecs_during_assignment);

        // cleanup guards
        oro_atomic_inc(&_read_guard);
        oro_atomic_dec(&other._read_guard);
        oro_atomic_inc(&_write_guard);
        return *this;
    }

    int getCopyCount() { return oro_atomic_read(&(_counter_details->copies)); }
    int getAssignmentCount() { return oro_atomic_read(&(_counter_details->assignments)); }
    void resetCounters() {
        oro_atomic_set(&(_counter_details->copies), 0);
        oro_atomic_set(&(_counter_details->assignments), 0);
    }

    this_type detachedCopy() const { return this_type(static_cast<const value_type &>(*this)); }

public:
    int sleep_usecs_during_assignment;

private:
    boost::intrusive_ptr<CopyAndAssignmentCountedDetails> _counter_details;
    oro_atomic_t _write_guard;
    mutable oro_atomic_t _read_guard;
};

template <typename T>
static void ResizeSample(T &sample, std::size_t size)
{
    sample.resize(size);
}

template <typename T, std::size_t N>
static void ResizeSample(CopyAndAssignmentCounted<boost::array<T,N> > &sample, std::size_t size)
{
    assert(size == N);
}

template <typename T>
static void GenerateRandomSample(T &sample, std::size_t size)
{
    ResizeSample(sample, size);
    std::srand(std::time(0));
    for(typename T::iterator it = sample.begin(); it != sample.end(); ++it)
    {
        *it = std::rand();
    }
}

template <typename T>
static void GenerateOrderedSample(T &sample, std::size_t size)
{
    ResizeSample(sample, size);
    std::size_t counter = 0;
    for(typename T::iterator it = sample.begin(); it != sample.end(); ++it)
    {
        *it = ++counter;
    }
}

timespec operator-(const timespec &a, const timespec &b)
{
    timespec result;
    result.tv_sec = a.tv_sec - b.tv_sec;
    if (a.tv_nsec >= b.tv_nsec) {
        result.tv_nsec = a.tv_nsec - b.tv_nsec;
    } else {
        result.tv_sec -= 1;
        result.tv_nsec = 1000000000 - b.tv_nsec + a.tv_nsec;
    }
    return result;
}

timespec operator+(const timespec &a, const timespec &b)
{
    timespec result;
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec >= 1000000000)
    {
        result.tv_sec += 1;
        result.tv_nsec -= 1000000000;
    }
    return result;
}

timespec operator/(const timespec &a, unsigned long divider)
{
    timespec result;

    if (divider == 0) {
        result.tv_sec = 0;
        result.tv_nsec = 0;
        return result;
    }

    result.tv_sec = a.tv_sec / divider;
    long long remainder_sec = a.tv_sec % divider;
    result.tv_nsec = a.tv_nsec / divider + ((remainder_sec * 1000000000ll) / divider);
    if (result.tv_nsec >= 1000000000)
    {
        result.tv_sec += 1;
        result.tv_nsec -= 1000000000;
    }

    return result;
}

bool operator<(const timespec &a, const timespec &b)
{
    return (a.tv_sec < b.tv_sec) || (a.tv_sec == b.tv_sec && a.tv_nsec < b.tv_nsec);
}

std::ostream &operator<<(std::ostream &stream, const timespec &tp)
{
    std::ostringstream seconds;
//    std::ios state(NULL);
//    state.copyfmt(stream);
    seconds << tp.tv_sec << "." << std::setw(9) << std::setfill('0') << tp.tv_nsec;
//    stream.copyfmt(state);
    return stream << seconds.str();
}

class Timer
{
public:
    struct Data {
        Data() { reset(); }
        void reset() { monotonic.tv_sec = 0; monotonic.tv_nsec = 0; cputime.tv_sec = 0; cputime.tv_nsec = 0; }
        operator const void *() const { return (monotonic.tv_sec == 0 && monotonic.tv_nsec == 0 && cputime.tv_sec == 0 && cputime.tv_nsec == 0) ? 0 : this; }
        Data &operator+=(const Data &other) { monotonic = monotonic + other.monotonic; cputime = cputime + other.cputime; return *this; }
        timespec monotonic;
        timespec cputime;
    };

    Timer(const std::string &name = "")
        : name_(name)
        , count_(0)
    {}

    const std::string &getName() const
    {
        return name_;
    }

    const Data &total() const { return total_; }
    const Data &min() const { return min_; }
    const Data &max() const { return max_; }
    const Data &last() const { return last_delta_; }
    std::size_t count() const { return count_; }

    Timer &operator+=(const Data &other) {
        total_ += other;
        count_++;
        return *this;
    }

    Timer &operator+=(const Timer &other) {
        total_ += other.total_;
        count_ += other.count_;
        if (!(min_.monotonic.tv_sec || min_.monotonic.tv_nsec) || (other.min_.monotonic < min_.monotonic)) min_.monotonic = other.min_.monotonic;
        if (!(min_.cputime.tv_sec || min_.cputime.tv_nsec)     || (other.min_.cputime < min_.cputime))     min_.cputime   = other.min_.cputime;
        if (!(max_.monotonic.tv_sec || max_.monotonic.tv_nsec) || (max_.monotonic < other.max_.monotonic)) max_.monotonic = other.max_.monotonic;
        if (!(max_.cputime.tv_sec || max_.cputime.tv_nsec)     || (max_.cputime < other.max_.cputime))     max_.cputime   = other.max_.cputime;
        return *this;
    }

    Timer &updateFrom(const Timer &other) {
        total_ += other.last_delta_;
        last_delta_ = other.last_delta_;
        if (!(min_.monotonic.tv_sec || min_.monotonic.tv_nsec) || (other.last_delta_.monotonic < min_.monotonic)) min_.monotonic = other.last_delta_.monotonic;
        if (!(min_.cputime.tv_sec || min_.cputime.tv_nsec)     || (other.last_delta_.cputime < min_.cputime))     min_.cputime   = other.last_delta_.cputime;
        if (!(max_.monotonic.tv_sec || max_.monotonic.tv_nsec) || (max_.monotonic < other.last_delta_.monotonic)) max_.monotonic = other.last_delta_.monotonic;
        if (!(max_.cputime.tv_sec || max_.cputime.tv_nsec)     || (max_.cputime < other.last_delta_.cputime))     max_.cputime   = other.last_delta_.cputime;
        count_++;
        return *this;
    }

    void reset()
    {
        total_.reset();
        min_.reset();
        max_.reset();
        tic_.reset();
        last_delta_.reset();
        count_ = 0;
    }

    void tic()
    {
        tic_.reset();
        last_delta_.reset();
#if defined(CLOCK_MONOTONIC)
        clock_gettime(CLOCK_MONOTONIC, &tic_.monotonic);
#elif defined(CLOCK_MONOTONIC_RAW)
        clock_gettime(CLOCK_MONOTONIC_RAW, &tic_.monotonic);
#else
        clock_gettime(CLOCK_REALTIME, &tic_.monotonic);
#endif
#if defined(CLOCK_THREAD_CPUTIME_ID)
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tic_.cputime);
#endif
    }

    Timer &toc()
    {
        if (!tic_) throw std::runtime_error("You called toc() without tic()!");

        Data toc;
#if defined(CLOCK_MONOTONIC)
        if (0 != clock_gettime(CLOCK_MONOTONIC, &toc.monotonic)) throw std::runtime_error(strerror(errno));
#elif defined(CLOCK_MONOTONIC_RAW)
        if (0 != clock_gettime(CLOCK_MONOTONIC_RAW, &toc.monotonic)) throw std::runtime_error(strerror(errno));
#else
        if (0 != clock_gettime(CLOCK_REALTIME, &toc.monotonic)) throw std::runtime_error(strerror(errno));
#endif
#if defined(CLOCK_THREAD_CPUTIME_ID)
        if (0 != clock_gettime(CLOCK_THREAD_CPUTIME_ID, &toc.cputime)) throw std::runtime_error(strerror(errno));
#endif

        count_++;
        last_delta_.monotonic = toc.monotonic - tic_.monotonic;
        last_delta_.cputime   = toc.cputime   - tic_.cputime;
        total_.monotonic = total_.monotonic + last_delta_.monotonic;
        total_.cputime   = total_.cputime   + last_delta_.cputime;
        if (!(min_.monotonic.tv_sec || min_.monotonic.tv_nsec) || (last_delta_.monotonic < min_.monotonic)) min_.monotonic = last_delta_.monotonic;
        if (!(min_.cputime.tv_sec || min_.cputime.tv_nsec)     || (last_delta_.cputime < min_.cputime))     min_.cputime   = last_delta_.cputime;
        if (!(max_.monotonic.tv_sec || max_.monotonic.tv_nsec) || (max_.monotonic < last_delta_.monotonic)) max_.monotonic = last_delta_.monotonic;
        if (!(max_.cputime.tv_sec || max_.cputime.tv_nsec)     || (max_.cputime < last_delta_.cputime))     max_.cputime   = last_delta_.cputime;

        tic_.reset();
        return *this;
    }

    class Section {
    public:
        Section(Timer &timer) : timer_(timer) { timer_.tic(); }
        ~Section() { timer_.toc(); }
    private:
        Timer &timer_;
    };

private:
    Data total_, min_, max_;
    Data tic_;
    Data last_delta_;
    std::string name_;
    std::size_t count_;
};
using ::Timer;

class ReaderWriterTaskContextBase : public RTT::TaskContext
{
public:
    ReaderWriterTaskContextBase(const std::string &name, std::size_t index = 0)
        : RTT::TaskContext(name + "[" + boost::lexical_cast<std::string>(index) + "]")
        , number_of_cycles_(0)
        , desired_number_of_cycles_(0)
    {}

    ReaderWriterTaskContextBase &setDesiredNumberOfCycles(std::size_t n) {
        MutexLock lock(mutex_);
        number_of_cycles_ = 0;
        desired_number_of_cycles_ = n;
        return *this;
    }

    ~ReaderWriterTaskContextBase() {
        BOOST_REQUIRE_MESSAGE(!this->inException(), exception_reason_);
    }

    void afterUpdateHook(bool trigger)
    {
        bool finished = false;
        {
            MutexLock lock(mutex_);
            number_of_cycles_++;
            if ((desired_number_of_cycles_ > 0) && (number_of_cycles_ >= desired_number_of_cycles_)) finished = true;
            finished_.broadcast();
        }
        if (trigger && !finished) {
            adaptor::trigger(this);
            adaptor::yield(this);
        }
        if (finished) this->stop();
    }

    bool finished() {
        MutexLock lock(mutex_);
        return (desired_number_of_cycles_ > 0) && (number_of_cycles_ >= desired_number_of_cycles_);
    }

    void waitUntilFinished()
    {
        if (inException()) return;
        MutexLock lock(mutex_);
        while(number_of_cycles_ < desired_number_of_cycles_) {
            finished_.wait(mutex_);
        }
    }

    std::size_t getNumberOfCycles()
    {
        MutexLock lock(mutex_);
        return number_of_cycles_;
    }

    std::size_t getDesiredNumberOfCycles()
    {
        MutexLock lock(mutex_);
        return desired_number_of_cycles_;
    }

protected:
    Mutex mutex_;
    Condition finished_;

    std::size_t number_of_cycles_;
    std::size_t desired_number_of_cycles_;

    std::string exception_reason_;
};

template <typename T, PortTypes PortType>
class Writer : public ReaderWriterTaskContextBase
{
public:
    typedef CopyAndAssignmentCounted<T> SampleType;
    typedef Adaptor<SampleType,PortType> AdaptorType;
    typename AdaptorType::OutputPort output_port;
    SampleType sample;
    Timer timer;
    std::map<WriteStatus, Timer> timer_by_status;

    Writer(const std::string &name, std::size_t index, std::size_t sample_size = 1, bool keep_last_written_value = false)
        : ReaderWriterTaskContextBase(name, index)
        , output_port("out")
        , timer(getName() + "::write()")
    {
#if RTT_VERSION_MAJOR >= 2
        output_port.keepLastWrittenValue(keep_last_written_value);
#endif
        AdaptorType::addPort(this, output_port);
        GenerateOrderedSample(sample, sample_size);
        AdaptorType::setDataSample(output_port, sample.detachedCopy());

        timer_by_status[WriteSuccess] = Timer(timer.getName() + "[WriteSuccess]");
        timer_by_status[WriteFailure] = Timer(timer.getName() + "[WriteFailure]");
        timer_by_status[NotConnected] = Timer(timer.getName() + "[NotConnected]");
    }

    ~Writer()
    {
        stop();
        disconnect();
    }

    void updateHook()
    {
        try {
            WriteStatus fs = WriteSuccess;
            {
                Timer::Section section(timer);
                fs = AdaptorType::write(output_port, sample);
            }
            timer_by_status[fs].updateFrom(timer);
            this->afterUpdateHook(/* trigger = */ true);
        } catch(std::exception &e) {
            exception_reason_ = e.what();
            throw;
        }
    }

    bool breakUpdateHook()
    {
        return true;
    }
};

template <typename T, PortTypes PortType>
class Reader : public ReaderWriterTaskContextBase
{
public:
    typedef CopyAndAssignmentCounted<T> SampleType;
    typedef Adaptor<SampleType,PortType> AdaptorType;
    typename AdaptorType::InputPort input_port;
    SampleType sample;
    Timer timer;
    std::map<FlowStatus, Timer> timer_by_status;

    Reader(const std::string &name, std::size_t index, bool read_loop, bool copy_old_data)
        : ReaderWriterTaskContextBase(name, index)
        , input_port("in")
        , timer(getName() + "::read()")
        , read_loop_(read_loop)
        , copy_old_data_(copy_old_data)
    {
        AdaptorType::addEventPort(this, input_port);

        timer_by_status[NoData]  = Timer(timer.getName() + "[NoData]");
        timer_by_status[OldData] = Timer(timer.getName() + "[OldData]");
        timer_by_status[NewData] = Timer(timer.getName() + "[NewData]");
    }

    ~Reader()
    {
        stop();
        disconnect();
    }

    void updateHook()
    {
        try {
            FlowStatus fs = NewData;
            while(fs == NewData) {
                {
                    Timer::Section section(timer);
                    fs = Adaptor<SampleType,PortType>::read(input_port, sample, copy_old_data_);
                }
                timer_by_status[fs].updateFrom(timer);
                if (!read_loop_) break;
            }

            this->afterUpdateHook(/* trigger = */ false);
        } catch(std::exception &e) {
            exception_reason_ = e.what();
            throw;
        }
    }

    bool breakUpdateHook()
    {
        return true;
    }

private:
    bool read_loop_;
    bool copy_old_data_;
};

template <typename T, PortTypes> class TestRunner;
typedef SAMPLE_TYPE SampleType;

struct TestOptions {
    std::size_t SampleSize;
    std::size_t NumberOfWriters;
    std::size_t NumberOfReaders;
    std::size_t NumberOfCycles;

    ConnPolicy policy;

    enum { NoWrite, WriteSynchronous, WriteAsynchronous } WriteMode;
    enum { NoRead, ReadSynchronous, ReadAsynchronous } ReadMode;

    bool KeepLastWrittenValue;
    bool ReadLoop;
    bool CopyOldData;
    int SleepUsecsDuringWriteAssignment;
    int SleepUsecsDuringReadAssignment;
    int SchedulerType;
    int ThreadPriority;
    std::bitset<16> CpuAffinity;

    TestOptions()
        : SampleSize(SAMPLE_SIZE),
          NumberOfWriters(1),
          NumberOfReaders(1),
          NumberOfCycles(NUMBER_OF_CYCLES),
          WriteMode(WriteAsynchronous),
          ReadMode(ReadAsynchronous),
          KeepLastWrittenValue(false),
          ReadLoop(false),
          CopyOldData(false),
          SleepUsecsDuringWriteAssignment(1),
          SleepUsecsDuringReadAssignment(10),
          SchedulerType(ORO_SCHED_RT),
          ThreadPriority(HighestPriority),
          CpuAffinity(getDefaultCpuAffinity() & std::bitset<16>(0x000f)) // run with at most 4 cores
    {
        policy.init = false;
    }

    static std::bitset<16> getDefaultCpuAffinity()
    {
        static std::bitset<16> default_cpu_affinity(0);
        if (default_cpu_affinity.none()) {
            Activity temp;
            default_cpu_affinity = adaptor::getCpuAffinity(temp.thread());
        }
        return default_cpu_affinity;
    }
};

const char *schedulerTypeToString(int scheduler) {
    static std::map<int, const char *> map;
    if (map.empty()) {
        map[ORO_SCHED_OTHER] = "ORO_SCHED_OTHER";
        map[ORO_SCHED_RT]    = "ORO_SCHED_RT";
    }
    return map.at(scheduler);
}

std::ostream &operator<<(std::ostream &os, const TestOptions &options)
{

    os << "***************************************************************************************************************************" << std::endl;
    os << " * RTT version:             " << _stringify(RTT_VERSION) << std::endl;
    os << " *" << std::endl;
    os << " * ConnPolicy:              " << options.policy << std::endl;
    os << " * Sample Type:             " << _stringify(SAMPLE_TYPE) << std::endl;
    os << " * Sample Size:             " << options.SampleSize << std::endl;
    os << " *" << std::endl;
    os << " * Writers:                 " << options.NumberOfWriters << std::endl;
    os << " * Readers:                 " << options.NumberOfReaders << std::endl;
    os << " * Cycles:                  " << options.NumberOfCycles << std::endl;
    os << " *" << std::endl;
    os << " * Sleep during write:      " << options.SleepUsecsDuringWriteAssignment << " usecs" << std::endl;
    os << " * Sleep during read:       " << options.SleepUsecsDuringReadAssignment << " usecs" << std::endl;
    os << " *" << std::endl;
    os << " * Scheduler:               " << schedulerTypeToString(options.SchedulerType) << std::endl;
    os << " * Priority:                " << options.ThreadPriority << std::endl;
    os << " * CPU affinity:            " << options.CpuAffinity << " (" << options.CpuAffinity.count() << " cores)" << std::endl;

    os << " *" << std::endl;
    switch(options.WriteMode) {
    case TestOptions::NoWrite:
        os << " * Write mode:              NONE (no writes)" << std::endl; break;
    case TestOptions::WriteSynchronous:
        os << " * Write mode:              Synchronous" << std::endl; break;
    case TestOptions::WriteAsynchronous:
        os << " * Write mode:              Asynchronous" << std::endl; break;
    }
    os << " * Keep last written value: " << (options.KeepLastWrittenValue ? "yes" : "no") << std::endl;
    os << " *" << std::endl;
    switch(options.ReadMode) {
    case TestOptions::NoRead:
        os << " * Read mode:               NONE (no reads)" << std::endl; break;
    case TestOptions::ReadSynchronous:
        os << " * Read mode:               Synchronous" << std::endl; break;
    case TestOptions::ReadAsynchronous:
        os << " * Read mode:               Asynchronous" << std::endl; break;
    }
    os << " * Read in loop:            " << (options.ReadLoop ? "yes" : "no") << std::endl;
    os << " * Copy old data:           " << (options.CopyOldData ? "yes" : "no") << std::endl;
    os << "***************************************************************************************************************************" << std::endl;
    os << std::endl;

    return os;
}

template <typename T, PortTypes PortType>
class TestRunner {
public:
    typedef TestRunner<T,PortType> TestRunnerType;
    typedef boost::shared_ptr<TestRunnerType> shared_ptr;

    typedef CopyAndAssignmentCounted<T> SampleType;
    typedef boost::shared_ptr< Writer<T,PortType> > WriterPtr;
    typedef boost::shared_ptr< Reader<T,PortType> > ReaderPtr;
    typedef boost::shared_ptr<ReaderWriterTaskContextBase> TaskPtr;
    typedef std::vector<WriterPtr> Writers;
    typedef std::vector<ReaderPtr> Readers;
    typedef std::vector<TaskPtr> Tasks;

    Writers writers;
    Readers readers;
    Tasks tasks;

    TestRunner(const TestOptions &options)
        : options_(options)
    {

        for(std::size_t index = 0; index < options_.NumberOfWriters; ++index) {
            WriterPtr writer(new Writer<T,PortType>("Writer", index, options_.SampleSize, options_.KeepLastWrittenValue));
            writers.push_back(writer);
            tasks.push_back(writer);
            switch(options_.WriteMode) {
            case TestOptions::WriteSynchronous:
                writer->setActivity(new SlaveActivity());
                writer->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            case TestOptions::WriteAsynchronous:
                writer->setActivity(new Activity(options_.SchedulerType, options_.ThreadPriority, 0.0, options_.CpuAffinity.to_ulong(), 0, writer->getName()));
                // adaptor::setCpuAffinity(writer.get(), options_.CpuAffinity);
                BOOST_WARN_EQUAL(writer->getActivity()->thread()->getScheduler(), options_.SchedulerType);
                BOOST_WARN_EQUAL(writer->getActivity()->thread()->getPriority(), options_.ThreadPriority);
                BOOST_WARN_EQUAL(adaptor::getCpuAffinity(writer->getActivity()->thread()), options_.CpuAffinity);
                writer->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            default:
                break;
            }
            writer->sample.sleep_usecs_during_assignment = options.SleepUsecsDuringWriteAssignment;
        }

        for(std::size_t index = 0; index < options_.NumberOfReaders; ++index) {
            ReaderPtr reader(new Reader<T,PortType>("Reader", index, options_.ReadLoop, options_.CopyOldData));
            readers.push_back(reader);
            tasks.push_back(reader);
            switch(options_.ReadMode) {
            case TestOptions::ReadSynchronous:
                reader->setActivity(new SlaveActivity());
                reader->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            case TestOptions::ReadAsynchronous:
                reader->setActivity(new Activity(options_.SchedulerType, options_.ThreadPriority, 0.0, options_.CpuAffinity.to_ulong(), 0, reader->getName()));
                // adaptor::setCpuAffinity(reader.get(), options_.CpuAffinity);
                BOOST_WARN_EQUAL(reader->getActivity()->thread()->getScheduler(), options_.SchedulerType);
                BOOST_WARN_EQUAL(reader->getActivity()->thread()->getPriority(), options_.ThreadPriority);
                BOOST_WARN_EQUAL(adaptor::getCpuAffinity(reader->getActivity()->thread()), options_.CpuAffinity);
                break;
            default:
                break;
            }
            reader->sample.sleep_usecs_during_assignment = options.SleepUsecsDuringReadAssignment;
        }
    }

    bool connectAll() {
        bool result = true;
        for(typename Writers::const_iterator writer = writers.begin(); writer != writers.end(); ++writer) {
            for(typename Readers::const_iterator reader = readers.begin(); reader != readers.end(); ++reader) {
                result = Adaptor<SampleType,PortType>::connect((*writer)->output_port, (*reader)->input_port, options_.policy) && result;
            }
        }
        return result;
    }

    void disconnectAll() {
        for(typename Writers::const_iterator task = writers.begin(); task != writers.end(); ++task) {
            (*task)->output_port.disconnect();
        }
        for(typename Readers::const_iterator task = readers.begin(); task != readers.end(); ++task) {
            (*task)->input_port.disconnect();
        }
    }

    bool startWriters() {
        if (options_.WriteMode == TestOptions::NoWrite) return true;
        bool result = true;
        for(typename Writers::const_iterator task = writers.begin(); task != writers.end(); ++task) {
            if (!(*task)->isRunning()) {
                result = (*task)->start() && result;
            }
        }
        return result;
    }

    bool startReaders() {
        if (options_.ReadMode == TestOptions::NoRead) return true;
        bool result = true;
        for(typename Readers::const_iterator task = readers.begin(); task != readers.end(); ++task) {
            if (!(*task)->isRunning()) {
                result = (*task)->start() && result;
            }
        }
        return result;
    }

    bool startAll() {
        return startWriters() && startReaders();
    }

    void stopAll() {
        for(typename Tasks::const_iterator task = tasks.begin(); task != tasks.end(); ++task) {
            (*task)->stop();
        }
    }

    void waitAll() {
        for(typename Tasks::const_iterator task = tasks.begin(); task != tasks.end(); ++task) {
            (*task)->waitUntilFinished();
        }
    }

    bool run() {
        if (!startAll()) return false;

        std::size_t cycle = 0;
        bool all_finished = false;

        while(!all_finished && (options_.NumberOfCycles == 0 || cycle < options_.NumberOfCycles)) {
            cycle++;
            all_finished = true;

            for(typename Tasks::const_iterator task = tasks.begin(); task != tasks.end(); ++task) {
                if (!(*task)->isRunning()) continue;
                adaptor::update(task->get());
                if (!(*task)->finished()) all_finished = false;
            }
        }

        waitAll();
        stopAll();
        return true;
    }

    void printStats() {
        Timer total_write_timer;
        std::map<WriteStatus, Timer> total_write_timer_by_status;

        std::cout << " Writer                                                              Monotonic Time [s]                       CPU Time [s]                                                                 " << std::endl;
        std::cout << " Task                           #Cycles          #Writes   Total        Average      Max          Total        Average      Max          #Copies   #Assign    #WSuccess #WFailure " << std::endl;
        std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        for(typename Writers::const_iterator task = writers.begin(); task != writers.end(); ++task) {
            std::cout << std::left
                      << " " << std::setw(30) << (*task)->getName()
                      << " " << std::setw(16) << (boost::lexical_cast<std::string>((*task)->getNumberOfCycles()) +
                                                  ((*task)->getDesiredNumberOfCycles() ? ("/" + boost::lexical_cast<std::string>((*task)->getDesiredNumberOfCycles())) : ""))
                      << " " << std::setw(9)  << (*task)->timer.count()
                      << " " << std::setw(12) << (*task)->timer.total().monotonic
                      << " " << std::setw(12) << ((*task)->timer.total().monotonic / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.max().monotonic
                      << " " << std::setw(12) << (*task)->timer.total().cputime
                      << " " << std::setw(12) << ((*task)->timer.total().cputime / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.max().cputime
                      << " " << std::setw(9) << (*task)->sample.getCopyCount()
                      << " " << std::setw(9) << (*task)->sample.getAssignmentCount()
                      << "  " << std::setw(9) << (*task)->timer_by_status[WriteSuccess].count()
                      << " " << std::setw(9) << (*task)->timer_by_status[WriteFailure].count()
                      << std::endl;
            if ((*task)->timer_by_status[WriteSuccess].count()) {
                std::cout << std::left
                          << " " << std::setw(30) << "  (WriteSuccess)"
                          << " " << std::setw(16) << ""
                          << " " << std::setw(9)  << (*task)->timer_by_status[WriteSuccess].count()
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteSuccess].total().monotonic
                          << " " << std::setw(12) << ((*task)->timer_by_status[WriteSuccess].total().monotonic / (*task)->timer_by_status[WriteSuccess].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteSuccess].max().monotonic
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteSuccess].total().cputime
                          << " " << std::setw(12) << ((*task)->timer_by_status[WriteSuccess].total().cputime / (*task)->timer_by_status[WriteSuccess].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteSuccess].max().cputime
                          << std::endl;
            }
            if ((*task)->timer_by_status[WriteFailure].count()) {
                std::cout << std::left
                          << " " << std::setw(30) << "  (WriteFailure)"
                          << " " << std::setw(16) << ""
                          << " " << std::setw(9)  << (*task)->timer_by_status[WriteFailure].count()
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteFailure].total().monotonic
                          << " " << std::setw(12) << ((*task)->timer_by_status[WriteFailure].total().monotonic / (*task)->timer_by_status[WriteFailure].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteFailure].max().monotonic
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteFailure].total().cputime
                          << " " << std::setw(12) << ((*task)->timer_by_status[WriteFailure].total().cputime / (*task)->timer_by_status[WriteFailure].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[WriteFailure].max().cputime
                          << std::endl;
            }

            total_write_timer += (*task)->timer;
            total_write_timer_by_status[WriteSuccess] += (*task)->timer_by_status[WriteSuccess];
            total_write_timer_by_status[WriteFailure] += (*task)->timer_by_status[WriteFailure];
            total_write_timer_by_status[NotConnected] += (*task)->timer_by_status[NotConnected];
        }
        std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << std::left
                  << " " << std::setw(30) << "Total"
                  << " " << std::setw(16) << ""
                  << " " << std::setw(9)  << total_write_timer.count()
                  << " " << std::setw(12) << total_write_timer.total().monotonic
                  << " " << std::setw(12) << (total_write_timer.total().monotonic / total_write_timer.count())
                  << " " << std::setw(12) << total_write_timer.max().monotonic
                  << " " << std::setw(12) << total_write_timer.total().cputime
                  << " " << std::setw(12) << (total_write_timer.total().cputime / total_write_timer.count())
                  << " " << std::setw(12) << total_write_timer.max().cputime
                  << " " << std::setw(9) << ""
                  << " " << std::setw(9) << ""
                  << "  " << std::setw(9) << total_write_timer_by_status[WriteSuccess].count()
                  << " " << std::setw(9) << total_write_timer_by_status[WriteFailure].count()
                  << std::endl;
        std::cout << std::left
                  << " " << std::setw(30) << "Average"
                  << " " << std::setw(16) << ""
                  << " " << std::setw(9)  << total_write_timer.count() / writers.size()
                  << " " << std::setw(12) << total_write_timer.total().monotonic / writers.size()
                  << " " << std::setw(12) << (total_write_timer.total().monotonic / total_write_timer.count()) / writers.size()
                  << " " << std::setw(12) << total_write_timer.max().monotonic / writers.size()
                  << " " << std::setw(12) << total_write_timer.total().cputime / writers.size()
                  << " " << std::setw(12) << (total_write_timer.total().cputime / total_write_timer.count()) / writers.size()
                  << " " << std::setw(12) << total_write_timer.max().cputime / writers.size()
                  << " " << std::setw(9) << ""
                  << " " << std::setw(9) << ""
                  << "  " << std::setw(9) << total_write_timer_by_status[WriteSuccess].count() / writers.size()
                  << " " << std::setw(9) << total_write_timer_by_status[WriteFailure].count() / writers.size()
                  << std::endl;
        std::cout << std::endl;

        Timer total_read_timer;
        std::map<FlowStatus, Timer> total_read_timer_by_status;

        std::cout << " Reader                                                              Monotonic Time [s]                       CPU Time [s]                                                    " << std::endl;
        std::cout << " Task                           #Cycles          #Reads    Total        Average      Max          Total        Average      Max          #NewData  #OldData  #NoData " << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        for(typename Readers::const_iterator task = readers.begin(); task != readers.end(); ++task) {
            std::cout << std::left
                      << " " << std::setw(30) << (*task)->getName()
                      << " " << std::setw(16) << (boost::lexical_cast<std::string>((*task)->getNumberOfCycles()) +
                                                  ((*task)->getDesiredNumberOfCycles() ? ("/" + boost::lexical_cast<std::string>((*task)->getDesiredNumberOfCycles())) : ""))
                      << " " << std::setw(9)  << (*task)->timer.count()
                      << " " << std::setw(12) << (*task)->timer.total().monotonic
                      << " " << std::setw(12) << ((*task)->timer.total().monotonic / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.max().monotonic
                      << " " << std::setw(12) << (*task)->timer.total().cputime
                      << " " << std::setw(12) << ((*task)->timer.total().cputime / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.max().cputime
                      << " " << std::setw(9) << (*task)->timer_by_status[NewData].count()
                      << " " << std::setw(9) << (*task)->timer_by_status[OldData].count()
                      << " " << std::setw(9) << (*task)->timer_by_status[NoData].count()
                      << std::endl;
            if ((*task)->timer_by_status[NewData].count()) {
                std::cout << std::left
                          << " " << std::setw(30) << "  (NewData)"
                          << " " << std::setw(16) << ""
                          << " " << std::setw(9)  << (*task)->timer_by_status[NewData].count()
                          << " " << std::setw(12) << (*task)->timer_by_status[NewData].total().monotonic
                          << " " << std::setw(12) << ((*task)->timer_by_status[NewData].total().monotonic / (*task)->timer_by_status[NewData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[NewData].max().monotonic
                          << " " << std::setw(12) << (*task)->timer_by_status[NewData].total().cputime
                          << " " << std::setw(12) << ((*task)->timer_by_status[NewData].total().cputime / (*task)->timer_by_status[NewData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[NewData].max().cputime
                          << std::endl;
            }
            if ((*task)->timer_by_status[OldData].count()) {
                std::cout << std::left
                          << " " << std::setw(30) << "  (OldData)"
                          << " " << std::setw(16) << ""
                          << " " << std::setw(9)  << (*task)->timer_by_status[OldData].count()
                          << " " << std::setw(12) << (*task)->timer_by_status[OldData].total().monotonic
                          << " " << std::setw(12) << ((*task)->timer_by_status[OldData].total().monotonic / (*task)->timer_by_status[OldData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[OldData].max().monotonic
                          << " " << std::setw(12) << (*task)->timer_by_status[OldData].total().cputime
                          << " " << std::setw(12) << ((*task)->timer_by_status[OldData].total().cputime / (*task)->timer_by_status[OldData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[OldData].max().cputime
                          << std::endl;
            }
            if ((*task)->timer_by_status[NoData].count()) {
                std::cout << std::left
                          << " " << std::setw(30) << "  (NoData)"
                          << " " << std::setw(16) << ""
                          << " " << std::setw(9)  << (*task)->timer_by_status[NoData].count()
                          << " " << std::setw(12) << (*task)->timer_by_status[NoData].total().monotonic
                          << " " << std::setw(12) << ((*task)->timer_by_status[NoData].total().monotonic / (*task)->timer_by_status[NoData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[NoData].max().monotonic
                          << " " << std::setw(12) << (*task)->timer_by_status[NoData].total().cputime
                          << " " << std::setw(12) << ((*task)->timer_by_status[NoData].total().cputime / (*task)->timer_by_status[NoData].count())
                          << " " << std::setw(12) << (*task)->timer_by_status[NoData].max().cputime
                          << std::endl;
            }

            total_read_timer += (*task)->timer;
            total_read_timer_by_status[NewData] += (*task)->timer_by_status[NewData];
            total_read_timer_by_status[OldData] += (*task)->timer_by_status[OldData];
            total_read_timer_by_status[NoData] += (*task)->timer_by_status[NoData];
        }
        std::cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << std::left
                  << " " << std::setw(30) << "Total"
                  << " " << std::setw(16) << ""
                  << " " << std::setw(9)  << total_read_timer.count()
                  << " " << std::setw(12) << total_read_timer.total().monotonic
                  << " " << std::setw(12) << (total_read_timer.total().monotonic / total_read_timer.count())
                  << " " << std::setw(12) << total_read_timer.max().monotonic
                  << " " << std::setw(12) << total_read_timer.total().cputime
                  << " " << std::setw(12) << (total_read_timer.total().cputime / total_read_timer.count())
                  << " " << std::setw(12) << total_read_timer.max().cputime
                  << " " << std::setw(9) << total_read_timer_by_status[NewData].count()
                  << " " << std::setw(9) << total_read_timer_by_status[OldData].count()
                  << " " << std::setw(9) << total_read_timer_by_status[NoData].count()
                  << std::endl;
        std::cout << std::endl;
    }

    const TestOptions &getOptions() const { return options_; }

private:
    TestOptions options_;
};

template <PortTypes PortType, typename Enabled = void>
struct DataFlowPerformanceTest_ {
    TestOptions options;
    typedef TestRunner<SampleType,PortType> RunnerType;
    typename RunnerType::shared_ptr runner;

    DataFlowPerformanceTest_();
    void run() {
        std::cout << runner->getOptions();
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }
};

template <>
DataFlowPerformanceTest_<DataPortType>::DataFlowPerformanceTest_()
{
    options.policy.type = ConnPolicy::DATA;
//    options.policy.lock_policy = ConnPolicy::LOCK_FREE;
}

template <>
DataFlowPerformanceTest_<BufferPortType>::DataFlowPerformanceTest_()
{
    options.policy.type = ConnPolicy::BUFFER;
    options.policy.size = 100;
//    options.policy.lock_policy = ConnPolicy::LOCK_FREE;
}

struct EmptyReads;
template <>
DataFlowPerformanceTest_<DataPortType, EmptyReads>::DataFlowPerformanceTest_()
{
    options.policy.type = ConnPolicy::DATA;
//    options.policy.lock_policy = ConnPolicy::LOCKED;

    options.WriteMode = TestOptions::NoWrite;
    options.ReadMode = TestOptions::ReadSynchronous;
}

// Registers the fixture into the 'registry'
typedef DataFlowPerformanceTest_<DataPortType> DataFlowPerformanceTest_Data;
BOOST_FIXTURE_TEST_SUITE( DataFlowPerformanceTest_Data_Suite, DataFlowPerformanceTest_Data )

#if (RTT_VERSION_MAJOR >= 2)
// 7 writers, 1 reader, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerConnection_7Writers1Reader )
{
    options.NumberOfWriters = 7;
    options.NumberOfReaders = 1;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 7 writers, 1 reader, PerInputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerInputPort_7Writers1Reader )
{
    options.NumberOfWriters = 7;
    options.NumberOfReaders = 1;
    options.policy.buffer_policy = PerInputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR >= 2)
// 1 writer, 7 readers, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerConnection_1Writer7Readers )
{
    options.NumberOfWriters = 1;
    options.NumberOfReaders = 7;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 1 writer, 7 readers, PerOutputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerOutputPort_1Writer7Readers )
{
    options.NumberOfWriters = 1;
    options.NumberOfReaders = 7;
    options.policy.buffer_policy = PerOutputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR >= 2)
// 4 writers, 4 readers, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerConnection_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}

#if RTT_VERSION_GTE(2,8,99)
// 4 writers, 4 readers, PerInputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerInputPort_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerInputPort;
    runner.reset(new RunnerType(options));
    run();
}

// 4 writers, 4 readers, PerOutputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_PerOutputPort_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerOutputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 4 writers, 4 readers, Shared
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Data_Shared_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = Shared;
    runner.reset(new RunnerType(options));
    run();
}
#endif

BOOST_AUTO_TEST_SUITE_END()

// Registers the fixture into the 'registry'
typedef DataFlowPerformanceTest_<BufferPortType> DataFlowPerformanceTest_Buffer;
BOOST_FIXTURE_TEST_SUITE( DataFlowPerformanceTest_Buffer_Suite, DataFlowPerformanceTest_Buffer )

#if (RTT_VERSION_MAJOR >= 2)
// 7 writers, 1 reader, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerConnection_7Writers1Reader )
{
      options.NumberOfWriters = 7;
      options.NumberOfReaders = 1;
      options.policy.buffer_policy = PerConnection;
      runner.reset(new RunnerType(options));

      std::cout << runner->getOptions();
      BOOST_CHECK( runner->connectAll() );
      BOOST_CHECK( runner->run() );

      runner->printStats();
      std::cout << std::endl;
    }
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 7 writers, 1 reader, PerInputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerInputPort_7Writers1Reader )
{
    options.NumberOfWriters = 7;
    options.NumberOfReaders = 1;
    options.policy.buffer_policy = PerInputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR >= 2)
// 1 writer, 7 readers, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerInputPort_1Writer7Readers )
{
    options.NumberOfWriters = 1;
    options.NumberOfReaders = 7;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 1 writer, 7 readers, PerOutputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerOutputPort_1Writer7Readers )
{
    options.NumberOfWriters = 1;
    options.NumberOfReaders = 7;
    options.policy.buffer_policy = PerOutputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR >= 2)
// 4 writers, 4 readers, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerConnection_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}

#if RTT_VERSION_GTE(2,8,99)
// 4 writers, 4 readers, PerInputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerInputPort_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerInputPort;
    runner.reset(new RunnerType(options));
    run();
}

// 4 writers, 4 readers, PerOutputPort
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_PerOutputPort_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = PerOutputPort;
    runner.reset(new RunnerType(options));
    run();
}
#endif
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 4 writers, 4 readers, Shared
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_Buffer_Shared_4Writers4Readers )
{
    options.NumberOfWriters = 4;
    options.NumberOfReaders = 4;
    options.policy.buffer_policy = Shared;
    runner.reset(new RunnerType(options));
    run();
}
#endif

BOOST_AUTO_TEST_SUITE_END()

// Registers the fixture into the 'registry'
typedef DataFlowPerformanceTest_<DataPortType, EmptyReads> DataFlowPerformanceTest_EmptyReads;
BOOST_FIXTURE_TEST_SUITE( DataFlowPerformanceTest_EmptyReads_Suite, DataFlowPerformanceTest_EmptyReads )

#if (RTT_VERSION_MAJOR >= 2)
// 7 writers, 1 reader, PerConnection
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_EmptyReads_PerConnection_7Writers1Reader )
{
    options.NumberOfWriters = 7;
    options.NumberOfReaders = 1;
    options.policy.buffer_policy = PerConnection;
    runner.reset(new RunnerType(options));
    run();
}
#endif

#if (RTT_VERSION_MAJOR == 1) || RTT_VERSION_GTE(2,8,99)
// 7 writers, 1 reader, Shared
BOOST_AUTO_TEST_CASE( DataFlowPerformanceTest_EmptyReads_Shared_7Writers1Reader )
{
    options.NumberOfWriters = 7;
    options.NumberOfReaders = 1;
    options.policy.buffer_policy = Shared;
    runner.reset(new RunnerType(options));
    run();
}
#endif

BOOST_AUTO_TEST_SUITE_END()
