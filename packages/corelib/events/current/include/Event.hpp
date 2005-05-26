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
#include <boost/signal.hpp>
#include <boost/call_traits.hpp>
#include "NameServerRegistrator.hpp"

#include "Logger.hpp"
#include "CompletionProcessor.hpp"
#include "TaskInterface.hpp"
#include "EventProcessor.hpp"
#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include <assert.h>

namespace ORO_CoreLib
{

        /**
         * @brief The Handle holds the information of a connection
         * between an Event Handler function and the Event itself.
         *
         * It is returned by the connection() method of Event and can
         * be used to disconnect a handler function from the event.
         */
        class Handle
        {
        public:
            boost::signals::connection _c;
            boost::signals::connection _c2;
            ORO_OS::MutexRecursive* _mutex;
            Handle( const boost::signals::connection & c,
                    const boost::signals::connection & c2,
                    ORO_OS::MutexRecursive& mutex) : _c(c), _c2(c2), _mutex(&mutex) {}
            Handle( const boost::signals::connection & c, ORO_OS::MutexRecursive& mutex ) : _c(c), _c2(c), _mutex(&mutex){}
            Handle( const Handle& h ) : _c(h._c), _c2(h._c2), _mutex(h._mutex) {}
            Handle() : _mutex(0) {}

            Handle& operator=(const Handle& h) {
                _c = h._c;
                _c2 = h._c2;
                _mutex = h._mutex;
                return *this;
            }

            bool operator==(const Handle& h) const {
                return (_c == h._c) && (_c2 == h._c2);
            }

            bool operator<(const Handle& h) const {
                return (_c < h._c) && (_c2 < h._c2);
            }

            bool connected() const {
                return _c.connected() && _c2.connected();
            }

            void disconnect() {
                if ( _mutex == 0 )
                    return; // nothing to disconnect
                    
                ORO_OS::MutexLock lock(*_mutex); // can lock on fire(), unless it is self-removal.
                _c.disconnect();
                _c2.disconnect();
            }
        };

    /**
     * The Orocos Event is a thread-safe wrapper around the boost::signal library
     * and extends its connection syntax with asynchronous event handling.
     * @see The Orocos CoreLib manual for usage.
     */
    template<
        typename _Signature, // function type R (T1, T2, ..., TN)
        typename Combiner = boost::last_value<typename boost::function_traits<_Signature>::result_type>,
        typename Group = int,
        typename GroupCompare = std::less<Group>,
        typename _SlotFunction = boost::function<_Signature>
    >
    class Event
        : public boost::signal<_Signature, Combiner, Group, GroupCompare, _SlotFunction>,
          private NameServerRegistrator<Event<_Signature,
            Combiner,
            Group,
            GroupCompare,
            _SlotFunction>*>
    {
        ORO_OS::MutexRecursive _mutex;
    public:
        /**
         * @see EventProcessor::AsynStorageType
         */
        typedef EventProcessor::AsynStorageType AsynStorageType;

        typedef boost::signal<
            _Signature,
            Combiner,
            Group,
            GroupCompare,
            _SlotFunction> signal_type;

        typedef Event<
            _Signature,
            Combiner,
            Group,
            GroupCompare,
            _SlotFunction> EventType;

        typedef _Signature Signature;
        typedef _SlotFunction SlotFunction;

        typedef typename boost::function_traits<_Signature>::result_type result_type;

        /**
         * Create a named Synchronous/Asynchronous Event.
         * @see boost::signals library
         */
        explicit Event(const std::string name,
                       const Combiner& combiner = Combiner(),
                       const GroupCompare& group_compare = GroupCompare())
            : signal_type(combiner, group_compare),
              NameServerRegistrator<EventType*>(nameserver, name, this)
        {
            Logger::log() << Logger::Debug << "Event Created with name  : "<< name << Logger::endl;
        }

        /**
         * Create a Synchronous/Asynchrnous Event.
         * @see boost::signals library
         */
        explicit Event(const Combiner& combiner = Combiner(),
                       const GroupCompare& group_compare = GroupCompare()) :
            signal_type(combiner, group_compare)
        {
            Logger::log() << Logger::Debug << "Nameless Event Created." << Logger::endl;
        }

        /**
         * @brief Connect a synchronous event slot to this event.
         */
        Handle connect(const SlotFunction& f)
        {
            return Handle(signal_type::connect( f ), _mutex);
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, TaskInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( task->getEventProcessor()->connect( l, *this, t ), _mutex );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, EventProcessor* ep, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( ep->connect( l, *this, t ), _mutex );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, TaskInterface* task, EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return Handle( signal_type::connect( l ), task->getEventProcessor()->connect( c, *this, t ), _mutex );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance()->getEventProcessor(), EventProcessor::AsynStorageType t = EventProcessor::OnlyFirst )
        {
            return Handle( signal_type::connect( l ), ep->connect( c, *this, t ), _mutex );
        }

        //! @name EventFire Event Fire methods
        //! @{
        /**
         * This method is the thread-safe equivalent to the operator() method
         * of boost::signal. You must call the fire( \a Signature ) method corresponding to the
         * parameter signature of your Event< \a Signature >. The return value can not be collected
         * and is thus always \c void.
         */
        void fire() {
            ORO_OS::MutexLock lock( _mutex );
            signal_type::operator()();
        }

        template<class A1>
        void fire(A1 a1) {
            ORO_OS::MutexLock lock( _mutex );
            signal_type::operator()(a1);
        }

        template<class A1, class A2>
        void fire(A1 a1,
                  A2 a2) {
                      ORO_OS::MutexLock lock( _mutex );
                      signal_type::operator()(a1, a2);
                  }

        template<class A1, class A2, class A3>
        void fire(A1 a1,
                  A2 a2,
                  A3 a3) {
                      ORO_OS::MutexLock lock( _mutex );
                      signal_type::operator()(a1, a2, a3);
                  }

        template<class A1, class A2, class A3, class A4>
        void fire(A1 a1,
                  A2 a2,
                  A3 a3,
                  A4 a4) {
                      ORO_OS::MutexLock lock( _mutex );
                      signal_type::operator()(a1, a2, a3, a4);
                  }

        template<class A1, class A2, class A3, class A4, class A5>
        void fire(A1 a1,
                  A2 a2,
                  A3 a3,
                  A4 a4,
                  A5 a5) {
                      ORO_OS::MutexLock lock( _mutex );
                      signal_type::operator()(a1, a2, a3, a4, a5);
                  }

        template<class A1, class A2, class A3, class A4, class A5, class A6>
        void fire(A1 a1,
                  A2 a2,
                  A3 a3,
                  A4 a4,
                  A5 a5,
                  A6 a6) {
                      ORO_OS::MutexLock lock( _mutex );
                      signal_type::operator()(a1, a2, a3, a4, a5, a6);
                  }

        //! @}

        // repeat for A7,A8,... + same in EventProcessor.hpp

        /**
         * @brief Public nameserver for Events.
         */
        static NameServer<EventType*> nameserver;
    };


    template<
        typename _Signature,
        typename Combiner,
        typename Group,
        typename GroupCompare,
        typename _SlotFunction
    >
    NameServer<Event< _Signature,Combiner, Group, GroupCompare, _SlotFunction>*>
    Event<_Signature, Combiner, Group, GroupCompare, _SlotFunction>::nameserver;

}

#endif
