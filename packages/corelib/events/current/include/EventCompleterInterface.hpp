/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:50:45 CEST 2002  EventCompleterInterface.h 

                       EventCompleterInterface.h -  description
                          -------------------
   begin                : Wed April 17 2002
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


#ifndef EVENTCOMPLETERINTERFACE_HPP
#define EVENTCOMPLETERINTERFACE_HPP

namespace ORO_CoreLib
{
    class EventCompleterInterface;

    /**
     * This interface defines an object that completes
     * events.
     */
    class EventCompleterInterface
    {
        public:
        virtual ~EventCompleterInterface()
        {}

        /**
         * Complete an Event.
         *
         */
        virtual void completeEvent() = 0;

        /**
         * Use this Completer if you do not which completion
         * for a certain Event type or class.
         */
        static EventCompleterInterface* None; // The empty Completer
    };

    /**
     * We interpret the term Completer always as an Event Completer.
     */
    typedef EventCompleterInterface Completer;
        
}

#endif
