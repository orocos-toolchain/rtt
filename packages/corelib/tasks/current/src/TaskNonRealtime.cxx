#include "corelib/TaskNonRealTime.hpp"

namespace ORO_CoreLib
{
    
    TaskNonRealTime::TaskNonRealTime(Seconds period, RunnableInterface* r )
        :RealTimeTask(period, r)
    {
        cproc = NonRealTimeThread::Instance();
    }

    TaskNonRealTime::TaskNonRealTime(secs sec, nsecs nsec, RunnableInterface* r )
        :RealTimeTask(sec, nsec, r)
    {
        cproc = NonRealTimeThread::Instance();
    }

    TaskNonRealTime::~TaskNonRealTime()
    {
        stop();
    }

    bool TaskNonRealTime::taskAdd()
    {
        return cproc->taskAdd(this, per_ns);
    }

    void TaskNonRealTime::taskRemove()
    {
        cproc->taskRemove(this);
    }
}

