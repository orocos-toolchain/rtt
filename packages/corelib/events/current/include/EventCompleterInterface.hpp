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
     * events. It is a completion of the handleEvent() method,
     * when it requested complete() on the given CallBackInterface.
     */
    class EventCompleterInterface
    {
        public:
        virtual ~EventCompleterInterface()
        {}

        /**
         * This method is called when this Completer is added 
         * to an Event, after the Listener's handleConnect() method
         * is called.
         */
        virtual void completeConnect( ) {}

        /**
         * Complete an Event. This method will be called, depending
         * on Event type and implementation, when the handleEvent()
         * method of the Listener requested completion. So this is
         * a callback on a callback, mainly used to do the second
         * callback in another thread, asynchronously
         * ( being the CompletionProcessor ).
         */
        virtual void completeEvent() = 0;

        /**
         * This method is called when this Completer is removed 
         * from an Event, after the Listener's handleClose() method
         * is called.
         */
        virtual void completeClose( ) {}

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
