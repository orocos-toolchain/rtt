/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  EventPeriodic.cxx 

                        EventPeriodic.cxx -  description
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
#include "corelib/EventPeriodic.hpp"

#include "corelib/CompletionProcessor.hpp"
#include <os/MutexLock.hpp>
#include <cmath>

namespace ORO_CoreLib
{
    EventPeriodic::EventPeriodic( Seconds average_period, Seconds trigger_period )
        : avPeriod( nsecs(rint(average_period * NSECS_IN_SECS)) ), 
          trigPeriod( nsecs(rint(trigger_period * NSECS_IN_SECS)) ),
          total_added(0), turn(0), completerHack( Completer::None ), reset_f_iter(false)
    {
        if (trigger_period == 0 )
            trigPeriod = nsecs(rint(average_period * NSECS_IN_SECS));
        // resize list if needed
        triggerPeriodSet( trigPeriod );

        f_iter = sortedList.begin();
        cp = CompletionProcessor::Instance();
    }

    EventPeriodic::EventPeriodic( unsigned int divider)
        : avPeriod(divider), trigPeriod(1), total_added(0), turn(0), completerHack(0), reset_f_iter(false)
    {
        // resize list if needed
        triggerPeriodSet(trigPeriod);

        f_iter = sortedList.begin();
        cp = CompletionProcessor::Instance();
    }
        
    void EventPeriodic::fire()
    {
        ORO_OS::MutexLock locker(mut);
        //cout <<"fire"<<endl;

        //cout <<trigPeriod<<avPeriod<<f_iter->eli<< f_iter->eci<< f_iter->turn<<endl; 
        while (  f_iter != sortedList.end() && f_iter->turn == turn )// in case all in one turn.
            {
                if (f_iter->eli != 0) 
                    {
                        completerHack = f_iter->eci;
#ifndef OROINT_OS_RECURSIVE_MUTEX
                        mut.unlock();
#endif
                        f_iter->eli->handleEvent(this);
#ifndef OROINT_OS_RECURSIVE_MUTEX
                        mut.lock();
#endif
                    }
                if (reset_f_iter) 
                    {
                        f_iter = sortedList.begin();
                        reset_f_iter = false;
                    }
                else
                    ++f_iter;
            }
        
        // at end of turn
        if ( ++turn == relPeriod )
            {
                turn = 0;
                f_iter = sortedList.begin();
            }
    }

    void EventPeriodic::complete()
    {
        // We might find a nicer way but it respects the contract.
        if (completerHack != 0 )
            cp->queue( completerHack );
    }

    void EventPeriodic::complete(EventListenerInterface* eli)
    {
        std::list<ListItem>::iterator itl;
        itl = find_if(sortedList.begin(), sortedList.end(), bind2nd( CMP(), eli) );
        if ( itl != sortedList.end() && itl->eci != 0 ) cp->queue( itl->eci );
    }

    void EventPeriodic::addHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        ORO_OS::MutexLock locker(mut);
        std::list<ListItem>::iterator itl;
        itl = find_if(sortedList.begin(), sortedList.end(), bind2nd( CMP(), 0) ); // find first zero
        if (itl == sortedList.end())
            return; // should return false ?

        itl->priority = ++total_added;
        itl->eci = eci;
        itl->eli = eli;
        // sorting of a list DOES NOT invalidate iterators ! and is stable
        sortedList.sort( PrioritySort() );
        
        update();
    }

    void EventPeriodic::removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        ORO_OS::MutexLock locker(mut);
        std::list<ListItem>::iterator itl;
        itl = find_if(sortedList.begin(), sortedList.end(), bind2nd( CMP(), eli) );
        if (itl == sortedList.end())
            return;
        // detection of self removal
        if ( itl != sortedList.begin() && itl == f_iter )
            --f_iter;

        // sort invalidated the iterators pointing to sortedList.end()
        if ( f_iter == sortedList.end() || ( itl == sortedList.begin() && itl == f_iter) )
            reset_f_iter = true;
            
        itl->eli = 0;
        itl->eci = 0;
        itl->priority = ~0; // yeah i know.
        
        sortedList.sort( PrioritySort() );

        update();
    }

    void EventPeriodic::update()
    {
        // numbering of turns
        std::list<ListItem>::iterator itl = sortedList.begin();
        unsigned int t_nr = 0;
        while ( itl != sortedList.end() )
            {
                for (unsigned int i=0; i < MAX_LISTENERS ; ++i)
                    (itl++)->turn = t_nr;
                ++t_nr;
            }
    }

    nsecs EventPeriodic::periodGet()
    {
        return avPeriod;
    }

    void EventPeriodic::triggerPeriodSet(nsecs n)
    {
        trigPeriod = n;

        relPeriod = nsecs(avPeriod)/nsecs(trigPeriod);
        if (relPeriod == 0) relPeriod = 1;

        sortedList.resize(MAX_LISTENERS * relPeriod);
        sortedList.sort( PrioritySort() );

        update();
    }
}
