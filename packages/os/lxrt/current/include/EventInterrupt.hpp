/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventInterrupt.hpp 

                        EventInterrupt.hpp -  description
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
 
#ifndef EVENTINTERRUPT_HPP
#define EVENTINTERRUPT_HPP


#include "EventInterruptInterface.hpp"
#include <corelib/Event.hpp>

struct rt_tasklet_struct;

namespace ORO_OS
{
    using ORO_CoreLib::Event;
    using ORO_CoreLib::CallbackInterface;
    using ORO_CoreLib::EventListenerInterface;
    using ORO_CoreLib::EventCompleterInterface;

    class ISRInterface;
    class DSRInterface;

    /**
     * An Event for interrupts which implements the 
     * EventInterruptInterface and extends Event.
     *
     */
    class EventInterrupt 
        : public Event, public EventInterruptInterface
    {
        struct rt_tasklet_struct *tasklet;
    public:
        EventInterrupt();

        virtual ~EventInterrupt();

        virtual void addHandler( ISRInterface* isr, DSRInterface * dsr);

        virtual void addHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( ISRInterface* isr, DSRInterface* dsr);

        virtual int IRQRequest();

        virtual void enable();

        virtual void disable();

        virtual void IRQFree();

        virtual void complete( ISRInterface* ethi );

    protected:
        EventInterrupt( const EventInterrupt& );
        
        virtual void interrupt( int irq );

        virtual void complete( EventListenerInterface* cb);
        /**
         * An interrupt handler as RTAI Tasklet.
         */
        static void EventInterruptTasklet( unsigned long data );
    private:

        /**
         * A map where we keep the ISRs and DSRs, which are only
         * notified when a real interrupt occurs
         */
        std::map<ISRInterface*, DSRInterface*> myTable;

        /**
         * Manual enable flag.
         */
        bool enable_f;
    };

} // namespace ORO_OS

#endif
