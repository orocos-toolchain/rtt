/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventDrivenActivity.hpp

                        EventDrivenActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef TASK_EVENT_DRIVEN_HPP
#define TASK_EVENT_DRIVEN_HPP

#include "../Activity.hpp"
#include "../internal/Signal.hpp"
#include "../base/Buffer.hpp"
#include <set>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace extras {


    /**
     * @brief An Event-driven base::ActivityInterface implementation.
     *
     * This class represents a Activity which can be attached
     * to an Signal<void(void)> and execute its functionality each time
     * the event is fired. The EventDrivenActivity is run asynchronously
     * in a given thread.
     */
    class RTT_API EventDrivenActivity
        : public Activity
    {
        // The set of events that can trigger this activity
        typedef std::vector< internal::Signal< void() >* > Signals;
        Signals   m_events;

        // The set of connection handles which link m_events with the activity
        typedef std::vector<Handle> Handles;
        Handles  m_handles;

        // The set of pending events (i.e. events that have been emitted since
        // the last time the activity went to sleep)
        typedef base::Buffer< internal::Signal< void() >*, base::BlockingPolicy, base::NonBlockingPolicy > Triggers;
        Triggers* m_pending_events;

        // The set of wakeup events (i.e. the events which triggered the
        // current wakeup of the activity)
        typedef std::vector< internal::Signal< void() >* > Wakeup;
        Wakeup   m_wakeup;

        // The trigger method, called by the event. \c event_id is the index of
        // the event in m_events.
        void event_trigger(internal::Signal<void()>* event);

    public:
        /**
         * Create an EventDrivenActivity which will run in a separate thread,
         * using its own event processor. The default scheduler for
         * EventDrivenActivity objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        EventDrivenActivity(int priority, base::RunnableInterface* _r = 0, const std::string& name ="EventDrivenActivity"  );

        /**
         * Create an EventDrivenActivity which will run in a separate thread,
         * using its own event processor.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        EventDrivenActivity(int scheduler, int priority, base::RunnableInterface* _r = 0, const std::string& name ="EventDrivenActivity" );

        /**
         * Cleanup and notify the base::RunnableInterface that we are gone.
         */
        ~EventDrivenActivity();

        bool start();
        bool stop();
        void loop();
        bool breakLoop();

        /** Returns the set of events that caused the activity to wake up. This
         * is only valid in the associated base::RunnableInterface's step() method
         * (and, by extension, in the TaskContext hook functions)
         */
        std::vector<internal::Signal<void()>*> const& getWakeupEvents() const;

        /**
         * Set the Event which will trigger the execution
         * of this task, once started.
         */
        bool addEvent( internal::Signal<void(void)>* _event);
    };

}}


#endif
