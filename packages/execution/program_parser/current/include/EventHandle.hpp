/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:41:00 CEST 2004  EventHandle.hpp 

                        EventHandle.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef EXEC_EVENTHANDLE_HPP
#define EXEC_EVENTHANDLE_HPP

#include <corelib/CommandInterface.hpp>
#include <corelib/EventInterfaces.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::HandlerRegistrationInterface;
    using ORO_CoreLib::EventListenerInterface;
    namespace detail 
    {
    class CommandConnect : public CommandInterface
    {
    public :
        HandlerRegistrationInterface* event;
        EventListenerInterface* listener;

        CommandConnect( HandlerRegistrationInterface* _e, EventListenerInterface* _l )
            : event(_e), listener(_l)
        {}
        
        virtual void execute() {
            event->addHandler( listener, ORO_CoreLib::Completer::None );
        }

        virtual std::string toString()
        {
            return std::string("CommandConnect");
        }

        virtual CommandInterface* clone() const
        {
            return new CommandConnect(event, listener);
        }
    };


    class CommandDisconnect : public CommandInterface
    {
    public :
        HandlerRegistrationInterface* event;
        EventListenerInterface* listener;

        CommandDisconnect( HandlerRegistrationInterface* _e, EventListenerInterface* _l )
            : event(_e), listener(_l)
        {}
        
        virtual void execute() {
            event->removeHandler( listener, ORO_CoreLib::Completer::None );
        }

        virtual std::string toString()
        {
            return std::string("CommandDisconnect");
        }

        virtual CommandInterface* clone() const
        {
            return new CommandDisconnect(event, listener);
        }
    };

    /**
     * A Helper class for the StateGraphParser.
     */
    struct EventHandle
    {
        HandlerRegistrationInterface* event;
        EventListenerInterface* listener;
        
        EventHandle() : event(0), listener(0) {}

        void init( HandlerRegistrationInterface* _e, EventListenerInterface* _l)
        {
            event = _e;
            listener = _l;
        }

        CommandInterface* createConnect()
        {
            return new CommandConnect( event, listener );
        }

        CommandInterface* createDisconnect()
        {
            return new CommandDisconnect( event, listener );
        }
    };
    }
}

#endif
