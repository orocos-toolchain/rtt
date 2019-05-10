/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  PeriodicActivity.cxx

                        PeriodicActivity.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "../Time.hpp"
#include "PeriodicActivity.hpp"
#include "../os/MutexLock.hpp"
#include "../Logger.hpp"
#include "TimerThread.hpp"
#include <cmath>

namespace RTT {
    using namespace extras;
    using namespace base;

    PeriodicActivity::PeriodicActivity(int priority, Seconds period, RunnableInterface* r )
        : ActivityInterface(r), running(false), active(false),
          thread_( TimerThread::Instance(priority,period) )
    {
        this->init();
    }

    PeriodicActivity::PeriodicActivity(int scheduler, int priority, Seconds period, RunnableInterface* r )
        : ActivityInterface(r), running(false), active(false),
          thread_( TimerThread::Instance(scheduler, priority,period) )
    {
        this->init();
    }

    PeriodicActivity::PeriodicActivity(int scheduler, int priority, Seconds period, unsigned cpu_affinity, RunnableInterface* r )
        : ActivityInterface(r), running(false), active(false),
          thread_( TimerThread::Instance(scheduler, priority, period, cpu_affinity) )
    {
        this->init();
    }

    PeriodicActivity::PeriodicActivity(TimerThreadPtr thread, RunnableInterface* r )
        : ActivityInterface(r), running(false), active(false),
          thread_( thread )
    {
        this->init();
    }

    PeriodicActivity::PeriodicActivity(Seconds period, TimerThreadPtr thread, RunnableInterface* r )
        : ActivityInterface(r), running(false), active(false),
          thread_(thread)
    {
        this->init();
    }

    PeriodicActivity::PeriodicActivity(secs s, nsecs ns, TimerThreadPtr thread, RunnableInterface* r )
        : ActivityInterface(r),
          running(false), active(false),
          thread_(thread)
    {
        this->init();
    }

    PeriodicActivity::~PeriodicActivity()
    {
        stop();
    }

    void PeriodicActivity::init() {
    }

    bool PeriodicActivity::start()
    {
        if ( isActive() || !thread_ ) {
            //Logger::log() << Logger::Error << "PeriodicActivity : already active or thread not running." << Logger::endl;
            return false;
        }
        // If thread is not yet running, try to start it.
        if ( !thread_->isRunning() && thread_->start() == false )
            return false;

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

    bool PeriodicActivity::stop()
    {
        if ( !isActive() ) return false;

        // since removeActivity synchronises, we do not need to mutex-lock
        // stop()
        if ( thread_->removeActivity( this ) ) {
            running = false;
            this->finalize();
            active = false;
            return true;
        }
        return false;
    }

    bool PeriodicActivity::isRunning() const
    {
        return running;
    }

    bool PeriodicActivity::isActive() const
    {
        return active;
    }

    Seconds PeriodicActivity::getPeriod() const
    {
        return thread_->getPeriod();
    }

    bool PeriodicActivity::setPeriod(Seconds s) {
        return false;
    }

    unsigned PeriodicActivity::getCpuAffinity() const
    {
      return thread_->getCpuAffinity();
    }

    bool PeriodicActivity::setCpuAffinity(unsigned cpu)
    {
      return thread_->setCpuAffinity(cpu);
    }

    bool PeriodicActivity::initialize() {
        if (runner != 0)
            return runner->initialize();
        else
            return true;
    }

    bool PeriodicActivity::execute()
    {
        return false;
    }

    bool PeriodicActivity::timeout()
    {
        return false;
    }

    bool PeriodicActivity::trigger()
    {
        return false;
    }

    void PeriodicActivity::step()
    {
        // override this method to avoid running runner.
        if (runner != 0)
            runner->step();
    }

    void PeriodicActivity::work(RunnableInterface::WorkReason reason)
    {
        // override this method to avoid running runner.
        if (runner != 0)
            runner->work(reason);
    }

    void PeriodicActivity::finalize() {
        if (runner != 0)
            runner->finalize();
    }

    os::ThreadInterface* PeriodicActivity::thread() { return thread_.get(); }

    bool PeriodicActivity::isPeriodic() const {
        return true;
    }
}
