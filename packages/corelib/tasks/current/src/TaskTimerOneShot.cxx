/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TaskTimerOneShot.cxx 

                        TaskTimerOneShot.cxx -  description
                           -------------------
    begin                : Fri February 11 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "corelib/TaskTimerOneShot.hpp"
#include "corelib/PeriodicTask.hpp"
#include <os/MutexLock.hpp>
#include <algorithm>
#include "corelib/Logger.hpp"
#include <assert.h>
namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;
    using std::find;

    TaskTimerOneShot::TaskTimerOneShot( nsecs task_period ) 
        : trig_per( task_period), task_per( task_period ), turn(1), cleanup(false)
        {
            tasks.reserve(TaskTimerInterface::MAX_TASKS);
        }

    TaskTimerOneShot::~TaskTimerOneShot()
    {}

    bool TaskTimerOneShot::addTask( PeriodicTask* t ) {
        MutexLock lock(mutex);
        if ( tasks.size() == TaskTimerInterface::MAX_TASKS ) {
//             Logger::log() << Logger:: << "TaskTimerOneShot : tasks queue full, failed to add Task : "<< t << Logger::endl;
            return false;
        }
//         if ( find(tasks.begin(), tasks.end(), t) != tasks.end() ) {
//                 Logger::log() << Logger::Fatal << "TaskTimerOneShot : double add "<< t << Logger::endl;
//                 return false;
//         }
        tasks.push_back( t );
//         Logger::log() << Logger::Debug << "TaskTimerOneShot : successfully started Task : "<< t  << Logger::endl;
        return true;
    }

    bool TaskTimerOneShot::removeTask( PeriodicTask* t ) {
        MutexLock lock(mutex);
        TaskList::iterator it = find(tasks.begin(), tasks.end(), t);
        if ( it != tasks.end() ) {
            *it = 0; // clear task away
            cleanup = true;
//             Logger::log() << Logger::Debug << "TaskTimerOneShot : successfully stopped Task : "<< t  << Logger::endl;
//             if ( find(tasks.begin(), tasks.end(), t) != tasks.end() ) {
//                 Logger::log() << Logger::Fatal << "TaskTimerOneShot : removeTask failed to remove "<< t << Logger::endl;
//                 for ( it = tasks.begin(); it != tasks.end(); ++it)
//                     Logger::log() << Logger::Error << *it << Logger::endl;
//                 assert(false);
//             }
            return true;
        }
//         Logger::log() << Logger::Debug << "TaskTimerOneShot : failed to stop Task : "<< t->getPeriod() << Logger::endl;
        return false;
    }

    void TaskTimerOneShot::tick() {
        MutexLock lock(mutex);
        
        if ( turn == 1 )
            // The size of the tasks vector does not change during add/remove, thus
            // t_iter is never invalidated.
            for( TaskList::iterator t_iter = tasks.begin(); t_iter != tasks.end(); ++t_iter) 
                if ( *t_iter )
                    (*t_iter)->step();
                        
        if ( turn * trig_per == task_per )
            turn = 1;
        else
            ++turn;
        if ( cleanup )
            this->reorderList();
    }

    void TaskTimerOneShot::reorderList() {
        // reorder the list to remove clear'ed tasks
        TaskList::iterator begin = tasks.begin();
        // first zero :
        PeriodicTask* nullTask = 0;
        TaskList::iterator it = tasks.begin();
        // Check the precondition to enter this loop : 
        // "At least one nullTask in tasks"
//         if ( find( tasks.begin(), tasks.end(), nullTask) == tasks.end() ) {
//             Logger::log() << Logger::Error << "TaskTimerOneShot: no null found at cleanup begin." << Logger::endl;
//             return; // nothing to do
//         }
//         Logger::log() << "Before:"<< Logger::nl;
//         for ( it = tasks.begin(); it != tasks.end(); ++it)
//             Logger::log() << Logger::Error << *it << Logger::nl;
        it = find( tasks.begin(), tasks.end(), nullTask); // First zero
        begin = it+1;
        while ( it != tasks.end() ) {
            // Look for first non-zero after 'it' :
            while ( begin != tasks.end() &&  *begin == 0  )
                ++begin;
            if ( begin == tasks.end() )  { // if no task found after zero :
//                 Logger::log() << Logger::Error << "beginBefore resize :"<< tasks.size() << Logger::endl;
                tasks.resize( it - tasks.begin() ); // cut out the items after 'it'
//                 Logger::log() << Logger::Error << "beginAfter resize :"<< tasks.size() << Logger::endl;
                break; // This is our exit condition !
            }
            // first zero after begin :
            TaskList::iterator end = find ( begin, tasks.end(), nullTask);
            // if end == tasks.end() ==> next while will copy all
            // if end != tasks.end() ==> next while will copy first zero's
            while ( begin != end ) {
                *it = *begin; // copy operation
                ++begin;
                ++it; // go to next slot to inspect.
            }
            
//             if ( begin == tasks.end() )  { // if all is copied
// //                 Logger::log() << Logger::Error << "endBefore resize :"<< tasks.size() << Logger::endl;
//                 tasks.resize( it - tasks.begin() ); // cut out the items after 'it'
// //                 Logger::log() << Logger::Error << "endAfter resize :"<< tasks.size() << Logger::endl;
//                 break; // This is our exit condition !
//             }
        }

//         if ( find( tasks.begin(), tasks.end(), nullTask) != tasks.end() ) {
//             Logger::log() << Logger::Error << "TaskTimerOneShot: null found at cleanup end." << Logger::endl;
//             for ( it = tasks.begin(); it != tasks.end(); ++it)
//                 Logger::log() << Logger::Error << *it << Logger::endl;
//         }
//         Logger::log() << "After:"<< Logger::nl;
//         for ( it = tasks.begin(); it != tasks.end(); ++it)
//             Logger::log() << Logger::Error << *it << Logger::endl;
//         TaskList::iterator it2 = tasks.begin();
//         for ( it2= tasks.begin(); it2 != tasks.end() && it2+1 != tasks.end() ; ++it2)
//             for ( it = it2 + 1; it != tasks.end(); ++it)
//                 if ( *it == *it2 ) {
//                     for ( it = tasks.begin(); it != tasks.end(); ++it)
//                         Logger::log() << Logger::Error << *it << Logger::endl;
//                     assert( false);
//                 }
//             Logger::log() << Logger::Error << *it << Logger::endl;

        //assert( nrtasks == tasks.size() );
        cleanup = false;
    }


}
