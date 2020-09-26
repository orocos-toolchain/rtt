/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationThread.cxx

                        SimulationThread.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "rtt/extras/SimulationThread.hpp"
#include "rtt/os/TimeService.hpp"
#include "rtt/extras/SimulationActivity.hpp"
#include "rtt/Logger.hpp"
#include "rtt/os/threads.hpp"
#include "rtt/os/MainThread.hpp"

#include "rtt/os/StartStopManager.hpp"
namespace RTT {
    using namespace extras;
    namespace
    {
        // Stop it before the application quits.
        void stopSIMThread()
        {
            SimulationThread::Release();
        }

        os::CleanupFunction SIMCleanup( &stopSIMThread );
    }
}

namespace RTT {
    using namespace extras;
    using namespace os;

    // The static class variables
    SimulationThreadPtr SimulationThread::_instance;

    SimulationThreadPtr SimulationThread::Instance(double period)
    {
        if ( !_instance )
        {
            _instance.reset( new SimulationThread(period) );
        }

        return _instance;
    }

    bool SimulationThread::Release()
    {
        _instance.reset();
        return true;
    }


    SimulationThread::SimulationThread(double period)
        : TimerThread( os::LowestPriority,
                      "SimulationThread",
                      period),
          beat( TimeService::Instance() ), maxsteps_(0), sim_running(false)
    {
        Logger::In in("SimulationThread");
        this->setScheduler(ORO_SCHED_OTHER);
        Logger::log() << Logger::Info << this->getName() <<" created with "<< this->getPeriod() <<"s periodicity";
        Logger::log() << Logger::Info << " and priority " << this->getPriority() << Logger::endl;
    }

    SimulationThread::~SimulationThread()
    {
        this->stop();
    }

    bool SimulationThread::isRunning() const
    {
        return sim_running || Thread::isRunning();
    }

    bool SimulationThread::start()
    {
        maxsteps_ = 0;
        return os::Thread::start();
    }

    bool SimulationThread::start(unsigned int maxsteps)
    {
        if (maxsteps == 0)
            return false;
        maxsteps_ = maxsteps;
        return os::Thread::start();
    }

    bool SimulationThread::run(unsigned int ms)
    {
        if ( ms == 0 || this->isRunning() || this->initialize() == false )
            return false;
        unsigned int cur = 0;
        this->sim_running = true;
        while( cur != ms ) {
            ++cur;
            TimerThread::step();
            beat->secondsChange(this->getPeriod());
        }
        this->sim_running = false;
        this->finalize();
        return true;
    }
    os::ThreadInterface* SimulationThread::simthread() {
        return os::MainThread::Instance();
    }

    bool SimulationThread::initialize()
    {
        Logger::In in("SimulationThread");
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
        Logger::In in("SimulationThread");
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
            beat->secondsChange(this->getPeriod());
        }

        // call stop once :
        if ( cursteps == maxsteps_ ) { // if maxsteps == 0, will never call stop().
            this->stop();
        }
    }

}
