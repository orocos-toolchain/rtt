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

#pragma implementation
#include "corelib/Time.hpp"
#include "corelib/PeriodicTask.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Logger.hpp"
#include "corelib/TaskExecution.hpp"

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
    
    PeriodicTask::PeriodicTask(Seconds period, TaskExecution* thread, RunnableInterface* r )
        : runner(r), running(false), inError(false), _thread(thread)
    {
        per_ns = Seconds_to_nsecs( period );
        this->init();
    }

    void PeriodicTask::init() {
        if (runner)
            runner->setTask(this);

        TaskTimerInterface* timer = _thread->timerGet( this->getPeriod() );
        if ( timer == 0 ) {
            timer = new TaskTimerOneShot( per_ns );
//             Logger::log() << Logger::Debug << "Timer Created, period_ns: "<< per_ns <<" thread :"<< _thread->taskNameGet() <<Logger::endl;
            // The timer is owned by the thread !
            if ( _thread->timerAdd( timer ) == false ) {
                delete timer;
                timer = 0;
            }
        }
//         else
//             Logger::log() << Logger::Debug << "Existing timer, period_ns: "<< timer->getPeriod() <<" thread :"<< _thread->taskNameGet() <<Logger::endl;

        _timer = timer;
    }

    PeriodicTask::PeriodicTask(secs s, nsecs ns, TaskExecution* thread, RunnableInterface* r )
        : runner(r), running(false), inError(false), per_ns( secs_to_nsecs(s) + ns), _thread(thread)
    {
        this->init();
    }

    PeriodicTask::~PeriodicTask()
    {
        stop();
        if (runner)
            runner->setTask(0);
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
        if ( isRunning() || !_thread->isRunning() ) return false;

        inError = !this->initialize();

        if ( !inError && _timer )
            running = _timer->addTask( this );
//         else
//             Logger::log() << Logger::Warning << "PeriodicTask with period "<<this->getPeriod()<< "s failed to initialize() in thread " << this->thread()->taskNameGet() << Logger::endl;

        return running;
    }

    bool PeriodicTask::stop()
    {
        if ( !isRunning() ) return false;

        return doStop();
    }

    bool PeriodicTask::doStop()
    {
        ORO_OS::MutexTryLock locker(stop_lock);
        if ( !locker.isSuccessful() )
            return true; // stopping is in progress

        if ( _timer->removeTask( this ) ) {
            running = false;
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
        
    void PeriodicTask::step()
    {
        // overload this method to avoid running runner.
        if (runner != 0)
            runner->step();
    }

    void PeriodicTask::finalize() {
        if (runner != 0)
            runner->finalize();
    }

    TaskThreadInterface* PeriodicTask::thread() const { return _thread; }

}
