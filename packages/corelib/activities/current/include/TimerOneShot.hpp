/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TimerOneShot.hpp 

                        TimerOneShot.hpp -  description
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
 
 
#ifndef TASKONESHOTTIMER_HPP
#define TASKONESHOTTIMER_HPP

#include "TimerInterface.hpp"
#include <vector>
#include <os/Mutex.hpp>

namespace ORO_CoreLib
{
    namespace detail {

    /**
     * A Timer which step()s all tasks in 'one shot' in the
     * first tick given of each task period. It does discard ticks
     * if there are multiple ticks per task period, hence not spreading
     * task step()s over ticks.
     */
    class TimerOneShot
        : public TimerInterface
    {
        typedef std::vector<PeriodicActivity*> ListActivity ;
        nsecs trig_per;
        nsecs task_per;
        int   turn;
        ListActivity tasks;
        bool cleanup;
        /**
         * A recursive mutex allows non-deadlocking self-removal.
         */
        ORO_OS::MutexRecursive mutex;
        void reorderList();
    public:
        TimerOneShot( nsecs task_period );

        virtual ~TimerOneShot();

        virtual void start();

        virtual void stop();

        virtual bool addActivity( PeriodicActivity* t );

        virtual bool removeActivity( PeriodicActivity* t );

        virtual void tick();

        virtual nsecs getPeriod() const { return task_per; }

        virtual nsecs getTrigger() const { return trig_per; }

        virtual void setTrigger(nsecs ns) { trig_per = ns; }

    };
    }
}

#endif
