/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  EventInterrupt.cxx 

                        EventInterrupt.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

namespace ORO_OS
{

    EventInterrupt::EventInterrupt()
            : Event( Event::SYNSYN )
    {
    }

    EventInterrupt::~EventInterrupt()
    {
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
        myTable.erase( eli );
    }

    int EventInterrupt::IRQRequest()
    {
        int status = -1;

        if ( irq != 0 )
        {
            if ( initHardWare != 0 )
                initHardWare();
            else
                hardwareInit();
        }

        return status;
    }

    void EventInterrupt::enable()
    {
    }

    void EventInterrupt::disable()
    {
    }

    void EventInterrupt::IRQFree()
    {
    }

    void EventInterrupt::interrupt( int irq )
    {
        Event::fire(); // call supers

        std::map<ISRInterface*, DSRInterface*>::iterator itl;

        for ( itl = myTable.begin(); itl != myTable.end(); itl++ )
            itl->first->handleInterrupt( irq, (void*)this, 0 );
    }

    void EventInterrupt::complete( EventListenerInterface* eli )
    {
        Event::complete( eli );
    }

    void EventInterrupt::complete( ISRInterface* ii )
    {
        if ( myTable[ ii ] != 0 )
            myTable[ ii ]->completeInterrupt();
    }

}
