#ifndef TASKPREEMPTIBLE_HPP
#define TASKPREEMPTIBLE_HPP

#include "ZeroLatencyThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * A TaskPreemptible is a RealTimeTask which may be preempted by
     * a TaskNonPreemptible. Typical TaskPreemptible instances are
     * large calculations or slower realtime device communications.
     * 
     * They can only be run in the ZeroLatencyThread.
     */
    class TaskPreemptible
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskPreemptible with a periodicity <period>
         * Seconds
         */
        TaskPreemptible(Seconds period, RunnableInterface* r=0 );
        /**
         * Construct a TaskPreemptible with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskPreemptible(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskPreemptible();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        /**
         * The ZeroLatencyThread used for scheduling this task
         */
        ZeroLatencyThread* zlt;

    };

}

#endif
