/***************************************************************************
 tag: Peter Soetens  Mon Jun 11 14:43:27 CEST 2002  EventInterrupt.cpp

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


#include "os/EventInterrupt.hpp"
#include "os/ISRInterface.hpp"
#include "os/DSRInterface.hpp"

#include "os/fosi.h" 
/**
 * TODO : Use assertions (Boost ?) instead of if (irq !=0)...
 */

namespace ORO_OS
{

    std::vector<EventInterrupt*> EventInterrupt::_instances;


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

    void EventInterrupt::addHandler( ISRInterface* thi, DSRInterface* bhi )
    {
        myTable.insert( std::make_pair( thi, bhi ) );
    }

    void EventInterrupt::addHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        Event::addHandler( eli, eci );
    }

    void EventInterrupt::removeHandler( EventListenerInterface* eli, EventCompleterInterface* eci )
    {
        Event::removeHandler( eli, eci );
    }

    void EventInterrupt::removeHandler( ISRInterface * eli, DSRInterface* eci )
    {
        // XXX check for correctness of eci
        myTable.erase( eli );
    }

    int EventInterrupt::IRQRequest()
    {
        int status = -1;

        if ( irq != 0 )
        {
            // XXX this is confusing for us
            if ( initHardWare != 0 )
                initHardWare();
            else
                hardwareInit();
        }

        return status;
    }

    void EventInterrupt::enable()
    {
        //    if (irq != 0)
        //        rt_startup_irq(irq);
    }

    void EventInterrupt::disable()
    {
        //    if (irq != 0)
        //        rt_shutdown_irq(irq);
    }

    void EventInterrupt::IRQFree()
    {
        //    if (irq !=0 )
        //        rt_free_linux_irq(irq, (void*)this);
    }

    void EventInterrupt::fire()
    {
        Event::fire(); // calls only supers notify
    }

    void EventInterrupt::interrupt( int irq, void* dev_id, struct pt_regs* regs )
    {
        Event::fire(); // call supers

        std::map<ISRInterface*, DSRInterface*>::iterator itl;

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
        if ( myTable[ ii ] != 0 )
            myTable[ ii ]->completeInterrupt();
    }

    unsigned int EventInterrupt::EventInterruptHandler22( unsigned int irq, struct pt_regs* regs )
    {
        EventInterrupt* ei = EventInterrupt::InstanceGet( irq );

        if ( ei != 0 )
        {
            ei->interrupt( irq, ( void* ) ei, regs ); // fire is the C++ handler version of this interrupt
            ei->enable();
        }

        return 0;  // store significant return value in EventInterrupt itself XXX
        // must be done in TopHalfs handler function
    }

    void EventInterrupt::EventInterruptHandler24( int irq, void* dev_id, struct pt_regs* regs )
    {
        EventInterrupt* ei = EventInterrupt::InstanceGet( irq );
        rtos_printf( "IRQ :%d\n ei: %p", irq, ei );

        if ( ei != 0 )
        {
            ei->interrupt( irq, dev_id, regs ); // fire is the C++ handler version of this interrupt
            ei->enable();
        }
    }


    EventInterrupt* EventInterrupt::InstanceGet( unsigned int irq )
    {
        //    if (_instances == 0 ) return 0;
        std::vector<EventInterrupt*>::iterator itl = _instances.begin();

        while ( ( *itl ) ->IRQGet() != irq && itl != _instances.end() )
            ++itl;

        return ( itl == _instances.end() ? 0 : *itl );
    }

    void EventInterrupt::InstanceAdd( EventInterrupt* ei )
    {
        //    if (_instances == 0) _instances = new vector<EventInterrupt*>;
        _instances.push_back( ei );
    }

    void EventInterrupt::InstanceRemove( EventInterrupt* ei )
    {
        std::vector<EventInterrupt*>::iterator itl;
        itl = std::find( _instances.begin(), _instances.end(), ei );

        if ( itl != _instances.end() )
            _instances.erase( itl );

        /*    if(_instances.size() == 0) {
                delete _instances;
                _instances=0;
            }*/
    }


}
