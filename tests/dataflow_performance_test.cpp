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

#include <boost/lexical_cast.hpp>

#include <vector>
#include <boost/array.hpp>

#define SAMPLE_SIZE 10000
// #define SAMPLE_TYPE boost::array<double,SAMPLE_SIZE>
#define SAMPLE_TYPE std::vector<double>
#define NUMBER_OF_CYCLES 1000

enum PortTypes { DataPortType = 1, BufferPortType = 2 };
template <typename T, PortTypes> struct Adaptor;


#if (RTT_VERSION_MAJOR == 1)

    #include "TaskContext.hpp"
    #include "DataPort.hpp"
    #include "BufferPort.hpp"
    #include "ConnPolicy.hpp"
    #include "os/Mutex.hpp"
    #include "SlaveActivity.hpp"

    using RTT::TaskContext;
    using RTT::ConnPolicy;
    enum FlowStatus { NoData, OldData, NewData, WriteSuccess, WriteFailure, NotConnected };

    using RTT::OS::Mutex;
    using RTT::OS::MutexLock;
    struct Condition
    {
        void broadcast() {}
        void wait(Mutex &) {}
    };

    using RTT::SlaveActivity;

    template <typename T, PortTypes>
    struct Adaptor {};

    template <typename T>
    struct Adaptor<T,DataPortType>
    {
        typedef RTT::DataPortBase<T> Port;
        typedef RTT::WriteDataPort<T> OutputPort;
        typedef RTT::ReadDataPort<T> InputPort;

        static void addPort(TaskContext *tc, Port &port) {

        }

        static void addEventPort(TaskContext *tc, InputPort &port) {

        }

        static void setDataSample(OutputPort &port, const T &sample) {

        }

        static FlowStatus write(OutputPort &port, const T &sample) {

        }

        static FlowStatus read(InputPort &port, T &sample, bool copy_old_data) {

        }

        static bool connect(OutputPort &output_port, InputPort &input_port, const ConnPolicy &cp) {

        }
    };

    template <typename T>
    struct Adaptor<T,BufferPortType>
    {
        typedef RTT::BufferPortBase<T> Port;
        typedef RTT::WriteBufferPort<T> OutputPort;
        typedef RTT::ReadBufferPort<T> InputPort;

        static void addPort(TaskContext *tc, Port &port) {

        }

        static void addEventPort(TaskContext *tc, InputPort &port) {

        }

        static void setDataSample(OutputPort &port, const T &sample) {

        }

        static FlowStatus write(OutputPort &port, const T &sample) {

        }

        static FlowStatus read(InputPort &port, T &sample, bool copy_old_data) {

        }

        static bool connect(OutputPort &output_port, InputPort &input_port, const ConnPolicy &cp) {

        }
    };

    namespace adaptor
    {
        static bool trigger(TaskContext *tc) {

        }

        static bool update(TaskContext *tc) {

        }
    }

#else
    #include <rtt/TaskContext.hpp>
    #include <rtt/InputPort.hpp>
    #include <rtt/OutputPort.hpp>
    #include <rtt/extras/SlaveActivity.hpp>

    using RTT::TaskContext;
    using RTT::ConnPolicy;

#if RTT_VERSION_GTE(2,8,99)
    using RTT::FlowStatus;
    using RTT::WriteSuccess;
    using RTT::WriteFailure;
    using RTT::NotConnected;
#else
    typedef int FlowStatus;
    enum { WriteSuccess = 3, WriteFailure = 4, NotConnected = 5 };
#endif
    using RTT::NoData;
    using RTT::OldData;
    using RTT::NewData;

    using RTT::os::Mutex;
    using RTT::os::MutexLock;
    using RTT::os::Condition;

    using RTT::extras::SlaveActivity;

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
        static FlowStatus write(OutputPort &port, const T &sample) {
            return port.write(sample);
        }
#else
        static FlowStatus write(OutputPort &port, const T &sample) {
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

    namespace adaptor
    {
        static bool trigger(TaskContext *tc) {
            return tc->trigger();
        }

        static bool update(TaskContext *tc) {
            return tc->update();
        }
    }
#endif

#ifndef _stringify
  #define _stringify(x) _stringify2(x)
  #ifndef _stringify2
    #define _stringify2(x...) #x
  #endif
#endif

struct CopyAndAssignmentCountedDetails {
    CopyAndAssignmentCountedDetails() { oro_atomic_set(&copies, 0); oro_atomic_set(&assignments, 0); oro_atomic_set(&refcount, 0); }
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

    CopyAndAssignmentCounted() : Derived(), _counter_details(new CopyAndAssignmentCountedDetails) {}
    CopyAndAssignmentCounted(const value_type &value) : Derived(value), _counter_details(new CopyAndAssignmentCountedDetails) {}
    CopyAndAssignmentCounted(const this_type &other) : Derived(other), _counter_details(other._counter_details) { oro_atomic_inc(&(_counter_details->copies)); }
    this_type &operator=(const value_type &) { throw std::runtime_error("Cannot assign CopyAndAssignmentCounted type from its value_type directly!"); }
    this_type &operator=(const this_type &other) { static_cast<value_type &>(*this) = other; _counter_details = other._counter_details; oro_atomic_inc(&(_counter_details->assignments)); return *static_cast<this_type *>(this); }

    int getCopyCount() { return oro_atomic_read(&(_counter_details->copies)); }
    int getAssignmentCount() { return oro_atomic_read(&(_counter_details->assignments)); }
    void resetCounters() {
        oro_atomic_set(&(_counter_details->copies), 0);
        oro_atomic_set(&(_counter_details->assignments), 0);
    }

    this_type detachedCopy() const { return this_type(static_cast<const value_type &>(*this)); }

private:
    boost::intrusive_ptr<CopyAndAssignmentCountedDetails> _counter_details;
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
    const Data &last() const { return last_delta_; }
    std::size_t count() const { return count_; }

    Timer &operator+=(const Data &other) {
        total_ += other;
        count_++;
        return *this;
    }

    Timer &operator+=(const Timer &other) {
        total_ += other.last_delta_;
        last_delta_ = other.last_delta_;
        count_++;
        return *this;
    }

    void reset()
    {
        total_.reset();
        tic_.reset();
        last_delta_.reset();
        count_ = 0;
    }

    void tic()
    {
        tic_.reset();
        last_delta_.reset();
        clock_gettime(CLOCK_MONOTONIC, &tic_.monotonic);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tic_.cputime);
    }

    Timer &toc()
    {
        if (!tic_) throw std::runtime_error("You called toc() without tic()!");

        Data toc;
        if (0 != clock_gettime(CLOCK_MONOTONIC, &toc.monotonic)) throw std::runtime_error(strerror(errno));
        if (0 != clock_gettime(CLOCK_THREAD_CPUTIME_ID, &toc.cputime)) throw std::runtime_error(strerror(errno));

        count_++;
        last_delta_.monotonic = toc.monotonic - tic_.monotonic;
        last_delta_.cputime   = toc.cputime   - tic_.cputime;
        total_.monotonic = total_.monotonic + last_delta_.monotonic;
        total_.cputime   = total_.cputime   + last_delta_.cputime;

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
    Data total_;
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

    void afterUpdateHook(bool trigger)
    {
        bool finished = false;
        {
            MutexLock lock(mutex_);
            number_of_cycles_++;
            if ((desired_number_of_cycles_ > 0) && (number_of_cycles_ >= desired_number_of_cycles_)) finished = true;
            finished_.broadcast();
        }
        if (trigger && !finished) adaptor::trigger(this);
        if (finished) this->stop();
    }

    bool finished() {
        MutexLock lock(mutex_);
        return (desired_number_of_cycles_ > 0) && (number_of_cycles_ >= desired_number_of_cycles_);
    }

    void waitUntilFinished()
    {
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

    Writer(const std::string &name, std::size_t index, std::size_t sample_size = 1, bool keep_last_written_value = false)
        : ReaderWriterTaskContextBase(name, index)
        , output_port("out")
        , timer(getName() + "::read()")
    {
#if RTT_VERSION_MAJOR == 2
        output_port.keepLastWrittenValue(keep_last_written_value);
#endif
        AdaptorType::addPort(this, output_port);
        GenerateOrderedSample(sample, sample_size);
        AdaptorType::setDataSample(output_port, sample.detachedCopy());
    }

    ~Writer()
    {
        stop();
        disconnect();
    }

    void updateHook()
    {
        {
            Timer::Section section(timer);
            AdaptorType::write(output_port, sample);
        }
        this->afterUpdateHook(/* trigger = */ true);
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
        FlowStatus fs = NewData;
        while(fs == NewData) {
            {
                Timer::Section section(timer);
                fs = Adaptor<SampleType,PortType>::read(input_port, sample, copy_old_data_);
            }
            timer_by_status[fs] += timer;
            if (!read_loop_) break;
        }

        this->afterUpdateHook(/* trigger = */ false);
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

    TestOptions()
        : SampleSize(SAMPLE_SIZE),
          NumberOfWriters(1),
          NumberOfReaders(1),
          NumberOfCycles(NUMBER_OF_CYCLES),
          WriteMode(WriteSynchronous),
          ReadMode(ReadAsynchronous),
          KeepLastWrittenValue(false),
          ReadLoop(false),
          CopyOldData(false)
    {
        policy.init = false;
    }
};

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
struct TestRunner {
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

    TestRunner(const std::string &name, const TestOptions &options)
        : options_(options)
    {
        for(std::size_t index = 0; index < options_.NumberOfWriters; ++index) {
            WriterPtr writer(new Writer<T,PortType>(name + "Writer", index, options_.SampleSize, options_.KeepLastWrittenValue));
            writers.push_back(writer);
            tasks.push_back(writer);
            switch(options_.WriteMode) {
            case TestOptions::WriteSynchronous:
                writer->setActivity(new SlaveActivity());
                writer->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            case TestOptions::WriteAsynchronous:
                writer->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            default:
                break;
            }
        }

        for(std::size_t index = 0; index < options_.NumberOfReaders; ++index) {
            ReaderPtr reader(new Reader<T,PortType>(name + "Reader", index, options_.ReadLoop, options_.CopyOldData));
            readers.push_back(reader);
            tasks.push_back(reader);
            switch(options_.ReadMode) {
            case TestOptions::ReadSynchronous:
                reader->setActivity(new SlaveActivity());
                reader->setDesiredNumberOfCycles(options_.NumberOfCycles);
                break;
            default:
                break;
            }
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
        return startReaders() && startWriters();
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
        std::cout << " Writer                                                       Monotonic Time [s]           CPU Time [s]                               " << std::endl;
        std::cout << " Task                           #Cycles          #Writes   Total        Average      Total        Average      #Copies   #Assignments " << std::endl;
        std::cout << "--------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        for(typename Writers::const_iterator task = writers.begin(); task != writers.end(); ++task) {
            std::cout << std::left
                      << " " << std::setw(30) << (*task)->getName()
                      << " " << std::setw(16) << (boost::lexical_cast<std::string>((*task)->getNumberOfCycles()) +
                                                  ((*task)->getDesiredNumberOfCycles() ? ("/" + boost::lexical_cast<std::string>((*task)->getDesiredNumberOfCycles())) : ""))
                      << " " << std::setw(9)  << (*task)->timer.count()
                      << " " << std::setw(12) << (*task)->timer.total().monotonic
                      << " " << std::setw(12) << ((*task)->timer.total().monotonic / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.total().cputime
                      << " " << std::setw(12) << ((*task)->timer.total().cputime / (*task)->timer.count())
                      << " " << std::setw(9) << (*task)->sample.getCopyCount()
                      << " " << std::setw(9) << (*task)->sample.getAssignmentCount()
                      << std::endl;
        }
        std::cout << std::endl;

        std::cout << " Reader                                                       Monotonic Time [s]           CPU Time [s]                                    " << std::endl;
        std::cout << " Task                           #Cycles          #Reads    Total        Average      Total        Average      #NewData  #OldData  #NoData " << std::endl;
        std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
        for(typename Readers::const_iterator task = readers.begin(); task != readers.end(); ++task) {
            std::cout << std::left
                      << " " << std::setw(30) << (*task)->getName()
                      << " " << std::setw(16) << (boost::lexical_cast<std::string>((*task)->getNumberOfCycles()) +
                                                  ((*task)->getDesiredNumberOfCycles() ? ("/" + boost::lexical_cast<std::string>((*task)->getDesiredNumberOfCycles())) : ""))
                      << " " << std::setw(9)  << (*task)->timer.count()
                      << " " << std::setw(12) << (*task)->timer.total().monotonic
                      << " " << std::setw(12) << ((*task)->timer.total().monotonic / (*task)->timer.count())
                      << " " << std::setw(12) << (*task)->timer.total().cputime
                      << " " << std::setw(12) << ((*task)->timer.total().cputime / (*task)->timer.count())
                      << " " << std::setw(9) << (*task)->timer_by_status[NewData].count()
                      << " " << std::setw(9) << (*task)->timer_by_status[OldData].count()
                      << " " << std::setw(9) << (*task)->timer_by_status[NoData].count()
                      << std::endl;
            std::cout << std::left
                      << " " << std::setw(30) << "  (NewData)"
                      << " " << std::setw(16) << ""
                      << " " << std::setw(9)  << (*task)->timer_by_status[NewData].count()
                      << " " << std::setw(12) << (*task)->timer_by_status[NewData].total().monotonic
                      << " " << std::setw(12) << ((*task)->timer_by_status[NewData].total().monotonic / (*task)->timer_by_status[NewData].count())
                      << " " << std::setw(12) << (*task)->timer_by_status[NewData].total().cputime
                      << " " << std::setw(12) << ((*task)->timer_by_status[NewData].total().cputime / (*task)->timer_by_status[NewData].count())
                      << std::endl;
            std::cout << std::left
                      << " " << std::setw(30) << "  (OldData)"
                      << " " << std::setw(16) << ""
                      << " " << std::setw(9)  << (*task)->timer_by_status[OldData].count()
                      << " " << std::setw(12) << (*task)->timer_by_status[OldData].total().monotonic
                      << " " << std::setw(12) << ((*task)->timer_by_status[OldData].total().monotonic / (*task)->timer_by_status[OldData].count())
                      << " " << std::setw(12) << (*task)->timer_by_status[OldData].total().cputime
                      << " " << std::setw(12) << ((*task)->timer_by_status[OldData].total().cputime / (*task)->timer_by_status[OldData].count())
                      << std::endl;
            std::cout << std::left
                      << " " << std::setw(30) << "  (NoData)"
                      << " " << std::setw(16) << ""
                      << " " << std::setw(9)  << (*task)->timer_by_status[NoData].count()
                      << " " << std::setw(12) << (*task)->timer_by_status[NoData].total().monotonic
                      << " " << std::setw(12) << ((*task)->timer_by_status[NoData].total().monotonic / (*task)->timer_by_status[NoData].count())
                      << " " << std::setw(12) << (*task)->timer_by_status[NoData].total().cputime
                      << " " << std::setw(12) << ((*task)->timer_by_status[NoData].total().cputime / (*task)->timer_by_status[NoData].count())
                      << std::endl;
        }
        std::cout << std::endl;
    }

private:
    TestOptions options_;
};

#include <boost/test/unit_test.hpp>

// Registers the test suite into the 'registry'
BOOST_AUTO_TEST_SUITE( DataFlowPerformanceTest )

BOOST_AUTO_TEST_CASE( dataConnection )
{
    TestOptions options;
    const PortTypes PortType = DataPortType;
    typedef TestRunner<SampleType,PortType> RunnerType;

    // 10 writers, 1 reader, push
    {
        options.NumberOfWriters = 10;
        options.NumberOfReaders = 1;
        options.policy.pull = ConnPolicy::PUSH;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 10 writers, 1 reader, pull
    {
        options.NumberOfWriters = 10;
        options.NumberOfReaders = 1;
        options.policy.pull = ConnPolicy::PULL;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 1 writer, 10 readers, push
    {
        options.NumberOfWriters = 1;
        options.NumberOfReaders = 10;
        options.policy.pull = ConnPolicy::PUSH;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 1 writer, 10 readers, pull
    {
        options.NumberOfWriters = 1;
        options.NumberOfReaders = 10;
        options.policy.pull = ConnPolicy::PULL;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 4 writers, 4 readers, push
    {
        options.NumberOfWriters = 4;
        options.NumberOfReaders = 4;
        options.policy.pull = ConnPolicy::PUSH;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 4 writers, 4 readers, pull
    {
        options.NumberOfWriters = 4;
        options.NumberOfReaders = 4;
        options.policy.pull = ConnPolicy::PULL;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("data", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }
}

BOOST_AUTO_TEST_CASE( emptyReads )
{
    TestOptions options;
    const PortTypes PortType = DataPortType;
    typedef TestRunner<SampleType,PortType> RunnerType;

    options.WriteMode = TestOptions::NoWrite;
    options.ReadMode = TestOptions::ReadSynchronous;

    // 10 writers, 1 reader, push
    {
        options.NumberOfWriters = 10;
        options.NumberOfReaders = 1;
        options.policy.pull = ConnPolicy::PUSH;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("no", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }

    // 10 writers, 1 reader, pull
    {
        options.NumberOfWriters = 10;
        options.NumberOfReaders = 1;
        options.policy.pull = ConnPolicy::PULL;
        std::cout << options;

        typename RunnerType::shared_ptr runner(new RunnerType("empty", options));
        BOOST_CHECK( runner->connectAll() );
        BOOST_CHECK( runner->run() );

        runner->printStats();
        std::cout << std::endl;
    }
}


BOOST_AUTO_TEST_SUITE_END()
