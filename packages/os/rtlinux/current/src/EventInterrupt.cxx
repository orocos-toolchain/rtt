/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:38 CEST 2002  EventInterrupt.cpp 

                       EventInterrupt.cpp -  description
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


#include "../../EventInterrupt.hpp"
#include "../../ISRInterface.hpp"
#include "../../DSRInterface.hpp"

namespace ORO_OS
{

    vector<EventInterrupt*> EventInterrupt::_instances;


    EventInterrupt::EventInterrupt()
            : Event( Event::SYNSYN ), irq( 0 ), initHardWare( 0 )
    {
        EventInterrupt::InstanceAdd( this );
    }

    EventInterrupt::~EventInterrupt()
    {
        EventInterrupt::InstanceRemove( this );
    }

    void EventInterrupt::hardwareInitSet( void ( *init ) () )
    {
        initHardWare = init;
    }

    void EventInterrupt::addHandler( ISRInterface* isr, DSRInterface* dsr )
    {
        myTable.insert( make_pair( isr, dsr ) );
    }

    void EventInterrupt::addHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        Event::addHandler( eli, eci );
    }

    void EventInterrupt::removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        Event::removeHandler( eli, eci );
    }


    void EventInterrupt::removeHandler( ISRInterface * isr, EventCompleterInterface* eci )
    {
        myTable.erase( isr );
    }


    int EventInterrupt::IRQRequest()
    {
        int status = -1;
        rtl_irqstate_t f;

        rtl_no_interrupts( f );

        if ( irq != 0 )
        {
            status = rtl_request_irq( irq, EventInterruptHandler22 );

            // XXX this is confusing for us

            if ( initHardWare != 0 )
                initHardWare();
            else
                hardwareInit();

            enable(); // maybe skip this
        }

        rtl_restore_interrupts( f );

        return status;
    }

    void EventInterrupt::enable()
    {
        if ( irq != 0 )
            rtl_hard_enable_irq( irq );
    }

    void EventInterrupt::disable()
    {
        if ( irq != 0 )
            rtl_hard_disable_irq( irq );
    }

    void EventInterrupt::IRQFree()
    {
        if ( irq != 0 )
            rtl_free_irq( irq );
    }

    void EventInterrupt::fire()
    {
        Event::fire();
    }

    void EventInterrupt::interrupt( int irq, void* dev_id, struct pt_regs* regs )
    {
        Event::fire(); // call supers

        map<ISRInterface*, DSRInterface*>::iterator itl;

        for ( itl = myTable.begin(); itl != myTable.end(); itl++ )
            itl->first->handleInterrupt( irq, dev_id, regs );
    }

    void EventInterrupt::complete( EventListenerInterface* eli )
    {
        // Implement here starting of the completion thread, and let it call complete
        Event::complete( eli );  // calls supers complete in case it was a normal event
    }


    void EventInterrupt::complete( ISRInterface* ii )
    {
        // Implement here starting of the completion thread, and let it call complete

        if ( myTable[ ii ] != NULL )
            myTable[ ii ] ->completeInterrupt();
    }

    /*
    // redefine notify
    // XXX cast here or cast in the BottomHalf/TopHalf ??? XXX
    // Maybe safer here, since you know 100% sure here what you got but not
    // in the halfs code
    void EventInterrupt::notify()
    {
                vector<EventListenerInterface*>::iterator itl;
                for (itl=listeners.begin(); itl != listeners.end(); ++itl)
                    dynamic_cast<ISRInterface*>(*itl)->handleInterrupt(this);
             
    //            map<EventListenerInterface*,EventCompleterInterface*>::iterator itl;
    //            for (itl = handlerMap.begin(); itl != handlerMap.end(); itl++)
    //                itl->first->handleEvent(this);
             
    }
    */

    void EventInterrupt::EventInterruptHandler24( int irq, void* dev_id, struct pt_regs* regs )
    {
        EventInterrupt* ei = EventInterrupt::InstanceGet( irq );

        if ( ei != 0 )
            ei->fire(); // fire is the C++ handler version of this interrupt
    }

    unsigned int EventInterrupt::EventInterruptHandler22( unsigned int irq, struct pt_regs* regs )
    {
        EventInterrupt* ei = EventInterrupt::InstanceGet( irq );

        if ( ei != 0 )
            ei->fire(); // fire is the C++ handler version of this interrupt

        return 0;
    }


    EventInterrupt* EventInterrupt::InstanceGet( unsigned int irq )
    {
        vector<EventInterrupt*>::iterator itl = _instances.begin();

        while ( ( *itl ) ->IRQGet() != irq && itl != _instances.end() )
            ++itl;

        return ( itl == _instances.end() ? 0 : *itl );
    }

    void EventInterrupt::InstanceAdd( EventInterrupt* ei )
    {
        _instances.push_back( ei );
    }

    void EventInterrupt::InstanceRemove( EventInterrupt* ei )
    {
        vector<EventInterrupt*>::iterator itl;
        itl = std::find( _instances.begin(), _instances.end(), ei );

        if ( itl != _instances.end() )
            _instances.erase( itl );
    }


};
