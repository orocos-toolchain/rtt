/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventPeriodic.hpp 

                        EventPeriodic.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef EVENTPERIODIC_HPP
#define EVENTPERIODIC_HPP

#include "EventInterfaces.hpp"
#include "Time.hpp"
#include "os/Mutex.hpp"

#include <list>

namespace ORO_CoreLib
{
    class CompletionProcessor;

    /**
     * An EventPeriodic is an object that will notify a listener every
     * n'th time it is fired() such that when n listeners are subscribed,
     * each listener will be notified on a different fire call of one period.
     * It will try to spread the notifying
     * depending on its notifying period and trigger period.
     * The former is the period between two consecutive listener
     * notifications for each listener, the latter, the elapsed time
     * between two fire() calls.
     *
     * @todo The implementation is based on <std::list>. This allows safe
     * add/remove of handlers during fire(). A more efficient implementation
     * with std::list is possible and TODO.
     */
    class EventPeriodic
        : public EventInterface,
          public CallbackInterface
    {
    public:
        /**
         * Creates an EventPeriodic which will have notified all listeners after
         * <average_period> / <trigger_period> times it is fired.
         * Every EventPeriodic is of the SYNASYN EventType.
         *
         * @param average_period The Average time between notifying each
         *        registered listener.
         * @param trigger_period The period between consecutive fire() calls.
         *        Defaults to average_period.
         */
        EventPeriodic( Seconds average_period, Seconds trigger_period = 0 );

        /**
         * Creates an EventPeriodic which will have notified each listener
         * after fire() was called <divider> times.
         * Every EventPeriodic is of the SYNASYN EventType.
         *
         * @param divider The average number of times fire() is called before
         *        each listener is notified. ( So N Listeners will all be 
         *        notified after fire() was called <divider> times.
         */
        EventPeriodic( unsigned int divider );

        virtual void fire();

        virtual void complete();
        
        virtual void complete(EventListenerInterface* eli);

        virtual void addHandler( EventListenerInterface* eli, EventCompleterInterface* eci );

        virtual void removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci );

        /**
         * Returns the periodicity with which the listeners are notified
         */
        nsecs periodGet();

        /**
         * Sets the period with which this event will be fired.
         */
        void triggerPeriodSet(nsecs ns);
        
        static const unsigned int MAX_LISTENERS=32;
    protected:

        struct ListItem
        {
            ListItem() : eli(0), eci(0), priority(~0), turn(0) {}
            EventListenerInterface* eli;
            EventCompleterInterface* eci;
            unsigned int priority;
            unsigned int turn;
        };

        struct CMP : public std::binary_function<ListItem, EventListenerInterface*, bool>
        { 
            bool operator()(const ListItem& p, const EventListenerInterface* eli) const
            {
                return p.eli == eli;
            }
        };

        struct PrioritySort : public std::binary_function<ListItem, ListItem, bool>
        {
            bool operator()(const ListItem& f, const ListItem& s) const
            {
                return f.priority < s.priority;
            }
        };

        inline
        void update();

        nsecs avPeriod;

        nsecs trigPeriod;

        unsigned long relPeriod;

        /**
         * monotonic increasing number.
         */
        unsigned long total_added;

        unsigned long turn;

        std::list<ListItem> sortedList;

        std::list<ListItem>::iterator f_iter;

        CompletionProcessor* cp;

        EventCompleterInterface* completerHack;

        bool reset_f_iter;

        ORO_OS::Mutex mut;

    };

}

#endif
