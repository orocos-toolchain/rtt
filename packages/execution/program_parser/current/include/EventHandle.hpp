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
#include <corelib/Event.hpp>

namespace ORO_Execution
{
    namespace detail 
    {
        using namespace ORO_CoreLib;

    class CommandConnect : public CommandInterface
    {
        Event<void(void)>* event;
        boost::function<void(void)> listener;
        Handle* h;
    public :
        CommandConnect( Event<void(void)>* _e, boost::function<void(void)> _l, Handle* _h )
            : event(_e), listener(_l), h( _h)
        {}
        
        virtual void execute() {
            *h = event->connect( listener );
        }

        virtual CommandInterface* clone() const
        {
            return new CommandConnect(event, listener, h);
        }
    };


    class CommandDisconnect : public CommandInterface
    {
        Handle* h;
    public :
        CommandDisconnect( Handle* _h )
            : h(_h)
        {}
        
        virtual void execute() {
            h->disconnect();
        }

        virtual CommandInterface* clone() const
        {
            return new CommandDisconnect(h);
        }
    };

    /**
     * A Helper class for the StateGraphParser.
     */
    struct EventHandle
    {
        Event<void(void)>* event;
        boost::function<void(void)> listener;
        Handle h;
        
        EventHandle() : event(0) {}

        void init( Event<void(void)>* _e,  boost::function<void(void)> _l)
        {
            event = _e;
            listener = _l;
        }

        CommandInterface* createConnect()
        {
            return new CommandConnect( event, listener, &h );
        }

        CommandInterface* createDisconnect()
        {
            return new CommandDisconnect( &h );
        }
    };
    }
}

#endif
