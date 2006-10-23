/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationActivity.cxx 

                        SimulationActivity.cxx -  description
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
 
 
#include "rtt/SimulationActivity.hpp"
#include "rtt/SimulationThread.hpp"
#include "rtt/TimerInterface.hpp"

namespace RTT
{
    
    SimulationActivity::SimulationActivity(Seconds period, RunnableInterface* r )
        :PeriodicActivity(period,SimulationThread::Instance(), r)
    {
    }

    SimulationActivity::SimulationActivity(secs sec, nsecs nsec, RunnableInterface* r )
        :PeriodicActivity(sec, nsec,SimulationThread::Instance(), r)
    {
    }

    SimulationActivity::~SimulationActivity()
    {
        stop();
    }

    bool SimulationActivity::execute()
    {
        if ( this->isRunning() ) {
            this->step();
            return true;
        }
        return false;
    }
        

    // COPY/PASTE from PeriodicActivity.cxx
    bool SimulationActivity::start()
    {
        if ( !timer_ || isActive() ) { //|| !thread_->isRunning() ) {
            //Logger::log() << Logger::Error << "PeriodicActivity : no timer, already active or thread not running." << Logger::endl;
            return false;
        }
	
        active = true;
        bool inError = !this->initialize();
        if ( inError ) {
            //Logger::log() << Logger::Error << "PeriodicActivity : initialize() returned false " << Logger::endl;
            active = false;
            return false;
        }

        bool res;
        res = timer_->addActivity( this );
        if ( res == false ) {
            //Logger::log() << Logger::Error << "PeriodicActivity : addActivity() returned false " << Logger::endl;
            this->finalize();
            active = false;
            return false;
        }

        running = true;
        return true;
    }

}
