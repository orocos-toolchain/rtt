
#include "corelib/SimulationThread.hpp"
#include "corelib/HeartBeatGenerator.hpp"
#include "corelib/TaskSimulation.hpp"

#include "pkgconf/corelib_tasks.h"

// NO AUTO START !

namespace ORO_CoreLib
{
    
    // The static class variables
    SimulationThread* SimulationThread::_instance;

    SimulationThread* SimulationThread::Instance()
    {
        if ( _instance == 0 )
        {
            _instance = new SimulationThread();
        }

        return _instance;
    }

    bool SimulationThread::Release()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
            return true;
        }

        return false;
    }


    SimulationThread::SimulationThread()
        : TaskExecution(ORONUM_CORELIB_TASKS_SIM_PRIORITY,
                        ORODAT_CORELIB_TASKS_SIM_NAME, 
                        ORONUM_CORELIB_TASKS_SIM_PERIOD),
          beat( HeartBeatGenerator::Instance() )
    {
        // we will update the clock in step()
        beat->enableSystemClock( false );
        this->continuousStepping( true );
    }

    SimulationThread::~SimulationThread()
    {
        beat->enableSystemClock( true );
    }

    bool SimulationThread::taskAdd( TaskSimulation* t, const nsecs n )
    {
        return TaskExecution::taskAdd( t, n );
    }

    void SimulationThread::taskRemove( TaskSimulation* t )
    {
        TaskExecution::taskRemove( t );
    }

    void SimulationThread::step()
    {
        TaskExecution::step();
        beat->secondsChange(ORONUM_CORELIB_TASKS_SIM_PERIOD);
    }

}
