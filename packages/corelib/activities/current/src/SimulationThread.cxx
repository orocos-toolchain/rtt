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
 
 

#include "rtt/SimulationThread.hpp"
#include "rtt/TimeService.hpp"
#include "rtt/SimulationActivity.hpp"
#include "rtt/Logger.hpp"
#include <rtt/os/threads.hpp>

#include "pkgconf/corelib_activities.h"

// NO AUTO START ! Only create/release
#ifdef OROSEM_CORELIB_ACTIVITIES_AUTOSTART
#include <rtt/os/StartStopManager.hpp>
namespace RTT
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

        OS::InitFunction SIMInit( &startSIMThread );
        OS::CleanupFunction SIMCleanup( &stopSIMThread );
    }
}
#endif


namespace RTT
{
    
    // The static class variables
    SimulationThreadPtr SimulationThread::_instance;

    SimulationThreadPtr SimulationThread::Instance()
    {
        if ( !_instance )
        {
            _instance.reset( new SimulationThread() );
        }

        return _instance;
    }

    bool SimulationThread::Release()
    {
        _instance.reset();
        return true;
    }


    SimulationThread::SimulationThread()
        : TimerThread( OS::LowestPriority,
                      ORODAT_CORELIB_ACTIVITIES_SIM_NAME, 
                      ORONUM_CORELIB_ACTIVITIES_SIM_PERIOD),
          beat( TimeService::Instance() ), maxsteps_(0)
    {
        this->continuousStepping( true );
        Logger::log() << Logger::Info << ORODAT_CORELIB_ACTIVITIES_SIM_NAME <<" created with "<< ORONUM_CORELIB_ACTIVITIES_SIM_PERIOD <<"s periodicity";
        Logger::log() << Logger::Info << " and priority " << this->getPriority() << Logger::endl;
    }

    SimulationThread::~SimulationThread()
    {
    }

    bool SimulationThread::run(unsigned int ms)
    {
        if ( this->initialize() == false || ms == 0)
            return false;
        unsigned int cur = 0;
        while( cur != ms ) {
            ++cur;
            TimerThread::step();
            beat->secondsChange(ORONUM_CORELIB_ACTIVITIES_SIM_PERIOD);
        }
        this->finalize();
        return true;
    }

    bool SimulationThread::initialize()
    {
        Logger::log() << Logger::Info << "SimulationThread takes over system time."<<Logger::nl;
        Logger::log() << Logger::Info << "System time will increase significantly faster."<<Logger::endl;
        
        // we will update the clock in step()
        beat->enableSystemClock( false );

        cursteps = 0;
        // No TimerThread::initialize() to allow 'freeze'
        return true;
    }

    void SimulationThread::finalize()
    {
        Logger::log() << Logger::Info << "SimulationThread releases system time."<<Logger::endl;
        // release systemclock again.
        beat->enableSystemClock( true );

        // DO NOT CALL TimerThread::finalize(), since we want to be able to start/stop the
        // SimulationThread and inspect the activities still running.
    }

    void SimulationThread::step()
    {
        ++cursteps;

        if ( maxsteps_ == 0 || cursteps < maxsteps_ + 1 ) {
            TimerThread::step();
            beat->secondsChange(ORONUM_CORELIB_ACTIVITIES_SIM_PERIOD);
        }

        // call stop once :
        if ( cursteps == maxsteps_ ) { // if maxsteps == 0, will never call stop().
            this->setToStop();
        }
    }

}
