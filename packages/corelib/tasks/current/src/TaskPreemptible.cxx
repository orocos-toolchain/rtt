#include "corelib/TaskPreemptible.hpp"

namespace ORO_CoreLib
{
    
    TaskPreemptible::TaskPreemptible(Seconds period, RunnableInterface* r )
        :RealTimeTask(period, r)
    {
        zlt = ZeroLatencyThread::Instance();
    }

    TaskPreemptible::TaskPreemptible(secs sec, nsecs nsec, RunnableInterface* r )
        :RealTimeTask(sec, nsec, r)
    {
        zlt = ZeroLatencyThread::Instance();
    }

    TaskPreemptible::~TaskPreemptible()
    {
        stop();
    }

    bool TaskPreemptible::taskAdd()
    {
        return zlt->taskAdd(this, per_ns);
    }

    void TaskPreemptible::taskRemove()
    {
        zlt->taskRemove(this);
    }
}
