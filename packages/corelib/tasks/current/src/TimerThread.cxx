/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TimerThread.cpp 

                       TimerThread.cpp -  description
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

#include "corelib/TimerThread.hpp"
#include "corelib/PeriodicTask.hpp"
#include "corelib/TaskTimerInterface.hpp"
#include "corelib/TaskTimerOneShot.hpp"
#include "corelib/Time.hpp"
#include "corelib/Logger.hpp"
#include <pkgconf/corelib_tasks.h>

namespace ORO_CoreLib
{

    TimerThread::TimerThread(int priority, const std::string& name, double periodicity)
        : TaskThreadInterface( priority, name, periodicity)
    {
        // create one default timer for the tasks with this periodicity.
        TaskTimerInterface* timer = new TaskTimerOneShot( Seconds_to_nsecs( periodicity ) );
        this->timerAdd( timer );
    }

    TimerThread::~TimerThread()
    {
        // make sure the thread does not run when we start deleting clocks...
        this->stop();
        // cleanup all timers :
        TimerList::iterator itl;
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            delete *itl; 
    }

    bool TimerThread::initialize()
    {
        TimerList::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            (*itl)->start();
        return true;
    }

    void TimerThread::step()
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

    void TimerThread::finalize()
    {
        TimerList::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            (*itl)->stop();
    }

    TaskTimerInterface* TimerThread::timerGet( Seconds period ) const {
        MutexLock locker(lock);
        for (TimerList::const_iterator it = clocks.begin(); it != clocks.end(); ++it)
            if ( (*it)->getPeriod() == Seconds_to_nsecs(period) )
                return *it;
        //Logger::log() << Logger::Debug << "Failed to find Timer in "<< this->taskNameGet()<<" : "<< period << Logger::endl;

        return 0;
    }

    bool TimerThread::timerAdd( TaskTimerInterface* t)
    {
        nsecs p = Seconds_to_nsecs( this->getPeriod() );
        // if period is too small or not a multiple :
        // we also detect t with period zero, 
        if ( t->getPeriod() < p || t->getPeriod() % p != 0  ) // comparison in nsecs
            return false; // can not use this timer.
        t->setTrigger( p );
        MutexLock locker(lock);
        clocks.push_back( t );
        return true;
    }

}
