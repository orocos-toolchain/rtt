/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:33:53 CEST 2002  EventInterrupt.hpp 

                       EventInterrupt.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef EVENTINTERRUPT_HPP
#define EVENTINTERRUPT_HPP


#include <corelib/Event.hpp>

struct pt_regs;

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
    class EventInterrupt : public Event
    {

        public:
            EventInterrupt();

            virtual ~EventInterrupt();

            /**
             * Set the IRQ this event reacts to
             */
            virtual void IRQSet( unsigned int i )
            {
                irq = i;
            }

            /**
             * Read the IRQ this events reacts to
             */
            virtual unsigned int IRQGet() const
            {
                return irq;
            }

            /**
             * Assign a C function for hardware initialisation
             * This will override any default behaviour implemented
             * in EventInterrupt itself
             */
            virtual void hardwareInitSet( void ( *init ) () );

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

            virtual void fire();

            /**
             * A system interrupt occured
             * @post all ISRs and EventListeners are notified
             */
            virtual void interrupt( int irq, void* dev_id, struct pt_regs* regs );

        virtual void complete( ISRInterface* ethi );

        virtual void complete( EventListenerInterface* eli );


        protected:
            EventInterrupt( const EventInterrupt& );

            /**
             * A dummy function which can get reimplemented in a derived class
             * for default initalisation of hardware.
             * Default, nothing is done.
             * This can be overriden by hardwareInitSet().
             */
            virtual void hardwareInit()
        {  }

            /**
             * The irq this EventInterrupt reacts to
             */
            unsigned int irq;

            /**
             * A C function to be executed for hardware initialisation
             * When not null, this will override the default hardwareInit()
             */
            void ( *initHardWare ) ();

            /**
             * An interrupt handler conform kernel 2.2.x
             */

            static unsigned int EventInterruptHandler22( unsigned int, struct pt_regs* regs );

            /**
             * An interrupt handler conform kernel 2.4.x
             */

            static void EventInterruptHandler24( int, void* dev_id, struct pt_regs* regs );

        public:
            /**
             * Get the instance of EventInterrupt that handles a certain irq
             */
            static EventInterrupt* InstanceGet( unsigned int irq );

            /**
             * Add an instance of EventInterrupt
             */
            static void InstanceAdd( EventInterrupt* );

            /**
             * Remove an instance of EventInterrupt
             */
            static void InstanceRemove( EventInterrupt* );


        private:
            /**
             * A vector of all existing EventInterrupt instances
             */
            static std::vector<EventInterrupt*> _instances;

            /**
             * A map where we keep the ISRs and DSRs, which are only
             * notified when a real interrupt occurs
             */
            std::map<ISRInterface*, DSRInterface*> myTable;

    };

} // namespace ORO_OS

#endif
