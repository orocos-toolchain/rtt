#include "corelib/TaskNonPreemptible.hpp"

namespace ORO_CoreLib
{
    
    TaskNonPreemptible::TaskNonPreemptible(Seconds period, RunnableInterface* r )
        :RealTimeTask(period, r)
    {
        ztt = ZeroTimeThread::Instance();
    }

    TaskNonPreemptible::TaskNonPreemptible(secs sec, nsecs nsec, RunnableInterface* r )
        :RealTimeTask(sec, nsec, r)
    {
        ztt = ZeroTimeThread::Instance();
    }

    TaskNonPreemptible::~TaskNonPreemptible()
    {
        stop();
    }

    bool TaskNonPreemptible::taskAdd()
    {
        return ztt->taskAdd(this, per_ns);
    }

    void TaskNonPreemptible::taskRemove()
    {
        ztt->taskRemove(this);
    }
}
