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
#include "corelib/PeriodicTask.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Logger.hpp"
#include "corelib/TaskExecution.hpp"

#include <cmath>

namespace ORO_CoreLib
{
    
    PeriodicTask::PeriodicTask(Seconds period, RunnableInterface* r )
       : runner(r), running(false), inError(false)
    {
        if (runner)
            runner->setTask(this);
        per_ns = nsecs( rint( period * secs_to_nsecs(1) ) );
    }

    PeriodicTask::PeriodicTask(secs s, nsecs ns, RunnableInterface* r )
       : runner(r), running(false), inError(false), per_ns( secs_to_nsecs(s) + ns)
    {
        if (runner)
            runner->setTask(this);
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
        if ( isRunning() ) return false;

        if (runner != 0)
            inError = !runner->initialize();
        else
            inError = !initialize();

        if ( !inError )
            running = taskAdd();
        else
            Logger::log() << Logger::Warning << "PeriodicTask with period "<<this->periodGet()<< "s failed to initialize() in thread " << this->thread()->taskNameGet() << Logger::endl;

        return running;
    }

    bool PeriodicTask::stop()
    {
        if ( !isRunning() ) return false;

        doStop();
        
        return true;
    }

    void PeriodicTask::doStop()
    {
        ORO_OS::MutexTryLock locker(stop_lock);
        if ( !locker.isSuccessful() )
            return; // stopping is in progress

        taskRemove();

        running = false;

        if (runner != 0)
            runner->finalize();
        else
            finalize();
    }

    bool PeriodicTask::isRunning() const
    {
        return running;
    }

    Seconds PeriodicTask::periodGet() const
    {
        return Seconds(per_ns) / (1000.0*1000.0*1000.0);
    }

    void PeriodicTask::doStep()
    {
        if ( running )
        {
            if (runner != 0)
                runner->step();
            else
                step();
        }
    }

}
