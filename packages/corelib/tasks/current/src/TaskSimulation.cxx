#include "corelib/TaskSimulation.hpp"

namespace ORO_CoreLib
{
    
    TaskSimulation::TaskSimulation(Seconds period, RunnableInterface* r )
        :RealTimeTask(period, r)
    {
        simt = SimulationThread::Instance();
    }

    TaskSimulation::TaskSimulation(secs sec, nsecs nsec, RunnableInterface* r )
        :RealTimeTask(sec, nsec, r)
    {
        simt = SimulationThread::Instance();
    }

    TaskSimulation::~TaskSimulation()
    {
        stop();
    }

    bool TaskSimulation::taskAdd()
    {
        return simt->taskAdd(this, this->per_ns);
    }

    void TaskSimulation::taskRemove()
    {
        simt->taskRemove(this);
    }
}
