#ifndef TASKONESHOTTIMER_HPP
#define TASKONESHOTTIMER_HPP

#include "TaskTimerInterface.hpp"
#include <vector>
#include <os/Mutex.hpp>

namespace ORO_CoreLib
{

    /**
     * A TaskTimer which step()s all tasks in 'one shot' in the
     * first tick given of each task period. It does discard ticks
     * if there are multiple ticks per task period, hence not spreading
     * task step()s over ticks.
     */
    class TaskTimerOneShot
        : public TaskTimerInterface
    {
        typedef std::vector<PeriodicTask*> TaskList ;
        nsecs trig_per;
        nsecs task_per;
        int   turn;
        TaskList tasks;
        bool cleanup;
        /**
         * A recursive mutex allows non-deadlocking self-removal.
         */
        ORO_OS::MutexRecursive mutex;
        void reorderList();
    public:
        TaskTimerOneShot( nsecs task_period );

        virtual ~TaskTimerOneShot();

        bool addTask( PeriodicTask* t );

        bool removeTask( PeriodicTask* t );

        virtual void tick();

        virtual nsecs getPeriod() const { return task_per; }

        virtual nsecs getTrigger() const { return trig_per; }

        virtual void setTrigger(nsecs ns) { trig_per = ns; }

    };
}

#endif
