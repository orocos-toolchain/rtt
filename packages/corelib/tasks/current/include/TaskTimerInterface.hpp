#ifndef TASKTIMERINTERFACE_HPP
#define TASKTIMERINTERFACE_HPP

#include "Time.hpp"

namespace ORO_CoreLib
{
    class PeriodicTask;
    /**
     * An TaskTimerInterface is an object that will step() a PeriodicTask every
     * n'th time it is tick()'ed such that when n tasks are present,
     * each task will be stepped on a different tick call of one period.
     * It will try to spread the stepping
     * depending on the task's periods and tick period.
     * The former is the period between two consecutive step() calls,
     *  the latter, the elapsed time between two tick() calls.
     *
     */
    class TaskTimerInterface
    {
    public:
        virtual ~TaskTimerInterface() {}

        /**
         * The maximum number of tasks each timer will handle, thus,
         * the max number of tasks of the same priority.
         */
        static const unsigned int MAX_TASKS = 64;

        /**
         * Add a task which must be step()'ed.
         */
        virtual bool taskAdd( PeriodicTask* t ) = 0;

        /**
         * Remove a task which must no longer be step()'ed.
         */
        virtual bool taskRemove( PeriodicTask* t ) = 0;

        /**
         * Execute a PeriodicTask, according to the average period.
         */
        virtual void tick() = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the tasks are step()'ed.
         */
        virtual nsecs periodGet() const = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the timer is tick()'ed.
         */
        virtual nsecs triggerGet() const = 0;

        /**
         * Sets the (average) period at which this timer will be ticked.
         */
        virtual void triggerSet(nsecs ns) = 0;
        
    };

}

#endif
