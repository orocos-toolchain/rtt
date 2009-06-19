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
#include "os/MainThread.hpp"

namespace RTT
{
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
        return mperiod;
    }

    OS::ThreadInterface* SlaveActivity::thread()
    {
        return mmaster ? mmaster->thread() : OS::MainThread::Instance();
    }

    bool SlaveActivity::initialize()
    {
        return true;
    }

    void SlaveActivity::step()
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
        if ( (mmaster && !mmaster->isActive()) || active == true )
            return false;

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

    bool SlaveActivity::execute()
    {
        // non periodic case.
        if ( mperiod == 0.0 ) {
            if ( !active || running )
                return false;
            running = true;
            if (runner)
                runner->loop();
            else
                this->loop();
            running = false;
            return true;
        }

        if ( running ) {
            if (runner) runner->step(); else this->step();
        }
        return running;
    }


}
