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


#ifndef TASKEXECUTION_HPP
#define TASKEXECUTION_HPP


#include "os/PeriodicThread.hpp"
#include "EventProcessor.hpp"

#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

#include <list>

namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;
    namespace detail {
        class TaskTimerInterface;
    }
    class PeriodicTask;

    /**
     * This Periodic Thread is meant for executing a PeriodicTask
     * object periodically.
     *
     * @see PeriodicTask
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
        bool timerAdd( detail::TaskTimerInterface* );

        /**
         * Get a Timer ticking at a certain period.
         */
        detail::TaskTimerInterface* timerGet( Seconds period ) const;

    protected:
        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        typedef std::vector<detail::TaskTimerInterface*> TimerList;

        /**
         * A list containing all the TaskTimer instances
         *  we must tick
         *
         * @see TaskTimer
         */ 
        TimerList clocks;

        /**
         * A Task can not create a task of same priority from step().
         * If so a deadlock will occur.
         */
        mutable ORO_OS::Mutex lock;
    };
} // namespace ORO_CoreLib

#endif
