#ifndef TASKNONPREEMPTIBLE_HPP
#define TASKNONPREEMPTIBLE_HPP

#include "ZeroTimeThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * A TaskNonPreemptible is a RealTimeTask which will never be preempted by
     * another RealTimeTask. Typical TaskNonPreemptible instances are
     * fast calculations with fast device input and output communication.
     * 
     * They can only be run in the ZeroTimeThread.
     */
    class TaskNonPreemptible
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskNonPreemptible with a periodicity <period>
         * Seconds
         */
        TaskNonPreemptible(Seconds period, RunnableInterface* r=0 );

        /**
         * Construct a TaskNonPreemptible with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskNonPreemptible(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskNonPreemptible();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        /**
         * The ZeroTimeThread used for scheduling this task
         */
        ZeroTimeThread* ztt;

    };

}

#endif
