/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  SlaveActivity.cxx

                        SlaveActivity.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#include "SlaveActivity.hpp"
#include "../os/MainThread.hpp"
#include "Logger.hpp"

namespace RTT {
    using namespace extras;
    using namespace base;
    SlaveActivity::SlaveActivity( ActivityInterface* master, RunnableInterface* run /*= 0*/ )
        :ActivityInterface(run), mmaster(master), mperiod( master->getPeriod() ), running(false), active(false)
    {
    }

    SlaveActivity::SlaveActivity( double period, RunnableInterface* run /*= 0*/ )
        :ActivityInterface(run), mmaster(0), mperiod(period), running(false), active(false)
    {
    }

    SlaveActivity::SlaveActivity( RunnableInterface* run /*= 0*/ )
        :ActivityInterface(run), mmaster(0), mperiod(0.0), running(false), active(false)
    {
    }

    SlaveActivity::~SlaveActivity()
    {
        stop();
    }

    Seconds SlaveActivity::getPeriod() const
    {
        if (mmaster)
            return mmaster->getPeriod(); // master can change period.
        return mperiod;
    }

    bool SlaveActivity::setPeriod(Seconds s) {
        if (mmaster)
            return false; // refuse to set period if master is involved.
        mperiod = s;
        return true;
    }

    unsigned SlaveActivity::getCpuAffinity() const
    {
        if (mmaster)
            return mmaster->getCpuAffinity();
        return ~0;
    }

    bool SlaveActivity::setCpuAffinity(unsigned cpu)
    {
        return false;
    }

    os::ThreadInterface* SlaveActivity::thread()
    {
        return mmaster ? mmaster->thread() : os::MainThread::Instance();
    }

    base::ActivityInterface *SlaveActivity::getMaster() const
    {
        return mmaster;
    }

    bool SlaveActivity::initialize()
    {
        return true;
    }

    void SlaveActivity::step()
    {
    }

    void SlaveActivity::work(base::RunnableInterface::WorkReason reason)
    {
    }

    void SlaveActivity::loop()
    {
        this->step();
    }

    bool SlaveActivity::breakLoop()
    {
        return false;
    }


    void SlaveActivity::finalize()
    {
    }

    bool SlaveActivity::start()
    {
        if (mmaster && !mmaster->isActive())
        {
            Logger::log() << Logger::Error << "Unable to start slave as master activity is not running" << Logger::endl;
            return false;
        }
        if ( active == true )
        {
            Logger::log() << Logger::Error  << "Unable to start slave as it is already started" << Logger::endl;
            return false;
        }

        active = true;

        if ( runner ? runner->initialize() : this->initialize() ) {
            running = this->isPeriodic();
        } else {
            active = false;
        }
        return active;
    }


    bool SlaveActivity::stop()
    {
        if ( !active )
            return false;

        // use breakLoop if not periodic and within loop
        if ( this->isPeriodic() == false) {
            if ( running && (runner ? (runner->breakLoop() == false): (this->breakLoop() == false) ) )
                return false;
        }

        running = false;
        if (runner)
            runner->finalize();
        else
            this->finalize();
        active = false;
        return true;
    }

    bool SlaveActivity::isRunning() const
    {
        return running;
    }

    bool SlaveActivity::isPeriodic() const
    {
        return mperiod != 0.0;
    }
    bool SlaveActivity::isActive() const
    {
        return active;
    }

    bool SlaveActivity::trigger()
    {
        if (mmaster)
            return mmaster->trigger();
        return false;
    }

    bool SlaveActivity::timeout()
    {
        if (mmaster)
            return mmaster->timeout();
        return false;
    }

    bool SlaveActivity::execute()
    {
        // non periodic case.
        if ( mperiod == 0.0 ) {
            if ( !active || running )
                return false;
            running = true;
            // Since we're in execute(), this is semantically forcing a TimeOut towards the runner.
            if (runner) {
                runner->loop();
                runner->work(RunnableInterface::TimeOut);
            } else {
                this->loop();
                this->work(RunnableInterface::TimeOut);
            }
            running = false;
            return true;
        }

        // executing a slave is semantical identical to a timeout happening:
        if ( running ) {
            if (runner) {
                runner->step();
                runner->work(RunnableInterface::TimeOut);
            } else {
                this->step();
                this->work(RunnableInterface::TimeOut);
            }
        }
        return running;
    }


}
