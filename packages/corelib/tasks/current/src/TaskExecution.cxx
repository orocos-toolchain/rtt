/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TaskExecution.cpp 

                       TaskExecution.cpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

   ***************************************************************************
   *                                                                         *
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   *                                                                         *
   ***************************************************************************/


#include "corelib/TaskExecution.hpp"
#include "corelib/Time.hpp"
#include <pkgconf/corelib_tasks.h>

namespace ORO_CoreLib
{

    TaskExecution::TaskExecution(int priority, const std::string& name, double periodicity)
        : PeriodicThread( priority, name, periodicity)
    {
    }

    TaskExecution::~TaskExecution()
    {
        // make sure the thread does not run when we start deleting clocks...
        this->stop();
#if OROSEM_CORELIB_TASKS_DYNAMIC_REG
        // cleanup events.
        for (std::list<EventItem>::iterator itl= clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->owner )
                delete ( itl->event );
#endif
    }

    void TaskExecution::step()
    {
        std::list<EventItem>::iterator itl;
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->event )
                itl->event->fire();
        // XXX Caveat : if the event is removed between the if() and the fire()
        // a null pointer will be called. but i can not lock since start()'ing
        // another task would then lead to selflock. The correct solution is
        // using a recursive mutex. It is okay to remove the event which is
        // calling your handler, since the iterator is not removed.
        // XXX 2 : at the end of step(), remove all iterators pointing to zero.
        //         
    }        

    void TaskExecution::eventAdd( EventPeriodic* ei)
    {
        doEventAdd( ei, false);
    }

    void TaskExecution::doEventAdd( EventPeriodic* ev, bool myEvent)
    {
        secs s;
        nsecs ns;
        periodGet(s,ns);
        ev->triggerPeriodSet( secs_to_nsecs(s) + ns );
        MutexLock locker(lock);
        clocks.push_back( EventItem(ev, myEvent) );
    }

    void TaskExecution::eventRemove( EventPeriodic* ev)
    {
        std::list<EventItem>::iterator itl;
        MutexLock locker(lock);
        itl = std::find_if(clocks.begin(), clocks.end(), bind2nd( EventItem::Locator(), ev ) );
        if (itl != clocks.end() )
            *itl = 0;
        //            clocks.erase(itl);
    }

    bool TaskExecution::taskAdd( EventListenerInterface* t, const nsecs n )
    {
        { 
            // scoped mutexlock with the for loop
            std::list<EventItem>::iterator itl;
            MutexLock locker(lock);
            for (itl = clocks.begin(); itl != clocks.end(); ++itl)
                if ( itl->event && itl->event->periodGet() == n )
                    {
                        itl->event->addHandler(t, Completer::None );
                        return true;
                    }
        }
#if OROSEM_CORELIB_TASKS_DYNAMIC_REG
        /**
         * Create the event when not existing.
         */
        EventPeriodic* ep = new EventPeriodic( Seconds(n)/NSECS_IN_SECS );
        ep->addHandler(t, Completer::None);

        doEventAdd(ep, true);  // my event.

        return true;
#else
        return false;
#endif
    }

    void TaskExecution::taskRemove( EventListenerInterface* t )
    {
        // this is doing it the hard way.
        std::list<EventItem>::iterator itl;
        MutexLock locker(lock);
        for (itl = clocks.begin(); itl != clocks.end(); ++itl)
            if ( itl->event )
                itl->event->removeHandler(t,0);
    }
        
}
