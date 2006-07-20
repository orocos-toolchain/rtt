/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  PeriodicActivity.cxx 

                        PeriodicActivity.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "rtt/Time.hpp"
#include "rtt/PeriodicActivity.hpp"
#include "rtt/os/MutexLock.hpp"
#include "rtt/Logger.hpp"
#include "rtt/TimerThread.hpp"
#include "rtt/EventProcessor.hpp"

// This define is used for creating and static_casting the Timer.
#include <pkgconf/corelib_activities.h>
#ifdef OROSEM_CORELIB_ACTIVITIES_TASKTIMER_TimerOneShot
#include "rtt/TimerOneShot.hpp"
#elif defined(OROSEM_CORELIB_ACTIVITIES_TASKTIMER_TimerSequencer)
#include "rtt/TimerSequencer.hpp"
#else
#include "rtt/TimerOneShot.hpp" // default
#endif

#include <cmath>

namespace RTT
{
    using namespace detail;
    
    PeriodicActivity::PeriodicActivity(int priority, Seconds period, RunnableInterface* r, bool private_event_processor )
        : runner(r), running(false), active(false),
          thread_( TimerThread::Instance(priority,period) ), eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        thread_->start();
        per_ns = Seconds_to_nsecs( period );
        this->init();
    }

    PeriodicActivity::PeriodicActivity(TimerThreadPtr thread, RunnableInterface* r, bool private_event_processor )
        : runner(r), running(false), active(false),
          thread_( thread ), eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        per_ns = Seconds_to_nsecs( thread->getPeriod() );
        this->init();
    }

    PeriodicActivity::PeriodicActivity(Seconds period, TimerThreadPtr thread, RunnableInterface* r, bool private_event_processor )
        : runner(r), running(false), active(false),
          thread_(thread), eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        per_ns = Seconds_to_nsecs( period );
        this->init();
    }

    PeriodicActivity::PeriodicActivity(secs s, nsecs ns, TimerThreadPtr thread, RunnableInterface* r, bool private_event_processor )
        : runner(r),
          running(false), active(false),
          per_ns( secs_to_nsecs(s) + ns),
          thread_(thread),
          eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        this->init();
    }

    PeriodicActivity::~PeriodicActivity()
    {
        stop();
        if (runner)
            runner->setActivity(0);
        delete eprocessor_;
    }

    void PeriodicActivity::init() {
        if (runner)
            runner->setActivity(this);

        TimerInterface* timer = thread_->timerGet( this->getPeriod() );
        if ( timer == 0 ) {
            timer = new TimerOneShot( per_ns );
//             Logger::log() << Logger::Debug << "Timer Created, period_ns: "<< per_ns <<" thread :"<< thread_->getName() <<Logger::endl;
            // The timer is owned by the thread !
            if ( thread_->timerAdd( timer ) == false ) {
                delete timer;
                timer = 0;
                Logger::log() << Logger::Critical << "PeriodicActivity with period "<<this->getPeriod()
                              << "s failed to schedule in thread " << thread_->getName()
                              << " which has period "<< thread_->getPeriod()<<"s."<< Logger::endl;
            }
        }
//         else
//             Logger::log() << Logger::Debug << "Existing timer, period_ns: "<< timer->getPeriod() <<" thread :"<< thread_->getName() <<Logger::endl;

        timer_ = timer;
    }

     
    bool PeriodicActivity::run( RunnableInterface* r )
    {
        if ( isRunning() )
            return false;
        if (runner)
            runner->setActivity(0);
        runner = r;
        if (runner)
            runner->setActivity(this);
        return true;
    }

    bool PeriodicActivity::start()
    {
        if ( !timer_ || isActive() || !thread_->isRunning() ) {
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

        if ( eprocessor_ ) {
            res = eprocessor_->initialize();
            if ( !res ) {
                this->stop();
                return false;
            }
        }
        running = true;
        return true;
    }

    bool PeriodicActivity::stop()
    {
        if ( !isActive() ) return false;

        // since removeActivity synchronises, we do not need to mutex-lock
        // stop()
        if ( timer_->removeActivity( this ) ) {
            running = false;
            if ( eprocessor_ )
                eprocessor_->finalize();
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
        return nsecs_to_Seconds( per_ns );
    }

    bool PeriodicActivity::initialize() { 
        if (runner != 0)
            return runner->initialize();
        else
            return true;
    }

    void PeriodicActivity::doStep()
    {
        this->step();
        if ( eprocessor_ )
            eprocessor_->step();
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

    void PeriodicActivity::finalize() {
        if (runner != 0)
            runner->finalize();
    }

    OS::ThreadInterface* PeriodicActivity::thread() { return thread_.get(); }

    EventProcessor* PeriodicActivity::getEventProcessor() const { return eprocessor_ == 0 ? thread_->getEventProcessor() : eprocessor_; }

    bool PeriodicActivity::isPeriodic() const {
        return true;
    }
}
