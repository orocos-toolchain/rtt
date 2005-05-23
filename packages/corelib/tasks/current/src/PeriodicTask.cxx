/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  PeriodicTask.cxx 

                        PeriodicTask.cxx -  description
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
#include "corelib/Time.hpp"
#include "corelib/PeriodicTask.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Logger.hpp"
#include "corelib/TimerThread.hpp"

// This define is used for creating and static_casting the Timer.
#include <pkgconf/corelib_tasks.h>
#ifdef OROSEM_CORELIB_TASKS_TASKTIMER_TaskTimerOneShot
#include "corelib/TaskTimerOneShot.hpp"
#elif defined(OROSEM_CORELIB_TASKS_TASKTIMER_TaskTimerSequencer)
#include "corelib/TaskTimerSequencer.hpp"
#else
#include "corelib/TaskTimerOneShot.hpp" // default
#endif

#include <cmath>

namespace ORO_CoreLib
{
    using namespace detail;
    
    PeriodicTask::PeriodicTask(Seconds period, TimerThread* thread, RunnableInterface* r, bool private_event_processor )
        : runner(r), running(false), inError(false), thread_(thread), eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        per_ns = Seconds_to_nsecs( period );
        this->init();
    }

    void PeriodicTask::init() {
        if (runner)
            runner->setTask(this);

        TaskTimerInterface* timer = thread_->timerGet( this->getPeriod() );
        if ( timer == 0 ) {
            timer = new TaskTimerOneShot( per_ns );
//             Logger::log() << Logger::Debug << "Timer Created, period_ns: "<< per_ns <<" thread :"<< thread_->getName() <<Logger::endl;
            // The timer is owned by the thread !
            if ( thread_->timerAdd( timer ) == false ) {
                delete timer;
                timer = 0;
                Logger::log() << Logger::Critical << "PeriodicTask with period "<<this->getPeriod()
                              << "s failed to schedule in thread " << thread_->getName()
                              << " which has period "<< thread_->getPeriod()<<"s."<< Logger::endl;
            }
        }
//         else
//             Logger::log() << Logger::Debug << "Existing timer, period_ns: "<< timer->getPeriod() <<" thread :"<< thread_->getName() <<Logger::endl;

        timer_ = timer;
    }

    PeriodicTask::PeriodicTask(secs s, nsecs ns, TimerThread* thread, RunnableInterface* r, bool private_event_processor )
        : runner(r),
          running(false), inError(false),
          per_ns( secs_to_nsecs(s) + ns),
          thread_(thread),
          eprocessor_( private_event_processor ? new EventProcessor() : 0)
    {
        this->init();
    }

    PeriodicTask::~PeriodicTask()
    {
        stop();
        if (runner)
            runner->setTask(0);
        delete eprocessor_;
    }
     
    bool PeriodicTask::run( RunnableInterface* r )
    {
        if ( isRunning() )
            return false;
        if (runner)
            runner->setTask(0);
        runner = r;
        if (runner)
            runner->setTask(this);
        return true;
    }

    bool PeriodicTask::start()
    {
        if ( isRunning() || !thread_->isRunning() ) return false;

        inError = !this->initialize();

        if ( !inError && timer_ )
            running = timer_->addTask( this );
//         else
//             Logger::log() << Logger::Warning << "PeriodicTask with period "<<this->getPeriod()<< "s failed to initialize() in thread " << this->thread()->taskNameGet() << Logger::endl;

        if ( eprocessor_ )
            eprocessor_->initialize();
        return running;
    }

    bool PeriodicTask::stop()
    {
        if ( !isRunning() ) return false;

        // since removeTask synchronises, we do not need to mutex-lock
        // stop()
        if ( timer_->removeTask( this ) ) {
            running = false;
            if ( eprocessor_ )
                eprocessor_->finalize();
            this->finalize();
            return true;
        }
        return false;
    }

    bool PeriodicTask::isRunning() const
    {
        return running;
    }

    Seconds PeriodicTask::getPeriod() const
    {
        return nsecs_to_Seconds( per_ns );
    }

    bool PeriodicTask::initialize() { 
        if (runner != 0)
            return runner->initialize();
        else
            return true;
    }

    void PeriodicTask::doStep()
    {
        this->step();
        if ( eprocessor_ )
            eprocessor_->step();
    }
        
    void PeriodicTask::step()
    {
        // override this method to avoid running runner.
        if (runner != 0)
            runner->step();
    }

    void PeriodicTask::finalize() {
        if (runner != 0)
            runner->finalize();
    }

    ORO_OS::ThreadInterface* PeriodicTask::thread() { return thread_; }

    EventProcessor* PeriodicTask::getEventProcessor() const { return eprocessor_ == 0 ? thread_ : eprocessor_; }

}
