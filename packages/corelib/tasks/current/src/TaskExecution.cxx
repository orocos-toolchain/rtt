/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TaskExecution.cpp 

                       TaskExecution.cpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
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

#include "corelib/TaskExecution.hpp"
#include "corelib/RealTimeTask.hpp"
#include "corelib/TaskTimer.hpp"
#include "corelib/Time.hpp"
#include "corelib/Logger.hpp"
#include <pkgconf/corelib_tasks.h>

namespace ORO_CoreLib
{

    TaskExecution::TaskExecution(int priority, const std::string& name, double periodicity)
        : TaskThreadInterface( priority, name, periodicity)
    {
    }

    TaskExecution::~TaskExecution()
    {
        // make sure the thread does not run when we start deleting clocks...
        this->stop();
#if OROSEM_CORELIB_TASKS_DYNAMIC_REG
        // cleanup tasks.
        for (std::list<TimerItem>::iterator itl= clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->owner )
                delete ( itl->timer );
#endif
    }

    void TaskExecution::step()
    {
        std::list<TimerItem>::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->timer )  // We do not remove our timers at runtime.
                itl->timer->tick();
        // Execute event completion handlers :
        EventProcessor::step();
    }        

    void TaskExecution::timerAdd( TaskTimer* ei)
    {
        doTimerAdd( ei, false);
    }

    void TaskExecution::doTimerAdd( TaskTimer* ev, bool myTimer)
    {
        secs s;
        nsecs ns;
        periodGet(s,ns);
        ev->triggerPeriodSet( secs_to_nsecs(s) + ns );
        MutexLock locker(lock);
        clocks.push_back( TimerItem(ev, myTimer) );
    }

    void TaskExecution::timerRemove( TaskTimer* ev)
    {
        std::list<TimerItem>::iterator itl;
        MutexLock locker(lock);
        itl = std::find_if(clocks.begin(), clocks.end(), bind2nd( TimerItem::Locator(), ev ) );
        if (itl != clocks.end() )
            *itl = 0;
        //            clocks.erase(itl);
    }

    bool TaskExecution::taskAdd( RealTimeTask* t, const nsecs n )
    {
        { 
            // scoped mutexlock with the for loop
            std::list<TimerItem>::iterator itl;
            MutexLock locker(lock);
            for (itl = clocks.begin(); itl != clocks.end(); ++itl)
                if ( itl->timer && itl->timer->periodGet() == n )
                    {
                        return itl->timer->addTask( t );
                    }
        }
#if OROSEM_CORELIB_TASKS_DYNAMIC_REG
        Logger::log() << Logger::Debug << "Added new TaskTimer with period "<< Seconds(n)/NSECS_IN_SECS <<"s";
        Logger::log() << Logger::Debug << " in thread " << this->taskNameGet() << Logger::endl;
        /**
         * Create the timer when not existing.
         */
        TaskTimer* ep = new TaskTimer( Seconds(n)/NSECS_IN_SECS );
        ep->addTask(t);

        doTimerAdd(ep, true);  // my timer.

        return true;
#else
        return false;
#endif
    }

    void TaskExecution::taskRemove( RealTimeTask* t )
    {
        // this is doing it the hard way.
        std::list<TimerItem>::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->timer )
                itl->timer->removeTask(t);
    }
        
}
