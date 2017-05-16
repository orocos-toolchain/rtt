/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TimerThread.cpp

                       TimerThread.cpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
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

#include "TimerThread.hpp"
#include "PeriodicActivity.hpp"

#include "rtt-config.h"

#include "../Time.hpp"
#include "../Logger.hpp"
#include <algorithm>
#include "../os/MutexLock.hpp"
#include "../os/MainThread.hpp"

namespace RTT {
    using namespace extras;
    using namespace base;
    using os::MutexLock;
    using namespace std;

    TimerThread::TimerThreadList TimerThread::TimerThreads;

    TimerThreadPtr TimerThread::Instance(int pri, double per)
    {
        return Instance(ORO_SCHED_RT, pri, per);
    }

    TimerThreadPtr TimerThread::Instance(int scheduler, int pri, double per)
    {
      return Instance(scheduler, pri, per, 0);
    }

    TimerThreadPtr TimerThread::Instance(int scheduler, int pri, double per, unsigned cpu_affinity)
    {
        // Since the period is stored as nsecs, we convert per to NS in order
        // to get a match.
        os::CheckPriority(scheduler, pri);
        if (cpu_affinity == 0) cpu_affinity = os::MainThread::Instance()->getCpuAffinity();
        TimerThreadList::iterator it = TimerThreads.begin();
        while ( it != TimerThreads.end() ) {
            TimerThreadPtr tptr = it->lock();
            // detect old pointer.
            if ( !tptr ) {
                TimerThreads.erase(it);
                it = TimerThreads.begin();
                continue;
            }
            if ( tptr->getScheduler() == scheduler &&
                 tptr->getPriority() == pri &&
                 tptr->getPeriodNS() == Seconds_to_nsecs(per) &&
                 tptr->getCpuAffinity() == cpu_affinity ) {
                return tptr;
            }
            ++it;
        }
        TimerThreadPtr ret( new TimerThread(scheduler, pri, "TimerThreadInstance", per, cpu_affinity) );
        TimerThreads.push_back( ret );
        return ret;
    }

    TimerThread::TimerThread(int priority, const std::string& name, double periodicity, unsigned cpu_affinity)
        : Thread( ORO_SCHED_RT, priority, periodicity, cpu_affinity, name), cleanup(false)
    {
    	tasks.reserve(MAX_ACTIVITIES);
    }

    TimerThread::TimerThread(int scheduler, int priority, const std::string& name, double periodicity, unsigned cpu_affinity)
        : Thread(scheduler, priority, periodicity, cpu_affinity, name), cleanup(false)
    {
    	tasks.reserve(MAX_ACTIVITIES);
    }

    TimerThread::~TimerThread()
    {
        // make sure the thread does not run when we start deleting clocks...
        this->stop();
    }

    bool TimerThread::addActivity( PeriodicActivity* t ) {
        MutexLock lock(mutex);
        if ( tasks.size() == MAX_ACTIVITIES ) {
//             Logger::log() << Logger:: << "TimerThread : tasks queue full, failed to add Activity : "<< t << Logger::endl;
            return false;
        }
        tasks.push_back( t );
//         Logger::log() << Logger::Debug << "TimerThread : successfully started Activity : "<< t  << Logger::endl;
        return true;
    }

    bool TimerThread::removeActivity( PeriodicActivity* t ) {
        MutexLock lock(mutex);
        ActivityList::iterator it = find(tasks.begin(), tasks.end(), t);
        if ( it != tasks.end() ) {
            *it = 0; // clear task away
            cleanup = true;
            return true;
        }
//         Logger::log() << Logger::Debug << "TimerThread : failed to stop Activity : "<< t->getPeriod() << Logger::endl;
        return false;
    }

    bool TimerThread::initialize() {
    	return true;
    }

    void TimerThread::finalize() {
        MutexLock lock(mutex);

        for( ActivityList::iterator t_iter = tasks.begin(); t_iter != tasks.end(); ++t_iter)
            if ( *t_iter )
                (*t_iter)->stop(); // stop() calls us back to removeActivity (recursive mutex).
        if ( cleanup )
            this->reorderList();
    }

    void TimerThread::step() {
        MutexLock lock(mutex);

        // The size of the tasks vector does not change during add/remove, thus
        // t_iter is never invalidated.
        for( ActivityList::iterator t_iter = tasks.begin(); t_iter != tasks.end(); ++t_iter)
            if ( *t_iter ) {
                (*t_iter)->step();
                if (*t_iter)
                    (*t_iter)->work(RunnableInterface::TimeOut);
            }

        if ( cleanup )
            this->reorderList();
    }

    void TimerThread::reorderList() {
        // reorder the list to remove clear'ed tasks
        ActivityList::iterator begin = tasks.begin();
        // first zero :
        PeriodicActivity* nullActivity = 0;
        ActivityList::iterator it = tasks.begin();

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
            ActivityList::iterator end = find ( begin, tasks.end(), nullActivity);
            // if end == tasks.end() ==> next while will copy all
            // if end != tasks.end() ==> next while will copy first zero's
            while ( begin != end ) {
                *it = *begin; // copy operation
                ++begin;
                ++it; // go to next slot to inspect.
            }
        }

        cleanup = false;
    }


}
