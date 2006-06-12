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
#include "corelib/PeriodicActivity.hpp"
#include "corelib/TimerInterface.hpp"
#include "corelib/EventProcessor.hpp"

#include "pkgconf/corelib_activities.h"

// this timer is the only correct, synchronising one
// with respect to step() and finalize()
#define OROSEM_ONESHOT_TIMER
#ifdef OROSEM_ONESHOT_TIMER
#include "corelib/TimerOneShot.hpp"
#else
#include "corelib/TimerLockFree.hpp"
#endif
#include "corelib/Time.hpp"
#include "corelib/Logger.hpp"
#include <algorithm>
#include <os/MutexLock.hpp>

namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;
    using namespace detail;
    using namespace std;

    TimerThread::TimerThreadList TimerThread::TimerThreads;

    TimerThreadPtr TimerThread::Instance(int pri, double per)
    {
        // Since the period is stored as nsecs, we convert per to NS in order
        // to get a match.
        TimerThreadList::iterator it = TimerThreads.begin();
        while ( it != TimerThreads.end() ) {
            TimerThreadPtr tptr = it->lock();
            // detect old pointer.
            if ( !tptr ) {
                TimerThreads.erase(it);
                it = TimerThreads.begin();
                continue;
            }
            if ( tptr->getPriority() == pri && tptr->getPeriodNS() == Seconds_to_nsecs(per) ) {
                return tptr;
            }
            ++it;
        }
        TimerThreadPtr ret( new TimerThread(pri, "TimerThreadInstance", per) );
        TimerThreads.push_back( ret );
        return ret;
    }

    TimerThread::TimerThread(int priority, const std::string& name, double periodicity)
        : PeriodicThread( priority, name, periodicity), eproc( new EventProcessor() )
    {
        // create one default timer for the tasks with this periodicity.
        TimerInterface* timer = 
#ifdef OROSEM_ONESHOT_TIMER
            new TimerOneShot( Seconds_to_nsecs( periodicity ) );
#else
            new TimerLockFree( Seconds_to_nsecs( periodicity ) );
#endif
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
        delete eproc;
    }

    bool TimerThread::initialize()
    {
        TimerList::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            (*itl)->start();
        eproc->initialize();
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
        eproc->step();
    }        

    void TimerThread::finalize()
    {
        TimerList::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            (*itl)->stop();
        eproc->finalize();
    }

    TimerInterface* TimerThread::timerGet( Seconds period ) const {
        MutexLock locker(lock);
        for (TimerList::const_iterator it = clocks.begin(); it != clocks.end(); ++it)
            if ( (*it)->getPeriod() == Seconds_to_nsecs(period) )
                return *it;
        //Logger::log() << Logger::Debug << "Failed to find Timer in "<< this->taskNameGet()<<" : "<< period << Logger::endl;

        return 0;
    }

    bool TimerThread::timerAdd( TimerInterface* t)
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
