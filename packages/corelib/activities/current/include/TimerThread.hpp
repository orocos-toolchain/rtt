/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  TimerThread.h 

                       TimerThread.h -  description
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


#ifndef ORO_TIMERTHREAD_HPP
#define ORO_TIMERTHREAD_HPP


#include "os/PeriodicThread.hpp"
#include "EventProcessor.hpp"

#include "os/Mutex.hpp"

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace ORO_CoreLib
{
    namespace detail {
        class TimerInterface;
    }
    class PeriodicActivity;
    class TimerThread;

    /**
     * TimerThread objects are reference counted such that
     * when the last PeriodicTask which uses it is deleted,
     * the thread is deleted as well.
     */
    typedef boost::shared_ptr<TimerThread> TimerThreadPtr;

    /**
     * This Periodic Thread is meant for executing a PeriodicActivity
     * object periodically. It does this by invoking a Timer object which
     * executes the activities.
     *
     * @see PeriodicActivity
     */
    class TimerThread
        : public ORO_OS::PeriodicThread,
          public EventProcessor
    {

    public:
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
         * Destructor
         */
        virtual ~TimerThread();

        /**
         * Add an Timer that will be ticked every execution period
         * Once added, a timer can not be removed.
         */
        bool timerAdd( detail::TimerInterface* );

        /**
         * Get a Timer ticking at a certain period.
         */
        detail::TimerInterface* timerGet( Seconds period ) const;

        /**
         * Create a TimerThread with a given priority and periodicity.
         */
        static TimerThreadPtr Instance(int priority, double periodicity);
    protected:
        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        typedef std::vector<detail::TimerInterface*> TimerList;

        /**
         * A list containing all the Timer instances
         *  we must tick
         *
         * @see Timer
         */ 
        TimerList clocks;

        /**
         * A Activity can not create a activity of same priority from step().
         * If so a deadlock will occur.
         */
        mutable ORO_OS::Mutex lock;

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
} // namespace ORO_CoreLib

#endif
