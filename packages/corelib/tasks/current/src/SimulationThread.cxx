/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationThread.cxx 

                        SimulationThread.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

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
        this->continuousStepping( true );
    }

    SimulationThread::~SimulationThread()
    {
    }

    bool SimulationThread::taskAdd( TaskSimulation* t, const nsecs n )
    {
        return TaskExecution::taskAdd( t, n );
    }

    void SimulationThread::taskRemove( TaskSimulation* t )
    {
        TaskExecution::taskRemove( t );
    }

    bool SimulationThread::initialize()
    {
        // we will update the clock in step()
        beat->enableSystemClock( false );
        return true;
    }

    void SimulationThread::finalize()
    {
        // release systemclock again.
        beat->enableSystemClock( true );
    }

    void SimulationThread::step()
    {
        TaskExecution::step();
        beat->secondsChange(ORONUM_CORELIB_TASKS_SIM_PERIOD);
    }

}
