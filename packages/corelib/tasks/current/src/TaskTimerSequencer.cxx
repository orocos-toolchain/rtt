#include "corelib/TaskTimerSequencer.hpp"
#include <os/MutexLock.hpp>
#include <cmath>
#include <algorithm>
#include "corelib/PeriodicTask.hpp"


#include <iostream>
using namespace std;

namespace ORO_CoreLib
{
    using std::find_if;

    TaskTimerSequencer::TaskTimerSequencer( Seconds average_period, Seconds trigger_period )
        : avPeriod( nsecs(rint(average_period * NSECS_IN_SECS)) ), 
          trigPeriod( nsecs(rint(trigger_period * NSECS_IN_SECS)) ),
          total_added(0), turn(0), reset_f_iter(false), in_tick(false)
    {
        if (trigger_period == 0 )
            trigPeriod = nsecs(rint(average_period * NSECS_IN_SECS));
        // resize list if needed
        triggerSet( trigPeriod );

        // list contains MAX_TASKS items.
        f_iter = sortedList.begin();
    }

    TaskTimerSequencer::TaskTimerSequencer( unsigned int divider)
        : avPeriod(divider), trigPeriod(1),
          total_added(0), turn(0),
          reset_f_iter(false), in_tick(false)
    {
        // resize list if needed
        triggerSet(trigPeriod);

        f_iter = sortedList.begin();
    }
        
    void TaskTimerSequencer::tick()
    {
        ORO_OS::MutexLock locker(mut);
        //cout <<"tick"<<endl;
        in_tick = true;
        if (reset_f_iter)
            f_iter = sortedList.begin();
        reset_f_iter = false;
        //cout <<trigPeriod<<avPeriod<<f_iter->task<< f_iter->turn<<endl; 
        while (  f_iter != sortedList.end() && f_iter->turn == turn )// in case all in one turn.
            {
                if (f_iter->task != 0) 
                    {
                        f_iter->task->step();
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
        in_tick = false;
    }

    bool TaskTimerSequencer::taskAdd( PeriodicTask* task )
    {
        ORO_OS::MutexLock locker(mut);
        std::list<ListItem>::iterator itl;
        itl = find_if(sortedList.begin(), sortedList.end(), bind2nd( CMP(), 0) ); // find first zero
        if (itl == sortedList.end())
            return false;

        itl->priority = ++total_added;
        itl->task = task;

        // sort will invalidate the iterators pointing to sortedList.end()
        if ( f_iter == sortedList.end() )
            reset_f_iter = true;
        // sorting of a list DOES NOT invalidate iterators ! and is stable
        sortedList.sort( PrioritySort() );
        
        update();
        return true;
    }

    bool TaskTimerSequencer::taskRemove( PeriodicTask* task )
    {
        ORO_OS::MutexLock locker(mut);
        std::list<ListItem>::iterator itl;
        itl = find_if(sortedList.begin(), sortedList.end(), bind2nd( CMP(), task) );
        if (itl == sortedList.end())
            return false;
        // Detection of self removal (recursive mutex) :
        // move f_iter one back to previous node.
        // on return in tick(), f_iter will be incremented again
        // and point to the correct node. Exception : removal of
        // first node will happen through reset_f_iter.
        if ( in_tick && itl != sortedList.begin() && itl == f_iter ) {
            --f_iter;
        }

        // if the first node is (self-)removed and we are (about to) ticking()
        // it, also reset.
        if ( itl == sortedList.begin() && itl == f_iter )
            reset_f_iter = true;
            
        itl->task = 0;
        itl->priority = ~0; // yeah i know.
        
        // sort will invalidate the iterators pointing to sortedList.end()
        if ( f_iter == sortedList.end() )
            reset_f_iter = true;

        sortedList.sort( PrioritySort() );

        update();
        return true;
    }

    void TaskTimerSequencer::update()
    {
        // numbering of turns
        std::list<ListItem>::iterator itl = sortedList.begin();
        unsigned int t_nr = 0;
        while ( itl != sortedList.end() )
            {
                for (unsigned int i=0; i < MAX_TASKS ; ++i)
                    (itl++)->turn = t_nr;
                ++t_nr;
            }
    }

    nsecs TaskTimerSequencer::periodGet() const
    {
        return avPeriod;
    }

    nsecs TaskTimerSequencer::triggerGet() const
    {
        return trigPeriod;
    }

    void TaskTimerSequencer::triggerSet(nsecs n)
    {
        trigPeriod = n;

        relPeriod = nsecs(avPeriod)/nsecs(trigPeriod);
        if (relPeriod == 0) relPeriod = 1;

        sortedList.resize(MAX_TASKS * relPeriod);
        sortedList.sort( PrioritySort() );

        update();
    }
}
