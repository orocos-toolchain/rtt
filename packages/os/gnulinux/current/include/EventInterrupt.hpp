/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:33:53 CEST 2002  EventInterrupt.hpp 

                       EventInterrupt.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
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

#ifndef EVENTINTERRUPT_HPP
#define EVENTINTERRUPT_HPP


#include <corelib/Event.hpp>
#include "EventInterruptInterface.hpp"

namespace ORO_OS
{

    using ORO_CoreLib::Event;
    using ORO_CoreLib::EventListenerInterface;
    using ORO_CoreLib::EventCompleterInterface;

    class ISRInterface;
    class DSRInterface;

    /**
     * An Event for interrupts
     * Top Half : the EventListener - ISR
     * Bottom Half : The EventCompleter - DSR
     *
     * The workings are pretty simple : 
     * * If you register a Listener,Completer pair,they will be notified, when 
     *   an interrupt occurs.
     *   
     * * If you register an ISR,DSR pair,they get notified when 
     *   an interrupt occurs
     *   
     * * When you fire(), only the Listener,Completer are notified (since this is
     *   software firing, not an IRQ)
     * 
     * @invar There is only one instance of EventInterrupt per IRQ
     *         foreach j,k in EventInterrupt,j != k : j.getIRQ() != k.getIRQ()
     */
    class EventInterrupt : public Event, public EventInterruptInterface
    {

    public:
        EventInterrupt();

        virtual ~EventInterrupt();

        /**
         * Register ISR and DSR like functions
         */
        virtual void addHandler( ISRInterface* isr, DSRInterface * dsr);

        virtual void addHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci);

        virtual void removeHandler( ISRInterface* isr, DSRInterface* dsr);

        /**
         * Request the set IRQ
         */
        virtual int IRQRequest();

        /**
         * Enable this interrupt
         */
        virtual void enable();

        /**
         * Disable this interrupt
         */
        virtual void disable();

        /**
         * Release this interrupt, as opposed to IRQrequest()
         */
        virtual void IRQFree();

        /**
         * A system interrupt occured
         * @post all ISRs and EventListeners are notified
         */
        virtual void interrupt( int irq );

        virtual void complete( ISRInterface* ethi );

        virtual void complete( EventListenerInterface* eli );


    protected:
        EventInterrupt( const EventInterrupt& );


    private:
        /**
         * A map where we keep the ISRs and DSRs, which are only
         * notified when a real interrupt occurs
         */
        std::map<ISRInterface*, DSRInterface*> myTable;

    };

} // namespace ORO_OS

#endif
