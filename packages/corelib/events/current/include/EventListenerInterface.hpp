/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:50:29 CEST 2002  EventListenerInterface.h 

                       EventListenerInterface.h -  description
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


#ifndef EVENTLISTENERINTERFACE_HPP
#define EVENTLISTENERINTERFACE_HPP

namespace ORO_CoreLib
{

    class CallbackInterface;
    class EventListenerInterface;
    class EventOperationInterface;

    /**
     * This interface describes an object that can react to
     * the firing of an EventOperationInterface instance
     */
    class EventListenerInterface
    {

    public:
        virtual ~EventListenerInterface()
        {}

        /**
         * This method is called when this Listener is added 
         * to an Event.
         */
        virtual void handleConnect( ) {}

        /**
         * Handles an occuring Event. Depending on the Event type
         * and implementation, handleEvent() will be called when the
         * event is fire()'ed.
         *
         * @param cb
         *        Used for calling back that the Event may be completed.
         */
        virtual void handleEvent( CallbackInterface *cb ) = 0;

        /**
         * This method is called when this Listener is removed 
         * from an Event.
         */
        virtual void handleClose( ) {}

        /**
         * Use this Listener if you do not which to be notified for
         * a certain Event type or class.
         */
        static EventListenerInterface* None; // A null Listener
    };

    /**
     * We understand Listener always as an EventListenerInterface.
     */
    typedef EventListenerInterface Listener;
}

#endif
