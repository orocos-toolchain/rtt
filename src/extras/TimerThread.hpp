/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  TimerThread.h

                       TimerThread.h -  description
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


#ifndef ORO_TIMERTHREAD_HPP
#define ORO_TIMERTHREAD_HPP


#include "../os/PeriodicThread.hpp"

#include "../os/Mutex.hpp"

#include <list>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace RTT
{
    class PeriodicActivity;
    class TimerThread;

    /**
     * TimerThread objects are reference counted such that
     * when the last PeriodicActivity which uses it is deleted,
     * the thread is deleted as well.
     */
    typedef boost::shared_ptr<TimerThread> TimerThreadPtr;

    /**
     * This Periodic Thread is meant for executing a PeriodicActivity
     * object periodically.
     *
     * @see PeriodicActivity
     */
    class RTT_API TimerThread
        : public OS::PeriodicThread
    {
        typedef std::vector<PeriodicActivity*> ActivityList ;
        ActivityList tasks;
        bool cleanup;
    public:
    	static const unsigned int MAX_ACTIVITIES = 64;
        /**
         * Create a periodic Timer thread.
         *
         * @param priority
         *        The priority of this thread
         * @param periodicity
         *        The periodicity of this thread in seconds (e.g. 0.001 = 1000Hz )
         */
        TimerThread(int priority, const std::string& name, double periodicity);

        /**
         * Create a periodic Timer thread with a given scheduler type.
         *
         * @param scheduler
         *        The scheduler in which this thread runs
         * @param priority
         *        The priority of this thread within \a scheduler
         * @param periodicity
         *        The periodicity of this thread in seconds (e.g. 0.001 = 1000Hz )
         */
        TimerThread(int scheduler, int priority, const std::string& name, double periodicity);

        /**
         * Destructor
         */
        virtual ~TimerThread();

        /**
         * Add an Timer that will be ticked every execution period
         * Once added, a timer can not be removed.
         */
        bool addActivity( PeriodicActivity* t );

        bool removeActivity( PeriodicActivity* t );

        /**
         * Create a TimerThread with a given priority and periodicity,
         * using the default scheduler, ORO_SCHED_RT.
         */
        static TimerThreadPtr Instance(int priority, double periodicity);
        /**
         * Create a TimerThread with a given scheduler, priority and periodicity.
         */
        static TimerThreadPtr Instance(int scheduler, int priority, double periodicity);
    protected:
        virtual bool initialize();
        virtual void step();
        virtual void finalize();
        void reorderList();
        /**
         * A Activity can not create a activity of same priority from step().
         * If so a deadlock will occur.
         */
        mutable OS::MutexRecursive mutex;

        /**
         * A Boost weak pointer is used to store non-owning pointers
         * to shared objects.
         */
        typedef std::vector< boost::weak_ptr<TimerThread> > TimerThreadList;

        /**
         * All timer threads.
         */
        static TimerThreadList TimerThreads;
    };
} // namespace RTT

#endif
