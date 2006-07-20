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
#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include <cassert>

namespace ORO_CoreLib
{

    /**
     * The Orocos Event is a thread-safe wrapper around the signal class
     * and extends its connection syntax with asynchronous event handling.
     * @see The Orocos CoreLib manual for usage.
     */
    template<
        typename _Signature, // function type R (T1, T2, ..., TN)
        typename _SlotFunction = boost::function<_Signature>
    >
    class Event
        : public sigslot::signal<_Signature, _SlotFunction>,
          private NameServerRegistrator<Event<_Signature,
                                              _SlotFunction>*>
    {
    protected:
        std::string mname;
    public:
        /**
         * @see EventProcessor::AsynStorageType
         */
        typedef EventProcessor::AsynStorageType AsynStorageType;

        typedef typename sigslot::signal<
            _Signature,
            _SlotFunction>::base_type signal_type;

        typedef typename sigslot::signal<
            _Signature,
            _SlotFunction> signal_base_type;

        typedef Event<
            _Signature,
            _SlotFunction> EventType;

        typedef _Signature Signature;
        typedef _SlotFunction SlotFunction;

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
         * @deprecated You must specify
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
        Handle connect( const SlotFunction& l, ActivityInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->connect( l, task->getEventProcessor(), t );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, EventProcessor* ep, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( ep->connect( l, *this, t ) );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, TimerThread* tt, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( tt->getEventProcessor()->connect( l, *this, t ) );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, ActivityInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->connect( l,c, task->getEventProcessor(), t );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance()->getEventProcessor(), EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::connect( l ), ep->connect( c, *this, t ) );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, TimerThread* tt, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::connect( l ), tt->getEventProcessor()->connect( c, *this, t ) );
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
        Handle setup( const SlotFunction& l, ActivityInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->setup( l, task->getEventProcessor(), t );
        }

        /**
         * @brief Setup an Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, TimerThread* tt, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->setup( l, tt->getEventProcessor(), t );
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
        Handle setup( const SlotFunction& l, const SlotFunction& c, ActivityInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->setup( l,c, task->getEventProcessor(), t );
        }

        /**
         * @brief Setup a Synchronous and Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, const SlotFunction& c, TimerThread* tt, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->setup( l,c, tt->getEventProcessor(), t );
        }

        /**
         * @brief Setup a Synchronous and Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance()->getEventProcessor(), EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::setup( l ), ep->setup( c, *this, t ) );
        }

        /**
         * @brief Public nameserver for Events.
         * @see also ORO_Execution::EventService for a 'type-less'/universal
         * Event nameserver.
         */
        static NameServer<EventType*> nameserver;
    };


    template<
        typename _Signature,
        typename _SlotFunction
    >
    NameServer<Event< _Signature, _SlotFunction>*>
    Event<_Signature, _SlotFunction>::nameserver;

}

#endif
