#ifndef TASKLOCKFREETIMER_HPP
#define TASKLOCKFREETIMER_HPP

#include "TaskTimerInterface.hpp"
#include <vector>
#include "ListLockFree.hpp"

namespace ORO_CoreLib
{
    namespace detail {

    /**
     * A lock-free TaskTimer which step()s all tasks in 'one shot' in the
     * first tick given of each task period. It does discard ticks
     * if there are multiple ticks per task period, hence not spreading
     * task step()s over ticks. If a task in this timer tries to start
     * or stop another task in this timer, the effect will only be in the next tick.
     */
    class TaskTimerLockFree
        : public TaskTimerInterface
    {
        typedef ListLockFree<PeriodicTask*> TaskList ;
        nsecs trig_per;
        nsecs task_per;
        int   turn;
        TaskList tasks;
    public:
        TaskTimerLockFree( nsecs task_period );

        virtual ~TaskTimerLockFree();

        virtual void start();

        virtual void stop();

        virtual bool addTask( PeriodicTask* t );

        virtual bool removeTask( PeriodicTask* t );

        virtual void tick();

        virtual nsecs getPeriod() const { return task_per; }

        virtual nsecs getTrigger() const { return trig_per; }

        virtual void setTrigger(nsecs ns) { trig_per = ns; }

    };
    }
}

#endif
