#ifndef ORO_TASKTIMERSEQUENCER_HPP
#define ORO_TASKTIMERSEQUENCER_HPP

#include "Time.hpp"
#include "os/Mutex.hpp"
#include "TaskTimerInterface.hpp"

#include <list>
#include <functional>

namespace ORO_CoreLib
{

    /**
     * An TaskTimerSequencer is an object that will step() a PeriodicTask every
     * n'th time it is tick()'ed such that when n tasks are present,
     * each task will be stepped on a different tick call of one period.
     * It will try to spread the stepping
     * depending on the task's periods and trigger period.
     * The former is the period between two consecutive step() calls,
     *  the latter, the elapsed time between two tick() calls.
     *
     * The implementation is based on <std::list>. This allows safe
     * add/remove of handlers during tick().
     *
     * @todo More efficient implementation if average_period == trigger_period.
     */
    class TaskTimerSequencer
        : public TaskTimerInterface
    {
    public:
        /**
         * Creates an TaskTimerSequencer which will have step() all tasks after
         * <average_period> / <trigger_period> times it is ticked.
         *
         * @param average_period The Average time between step()'ing each
         *        registered listener.
         * @param trigger_period The period between consecutive tick() calls.
         *        Defaults to average_period.
         */
        TaskTimerSequencer( Seconds average_period, Seconds trigger_period = 0 );

        /**
         * Creates an TaskTimerSequencer which will have step()'ed each task
         * after tick() was called <divider> times.
         *
         * @param divider The average number of times tick() is called before
         *        each task is step()'ed. ( So N Tasks will all be 
         *        step()'ed after tick() was called <divider> times. )
         */
        TaskTimerSequencer( unsigned int divider );

        void tick();

        bool taskAdd( PeriodicTask* task );

        bool taskRemove( PeriodicTask* task );

        nsecs periodGet() const;

        nsecs triggerGet() const;

        void triggerSet(nsecs ns);
        
    protected:

        struct ListItem
        {
            // The default constructor is called when the 
            // list is resized().
            ListItem() : task(0), priority(~0), turn(0) {}
            PeriodicTask* task;
            unsigned int priority;
            unsigned int turn;
        };

        struct CMP : public std::binary_function<ListItem, PeriodicTask*, bool>
        { 
            bool operator()(const ListItem& p, const PeriodicTask* task) const
            {
                return p.task == task;
            }
        };

        struct PrioritySort : public std::binary_function<ListItem, ListItem, bool>
        {
            bool operator()(const ListItem& f, const ListItem& s) const
            {
                return f.priority < s.priority;
            }
        };

        inline
        void update();

        nsecs avPeriod;

        nsecs trigPeriod;

        unsigned long relPeriod;

        /**
         * monotonic increasing number.
         */
        unsigned long total_added;

        unsigned long turn;

        std::list<ListItem> sortedList;

        std::list<ListItem>::iterator f_iter;

        bool reset_f_iter;

        ORO_OS::MutexRecursive mut;

        bool in_tick;
    };

}

#endif
