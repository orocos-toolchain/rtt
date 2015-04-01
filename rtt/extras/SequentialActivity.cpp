/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  SequentialActivity.cpp

                        SequentialActivity.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#include "SequentialActivity.hpp"
#include "../os/MainThread.hpp"
#include "../os/MutexLock.hpp"

namespace RTT {
    using namespace extras;
    using namespace base;

    SequentialActivity::SequentialActivity( RunnableInterface* run /*= 0*/ )
        : ActivityInterface(run), running(false), active(false)
    {
    }

    SequentialActivity::~SequentialActivity()
    {
        stop();
    }

    Seconds SequentialActivity::getPeriod() const
    {
        return 0.0;
    }

    bool SequentialActivity::setPeriod(Seconds s) {
        if ( s == 0.0)
            return true;
        return false;
    }

    unsigned SequentialActivity::getCpuAffinity() const
    {
      return ~0;
    }

    bool SequentialActivity::setCpuAffinity(unsigned cpu)
    {
      return false;
    }

    os::ThreadInterface* SequentialActivity::thread()
    {
        return os::MainThread::Instance();
    }

    bool SequentialActivity::initialize()
    {
        return true;
    }

    void SequentialActivity::step()
    {
    }

    bool SequentialActivity::breakLoop()
    {
        return false;
    }


    void SequentialActivity::finalize()
    {
    }

    bool SequentialActivity::start()
    {
        if (active == true )
            return false;

        active = true;

        if ( runner ? runner->initialize() : this->initialize() ) {
        } else {
            active = false;
        }
        return active;
    }


    bool SequentialActivity::stop()
    {
        if ( !active )
            return false;

        if (runner)
            runner->finalize();
        else
            this->finalize();
        active = false;
        return true;
    }

    bool SequentialActivity::isRunning() const
    {
        return running;
    }

    bool SequentialActivity::isPeriodic() const
    {
        return false;
    }

    bool SequentialActivity::isActive() const
    {
        return active;
    }

    bool SequentialActivity::trigger()
    {
        // This function may recurse, in which case it returns true.
        // We could also rely on the MutexTryLock to fail, but in
        // case an OS only has recursive mutexes, we'd need to
        // check running anyway before calling runner->step(). So
        // we moved that piece of code up front.
        // The other thread will complete the work. (hasWork).
        if (running)
            return true;
        if ( active ) {
            bool did_step = false;
            do {
                os::MutexTryLock lock(execution_lock);
                if ( lock.isSuccessful() ) {
                    running = true;
                    if (runner) {
                        runner->step();
                        runner->work(RunnableInterface::TimeOut); // for sequentials, every trigger is also a TimeOut...
                    }  else {
                        this->step();
                    }
                    running = false;
                    did_step = true;
                } else {
                    // nop: step() is already being executed and
                    // should notice that new data is there.
                    // race: step() returns and trigger is called->
                    // trigger is ignored.
                    return true;
                }
                // mutex unlocked here.
            } // if the runner signals work again (ie a race occured),
              // rety to lock.
            while ( did_step && runner->hasWork() );
            return true;
        }
        return false;
    }

    bool SequentialActivity::timeout()
    {
        return trigger();
    }

    bool SequentialActivity::execute()
    {
        return false;
    }


}
