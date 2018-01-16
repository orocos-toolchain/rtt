/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  Activity.cpp

                        Activity.cpp -  description
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



#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "Time.hpp"
#include "Activity.hpp"
#include "os/MutexLock.hpp"
#include "Logger.hpp"
#include "rtt-fwd.hpp"
#include "os/fosi_internal_interface.hpp"

#include <cmath>

namespace RTT
{
    using namespace detail;

    Activity::Activity(RunnableInterface* _r, const std::string& name )
        : ActivityInterface(_r), os::Thread(ORO_SCHED_OTHER, RTT::os::LowestPriority, 0.0, 0, name ),
          update_period(0.0), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
    {
    }

    Activity::Activity(int priority, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(ORO_SCHED_RT, priority, 0.0, 0, name ),
          update_period(0.0), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
    {
    }

    Activity::Activity(int priority, Seconds period, RunnableInterface* r, const std::string& name )
        : ActivityInterface(r), os::Thread(ORO_SCHED_RT, priority, period, 0, name ),
          update_period(period), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
    {
        // We pass the requested period to the constructor to not confuse users with log messages.
        // Then we clear it immediately again in order to force the Thread implementation to
        // non periodic:
        Thread::setPeriod(0,0);
    }

     Activity::Activity(int scheduler, int priority, RunnableInterface* r, const std::string& name )
         : ActivityInterface(r), os::Thread(scheduler, priority, 0.0, 0, name ),
           update_period(0.0), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
     {
     }

     Activity::Activity(int scheduler, int priority, Seconds period, RunnableInterface* r, const std::string& name )
         : ActivityInterface(r), os::Thread(scheduler, priority, period, 0, name ),
           update_period(period), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
     {
         // We pass the requested period to the constructor to not confuse users with log messages.
         // Then we clear it immediately again in order to force the Thread implementation to
         // non periodic:
         Thread::setPeriod(0,0);
     }

     Activity::Activity(int scheduler, int priority, Seconds period, unsigned cpu_affinity, RunnableInterface* r, const std::string& name )
     : ActivityInterface(r), os::Thread(scheduler, priority, period, cpu_affinity, name ),
       update_period(period), mtimeout(false), mstopRequested(false), mwaitpolicy(ORO_WAIT_ABS)
     {
         // We pass the requested period to the constructor to not confuse users with log messages.
         // Then we clear it immediately again in order to force the Thread implementation to
         // non periodic:
         Thread::setPeriod(0,0);
     }

    Activity::~Activity()
    {
        stop();

        // We need to join the activity's thread before destruction as the thread function might still
        // access member variables. Activity::stop() does not guarantuee to stop the underlying thread.
        terminate();
    }

    os::ThreadInterface* Activity::thread() {
        return this;
    }

    bool Activity::initialize() {
        if ( runner )
            return runner->initialize();
        return true;
    }

    void Activity::step() {
        if ( runner )
            runner->step();
    }

    void Activity::work(base::RunnableInterface::WorkReason reason) {
        if ( runner )
            runner->work(reason);
    }

    bool Activity::setPeriod( Seconds s ) {
        if (s < 0.0)
            return false;
        update_period = s;
        // we need to trigger internally to get the period started
        trigger();
        return true;
    }

    Seconds Activity::getPeriod() const {
        return update_period;
    }

    bool Activity::trigger() {
        if ( ! Thread::isActive() )
            return false;
        //a trigger is always allowed when active
        msg_cond.broadcast();
        Thread::start();
        return true;
    }

    bool Activity::execute() {
        // only allowed in slaves
        return false;
    }

    bool Activity::timeout() {
        // a user-timeout is only allowed for non-periodics
        if ( update_period > 0) {
            return false;
        }
        mtimeout = true;
        msg_cond.broadcast();
        Thread::start();
        return true;
    }

    void Activity::loop() {
        nsecs wakeup = 0;
        int overruns = 0;
        while ( true ) {
            // since update_period may be changed at any time, we need to recheck it each time:
            if ( update_period > 0.0) {
                // initialize first wakeup time if we are periodic.
                if ( wakeup == 0 ) {
                    wakeup = os::TimeService::Instance()->getNSecs() + Seconds_to_nsecs(update_period);
                }
            } else {
                // only clear if update_period <= 0.0 :
                wakeup = 0;
            }

            // periodic: we flag mtimeout below; non-periodic: we flag mtimeout in timeout()
            if (mtimeout) {
                // was a timeout() call, or internally generated after wakeup
                mtimeout = false;
                this->step();
                this->work(base::RunnableInterface::TimeOut);
            } else {
                // was a trigger() call
                if ( update_period > 0 ) {
                    this->step();
                    this->work(base::RunnableInterface::Trigger);
                } else {
                    if (runner) {
                        runner->loop();
                        runner->work(base::RunnableInterface::Trigger);
                    } else {
                        this->step();
                        this->work(base::RunnableInterface::Trigger);
                    }
                }
                // if a timeout() was done during work(), we will re-enter
                // loop() due to the Thread::start().
            }
            // next, sleep/wait
            os::MutexLock lock(msg_lock);
            if ( wakeup == 0 ) {
                // non periodic, default behavior:
                return;
            } else {
                // If periodic, sleep until wakeup time or a message comes in.
                // when wakeup time passed, wait_until will return false and we recalculate wakeup + update_period
                bool time_elapsed = ! msg_cond.wait_until(msg_lock,wakeup);

                if (time_elapsed) {
                    nsecs now = os::TimeService::Instance()->getNSecs();

                    // calculate next wakeup point
                    nsecs nsperiod = Seconds_to_nsecs(update_period);
                    wakeup = wakeup + nsperiod;

                    // detect overruns
                    if ( wakeup < now )
                    {
                        ++overruns;
                        if (overruns == maxOverRun)
                            break; // break while(true)
                    }
                    else if (overruns != 0) {
                        --overruns;
                    }

                    // ORO_WAIT_REL: reset next wakeup time to now (before step) + period
                    if ( mwaitpolicy == ORO_WAIT_REL ) {
                        wakeup = now + nsperiod;
                    }
                    mtimeout = true;
                }
            }
            if (mstopRequested) {
                mstopRequested = false; // guarded by Mutex lock
                return;
            }
        }
        if (overruns == maxOverRun)
        {
            this->emergencyStop();
            log(Critical) << rtos_task_get_name(this->getTask())
                    << " got too many periodic overruns in step() ("
                    << overruns << " times), stopped Thread !"
                    << endlog();
            log(Critical) << " See Thread::setMaxOverrun() for info."
                    << endlog();
        }
    }

    bool Activity::breakLoop() {
        if ( runner )
            return runner->breakLoop();
        return false;
    }

    void Activity::finalize() {
        if ( runner )
            runner->finalize();
    }


    bool Activity::start() {
        mstopRequested = false;
        return Thread::start();
    }

    bool Activity::stop()
    {
        if (!active)
            return false;

        running = false;

        // first of all, exit loop() if possible:
        {
            os::MutexLock lock(msg_lock); // protects stopRequested
            mstopRequested = true;
            msg_cond.broadcast();
        }

        if (update_period == 0)
        {
            if ( inloop ) {
                if ( !this->breakLoop() ) {
                    log(Warning) << "Failed to stop thread " << this->getName() << ": breakLoop() returned false."<<endlog();
                    running = true;
                    return false;
                }
                // breakLoop was ok, wait for loop() to return.
            }
            MutexTimedLock lock(breaker, getStopTimeout());
            if ( !lock.isSuccessful() ) {
                log(Error) << "Failed to stop thread " << this->getName() << ": breakLoop() returned true, but loop() function did not return after "<<getStopTimeout() << " second(s)."<<endlog();
                running = true;
                return false;
            }
        } else {
            //
            MutexTimedLock lock(breaker, getStopTimeout() );
            if ( lock.isSuccessful() ) {
                // drop out of periodic mode.
                rtos_task_make_periodic(&rtos_task, 0);
            } else {
                log(Error) << "Failed to stop thread " << this->getName() << ": step() function did not return after "<< getStopTimeout() <<" second(s)."<<endlog();
                running = true;
                return false;
            }
        }

        this->finalize();
        active = false;
        return true;
    }

    bool Activity::isRunning() const {
        return Thread::isRunning();
    }

    bool Activity::isActive() const {
        return Thread::isActive();
    }

    bool Activity::isPeriodic() const {
        return Thread::isPeriodic() || (update_period != 0.0);
    }

    unsigned Activity::getCpuAffinity() const
    {
        return Thread::getCpuAffinity();
    }

    bool Activity::setCpuAffinity(unsigned cpu)
    {
        return Thread::setCpuAffinity(cpu);
    }

    void Activity::setWaitPeriodPolicy(int p)
    {
        mwaitpolicy = p;
    }

}
