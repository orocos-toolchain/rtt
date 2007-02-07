/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TimerOneShot.cxx 

                        TimerOneShot.cxx -  description
                           -------------------
    begin                : Fri February 11 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#include "TimerOneShot.hpp"
#include "PeriodicActivity.hpp"
#include <os/MutexLock.hpp>
#include <algorithm>
#include "Logger.hpp"
#include <assert.h>
namespace RTT
{
    using namespace detail;
    using OS::MutexLock;
    using std::find;

    TimerOneShot::TimerOneShot( nsecs task_period ) 
        : trig_per( task_period), task_per( task_period ), turn(1), cleanup(false)
        {
            tasks.reserve(TimerInterface::MAX_ACTIVITIES);
        }

    TimerOneShot::~TimerOneShot()
    {}

    bool TimerOneShot::addActivity( PeriodicActivity* t ) {
        MutexLock lock(mutex);
        if ( tasks.size() == TimerInterface::MAX_ACTIVITIES ) {
//             Logger::log() << Logger:: << "TimerOneShot : tasks queue full, failed to add Activity : "<< t << Logger::endl;
            return false;
        }
//         if ( find(tasks.begin(), tasks.end(), t) != tasks.end() ) {
//                 Logger::log() << Logger::Fatal << "TimerOneShot : double add "<< t << Logger::endl;
//                 return false;
//         }
        tasks.push_back( t );
//         Logger::log() << Logger::Debug << "TimerOneShot : successfully started Activity : "<< t  << Logger::endl;
        return true;
    }

    bool TimerOneShot::removeActivity( PeriodicActivity* t ) {
        MutexLock lock(mutex);
        ListActivity::iterator it = find(tasks.begin(), tasks.end(), t);
        if ( it != tasks.end() ) {
            *it = 0; // clear task away
            cleanup = true;
//             Logger::log() << Logger::Debug << "TimerOneShot : successfully stopped Activity : "<< t  << Logger::endl;
//             if ( find(tasks.begin(), tasks.end(), t) != tasks.end() ) {
//                 Logger::log() << Logger::Fatal << "TimerOneShot : removeActivity failed to remove "<< t << Logger::endl;
//                 for ( it = tasks.begin(); it != tasks.end(); ++it)
//                     Logger::log() << Logger::Error << *it << Logger::endl;
//                 assert(false);
//             }
            return true;
        }
//         Logger::log() << Logger::Debug << "TimerOneShot : failed to stop Activity : "<< t->getPeriod() << Logger::endl;
        return false;
    }

    void TimerOneShot::start() {
        MutexLock lock(mutex);
        turn = 1;
    }

    void TimerOneShot::stop() {
        MutexLock lock(mutex);
        
        for( ListActivity::iterator t_iter = tasks.begin(); t_iter != tasks.end(); ++t_iter) 
            if ( *t_iter )
                (*t_iter)->stop(); // stop() calls us back to removeActivity (recursive mutex).
        if ( cleanup )
            this->reorderList();
    }

    void TimerOneShot::tick() {
        MutexLock lock(mutex);
        
        if ( turn == 1 )
            // The size of the tasks vector does not change during add/remove, thus
            // t_iter is never invalidated.
            for( ListActivity::iterator t_iter = tasks.begin(); t_iter != tasks.end(); ++t_iter) 
                if ( *t_iter )
                    (*t_iter)->doStep();
                        
        if ( turn * trig_per == task_per )
            turn = 1;
        else
            ++turn;
        if ( cleanup )
            this->reorderList();
    }

    void TimerOneShot::reorderList() {
        // reorder the list to remove clear'ed tasks
        ListActivity::iterator begin = tasks.begin();
        // first zero :
        PeriodicActivity* nullActivity = 0;
        ListActivity::iterator it = tasks.begin();
        // Check the precondition to enter this loop : 
        // "At least one nullActivity in tasks"
//         if ( find( tasks.begin(), tasks.end(), nullActivity) == tasks.end() ) {
//             Logger::log() << Logger::Error << "TimerOneShot: no null found at cleanup begin." << Logger::endl;
//             return; // nothing to do
//         }
//         Logger::log() << "Before:"<< Logger::nl;
//         for ( it = tasks.begin(); it != tasks.end(); ++it)
//             Logger::log() << Logger::Error << *it << Logger::nl;
        it = find( tasks.begin(), tasks.end(), nullActivity); // First zero
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
            ListActivity::iterator end = find ( begin, tasks.end(), nullActivity);
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

//         if ( find( tasks.begin(), tasks.end(), nullActivity) != tasks.end() ) {
//             Logger::log() << Logger::Error << "TimerOneShot: null found at cleanup end." << Logger::endl;
//             for ( it = tasks.begin(); it != tasks.end(); ++it)
//                 Logger::log() << Logger::Error << *it << Logger::endl;
//         }
//         Logger::log() << "After:"<< Logger::nl;
//         for ( it = tasks.begin(); it != tasks.end(); ++it)
//             Logger::log() << Logger::Error << *it << Logger::endl;
//         ListActivity::iterator it2 = tasks.begin();
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
