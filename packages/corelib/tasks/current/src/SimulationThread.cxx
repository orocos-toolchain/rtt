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
#include "corelib/TimeService.hpp"
#include "corelib/TaskSimulation.hpp"
#include "corelib/Logger.hpp"

#include "pkgconf/corelib_tasks.h"

// NO AUTO START ! Only create/release
#ifdef OROSEM_CORELIB_TASKS_AUTOSTART
#include <os/StartStopManager.hpp>
namespace ORO_CoreLib
{
    namespace
    {
        int startSIMThread()
        {
            SimulationThread::Instance();
            return true;
        }

        void stopSIMThread()
        {
            SimulationThread::Release();
        }

        ORO_OS::InitFunction SIMInit( &startSIMThread );
        ORO_OS::CleanupFunction SIMCleanup( &stopSIMThread );
    }
}
#endif


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
        : TimerThread(ORONUM_CORELIB_TASKS_SIM_PRIORITY,
                        ORODAT_CORELIB_TASKS_SIM_NAME, 
                        ORONUM_CORELIB_TASKS_SIM_PERIOD),
          beat( TimeService::Instance() )
    {
        this->continuousStepping( true );
        Logger::log() << Logger::Info << ORODAT_CORELIB_TASKS_SIM_NAME <<" created with "<< ORONUM_CORELIB_TASKS_SIM_PERIOD <<"ms periodicity";
        Logger::log() << Logger::Info << " and priority " << ORONUM_CORELIB_TASKS_SIM_PRIORITY << Logger::endl;
    }

    SimulationThread::~SimulationThread()
    {
    }
    bool SimulationThread::initialize()
    {
        Logger::log() << Logger::Info << "SimulationThread takes over system time."<<Logger::nl;
        Logger::log() << Logger::Info << "System time will increase significantly faster."<<Logger::endl;
        
        // we will update the clock in step()
        beat->enableSystemClock( false );

        // No TimerThread::initialize() to allow 'freeze'
        return true;
    }

    void SimulationThread::finalize()
    {
        Logger::log() << Logger::Info << "SimulationThread releases system time."<<Logger::endl;
        // release systemclock again.
        beat->enableSystemClock( true );

        // DO NOT CALL TimerThread::finalize(), since we want to be able to start/stop the
        // SimulationThread and inspect the tasks still running.
    }

    void SimulationThread::step()
    {
        TimerThread::step();
        beat->secondsChange(ORONUM_CORELIB_TASKS_SIM_PERIOD);
    }

}
