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
#include "corelib/PeriodicTask.hpp"
#include "corelib/TaskTimerInterface.hpp"
#include "corelib/Time.hpp"
#include "corelib/Logger.hpp"
#include <pkgconf/corelib_tasks.h>

namespace ORO_CoreLib
{

    TaskExecution::TaskExecution(int priority, const std::string& name, double periodicity)
        : TaskThreadInterface( priority, name, periodicity)
    {
//         clocks.reserve( MAX_TASK_TIMERS );
    }

    TaskExecution::~TaskExecution()
    {
        // make sure the thread does not run when we start deleting clocks...
        this->stop();
        // cleanup all timers :
        TimerList::iterator itl;
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            delete *itl; 
    }

    void TaskExecution::step()
    {
        TimerList::iterator itl;
        {
            // This critical section can be 'removed' if we
            // clocks.reserve() enough elements.
            // now it is needed as the thread is running while timerAdd() is called.
            MutexLock locker(lock);
            for (itl = clocks.begin(); itl != clocks.end(); ++itl)
                (*itl)->tick();
        }
        // Execute event completion handlers :
        EventProcessor::step();
    }        

    TaskTimerInterface* TaskExecution::timerGet( Seconds period ) const {
        for (TimerList::const_iterator it = clocks.begin(); it != clocks.end(); ++it)
            if ( (*it)->periodGet() == Seconds_to_nsecs(period) )
                return *it;
        //Logger::log() << Logger::Debug << "Failed to find Timer in "<< this->taskNameGet()<<" : "<< period << Logger::endl;

        return 0;
    }

    bool TaskExecution::timerAdd( TaskTimerInterface* t)
    {
//         if ( clocks.size() == MAX_TASK_TIMERS ) {
//                 Logger::log() << Logger::Critical << "TaskExecution: Could not add Timer with period_ns: "<< t->periodGet() <<" in thread :"<< this->taskNameGet() <<Logger::nl;
//                 Logger::log() << Logger::Critical << "  Advice : increase MAX_TASK_TIMERS ( current value : " << TaskExecution::MAX_TASK_TIMERS <<" )." << Logger::endl;
//             return false;
        secs s;
        nsecs ns;
        periodGet(s,ns);
        t->triggerSet( secs_to_nsecs(s) + ns );
        MutexLock locker(lock);
        clocks.push_back( t );
        return true;
    }

}
