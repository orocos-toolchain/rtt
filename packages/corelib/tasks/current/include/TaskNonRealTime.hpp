#ifndef TASKNONREALTIME_HPP
#define TASKNONREALTIME_HPP

#include "NonRealTimeThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * A TaskNonRealTime is a RealTimeTask which will never preempt
     * another RealTimeTask. Typical TaskNonRealTime instances are
     * not realtime user communications, GUI updates and so on. 
     * 
     * They can only be run in the NonRealTimeThread.
     */
    class TaskNonRealTime
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskNonRealTime with a periodicity <period>
         * Seconds
         */
        TaskNonRealTime(Seconds period, RunnableInterface* r=0 );

        /**
         * Construct a TaskNonRealTime with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskNonRealTime(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskNonRealTime();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        /**
         * The NonRealTimeThread used for scheduling this task
         */
        NonRealTimeThread* cproc;

    };

}

#endif
