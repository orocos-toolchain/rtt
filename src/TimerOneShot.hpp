/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TimerOneShot.hpp 

                        TimerOneShot.hpp -  description
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
 
 
#ifndef TASKONESHOTTIMER_HPP
#define TASKONESHOTTIMER_HPP

#include "TimerInterface.hpp"
#include <vector>
#include "os/Mutex.hpp"

namespace RTT
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
        OS::MutexRecursive mutex;
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
