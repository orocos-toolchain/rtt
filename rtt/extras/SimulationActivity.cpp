/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationActivity.cxx

                        SimulationActivity.cxx -  description
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


#include "SimulationActivity.hpp"
#include "SimulationThread.hpp"

namespace RTT {
    using namespace base;
    using namespace extras;

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

    os::ThreadInterface* SimulationActivity::thread() {
        return SimulationThread::Instance()->simthread();
    }

    bool SimulationActivity::execute()
    {
        if ( this->isRunning() ) {
            this->step();
            this->work(RunnableInterface::TimeOut);
            return true;
        }
        return false;
    }


    // COPY/PASTE from PeriodicActivity.cxx
    bool SimulationActivity::start()
    {
        if ( isActive() ) { //|| !thread_->isRunning() ) {
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
        res = thread_->addActivity( this );
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
