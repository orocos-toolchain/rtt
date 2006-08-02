/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:49:58 CEST 2002  Event.h

                       Event.hpp -  description
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


#ifndef ORO_CORELIB_EVENT_HPP
#define ORO_CORELIB_EVENT_HPP

#include "os/fosi.h"
#include "Signal.hpp"
#include "Handle.hpp"
#include <boost/call_traits.hpp>
#include "NameServerRegistrator.hpp"

#include "Logger.hpp"
#include "CompletionProcessor.hpp"
#include "ActivityInterface.hpp"
#include "TimerThread.hpp"
#include "EventProcessor.hpp"
#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"
#include <cassert>

namespace RTT
{

    /**
     * The Orocos Event is a thread-safe publish-subscribe implementation
     * and provides synchronous and asynchronous callback handling.
     *
     * Use connect() to immediately connect a callback function to this Event.
     * The returned Handle object can be used to disconnect/reconnect it lateron.
     *
     * Use setup() to setup a connection of a callback function with this Event,
     * but leave it in a dormant state. Use the returned Handle object and connect()
     * to establish the connection.
     *
     * @param _Signature The function type signature of the callback functions
     * this event will call. For example void(int, double) if you want the
     * event to call a function 'void foo(int x, double y)';
     * @see The Orocos CoreLib manual for usage.
     * @ingroup CoreLibEvents
     */
    template<
        typename _Signature // function type R (T1, T2, ..., TN)
    >
    class Event
        : public sigslot::signal<_Signature, boost::function<_Signature> >,
          private NameServerRegistrator<Event<_Signature>*>
    {
    protected:
        std::string mname;
    public:
        /**
         * @see EventProcessor::AsynStorageType
         */
        typedef EventProcessor::AsynStorageType AsynStorageType;

        typedef boost::function<_Signature> SlotFunction;

        typedef typename sigslot::signal<
            _Signature,
            SlotFunction>::base_type signal_type;

        typedef typename sigslot::signal<
            _Signature,
            SlotFunction> signal_base_type;

        typedef Event<_Signature> EventType;

        typedef _Signature Signature;

        /**
         * The result type of the function signature.
         */
        typedef typename boost::function_traits<_Signature>::result_type result_type;
        /**
         * The result type of emit().
         */
        typedef void emit_type;

        /**
         * Create a named Synchronous/Asynchronous Event.
         */
        explicit Event(const std::string name )
            : signal_base_type(),
              NameServerRegistrator<EventType*>(nameserver, name, this),
              mname(name)
        {
            Logger::log() << Logger::Debug << "Event Created with name  : "<< name << Logger::endl;
        }

        /**
         * Create a Synchronous/Asynchronous Event.
         */
        Event()
            : signal_base_type()
        {
        }

        const std::string& getName() const { return mname; }

        using signal_base_type::emit;
        using signal_base_type::fire;
        using signal_base_type::operator();

        /**
         * @brief Connect a synchronous event slot to this event.
         */
        Handle connect(const SlotFunction& f)
        {
            return Handle( signal_type::connect( f ) );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, EventProcessor* ep, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( ep->connect( l, *this, t ) );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance(), EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::connect( l ), ep->connect( c, *this, t ) );
        }

        /**
         * @brief Setup a synchronous event slot to this event.
         */
        Handle setup(const SlotFunction& f)
        {
            return Handle( signal_type::setup( f ) );
        }

        /**
         * @brief Setup an Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, EventProcessor* ep, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( ep->setup( l, *this, t ) );
        }

        /**
         * @brief Setup a Synchronous and Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance(), EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::setup( l ), ep->setup( c, *this, t ) );
        }

        /**
         * @brief Public nameserver for Events.
         * @see also EventService for a 'type-less'/universal
         * Event nameserver.
         */
        static NameServer<EventType*> nameserver;
    };


    template<
        typename _Signature
    >
    NameServer<Event<_Signature>*>
    Event<_Signature>::nameserver;

}

#endif
