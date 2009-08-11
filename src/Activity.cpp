
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "Time.hpp"
#include "Activity.hpp"
#include "os/MutexLock.hpp"
#include "Logger.hpp"
#include "extras/TimerThread.hpp"
#include <cmath>

namespace RTT
{
    Activity::Activity(RunnableInterface* _r, const std::string& name )
        : ActivityInterface(_r), OS::Thread(ORO_SCHED_OTHER, 0, 0.0, name )
    {
    }

    Activity::Activity(int priority, Seconds period, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), OS::Thread(ORO_SCHED_RT, priority, period, name )
    {
    }

     Activity::Activity(int scheduler, int priority, Seconds period, RunnableInterface* r, const std::string& name )
     : ActivityInterface(r), OS::Thread(scheduler, priority, period, name )
     {
     }

    Activity::~Activity()
    {
        stop();
    }

    OS::ThreadInterface* Activity::thread() {
        return this;
    }

    bool Activity::initialize() {
        if ( runner )
            return runner->initialize();
        return true;
    }

    void Activity::step() {
        if ( runner )
            runner->step();
    }

    void Activity::loop() {
        if ( runner )
            runner->loop();
    }

    bool Activity::breakLoop() {
        if ( runner )
            return runner->breakLoop();
        return false;
    }

    void Activity::finalize() {
        if ( runner )
            runner->finalize();
    }


    bool Activity::start() {
        return Thread::start();
    }

    bool Activity::stop() {
        return Thread::stop();
    }

    bool Activity::trigger() {
        return Thread::isActive() ? Thread::start() : false;
    }

    bool Activity::execute() {
        return false;
    }

    bool Activity::isRunning() const {
        return Thread::isRunning();
    }

    bool Activity::isActive() const {
        return Thread::isActive();
    }

    Seconds Activity::getPeriod() const
    {
        return Thread::getPeriod();
    }

    bool Activity::setPeriod(Seconds period)
    {
        return Thread::setPeriod(period);
    }


    bool Activity::isPeriodic() const {
        return Thread::isPeriodic();
    }
}
