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


#ifndef EVENT_HPP
#define EVENT_HPP

#include "os/fosi.h"
#include <boost/signal.hpp>
#include <boost/call_traits.hpp>
#include "NameServerRegistrator.hpp"

#include "CompletionProcessor.hpp"
#include "TaskInterface.hpp"
#include "TaskExecution.hpp"


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
            boost::signals::connection c;
            boost::signals::connection c2;
            Handle( const boost::signals::connection & c_,
                    const boost::signals::connection & c2_) : c(c_), c2(c2_){}
            Handle( const boost::signals::connection & c_ ) : c(c_), c2(c_){}
            Handle( const Handle& h ) : c(h.c), c2(h.c2) {}
            Handle(){}

            Handle& operator=(const Handle& h) {
                c = h.c;
                c2 = h.c2;
                return *this;
            }

            bool operator==(const Handle& h) const {
                return (c == h.c) && (c2 == h.c2);
            }

            bool operator<(const Handle& h) const {
                return (c < h.c) && (c2 < h.c2);
            }

            bool connected() const {
                return c.connected() && c2.connected();
            }

            void disconnect() const {
                c.disconnect();
                c2.disconnect();
            }
        };

    using boost::function;

    /**
     * The Orocos Event is a wrapper around the boost::signal library
     * and extends its connection syntax with asynchronous event handling.
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
    public:
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

        explicit Event(const std::string name,
                       const Combiner& combiner = Combiner(),
                       const GroupCompare& group_compare = GroupCompare())
            : signal_type(combiner, group_compare),
              NameServerRegistrator<EventType*>(nameserver, name, this)
        {
        }


        explicit Event(const Combiner& combiner = Combiner(),
                       const GroupCompare& group_compare = GroupCompare()) :
            signal_type(combiner, group_compare)
        {
        }

        /**
         * @brief Connect a synchronous event slot to this event.
         */
        Handle connect(const SlotFunction& f)
        {
            return signal_type::connect( f );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, TaskInterface* task)
        {
            return Handle( task->thread()->connect( l, *this ) );
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, EventProcessor* ep)
        {
            return Handle( ep->connect( l, *this ) );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, TaskInterface* task)
        {
            return Handle( signal_type::connect( l ), task->thread()->connect( c, *this ) );
        }

        /**
         * @brief Connect a Synchronous and Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, const SlotFunction& c, EventProcessor* ep = CompletionProcessor::Instance() )
        {
            return Handle( signal_type::connect( l ), ep->connect( c, *this ) );
        }

        /**
         * This method is 100% equivalent to the operator() method
         * of boost::signal.
         */
        void fire() {
            signal_type::operator()();
        }

        template<class A1>
        void fire(const A1& a1) {
            signal_type::operator()(a1);
        }

        template<class A1, class A2>
        void fire(typename boost::call_traits<A1>::param_type a1,
                  typename boost::call_traits<A2>::param_type a2) {
                      signal_type::operator()(a1, a2);
                  }

        template<class A1, class A2, class A3>
        void fire(typename boost::call_traits<A1>::param_type a1,
                  typename boost::call_traits<A2>::param_type a2,
                  typename boost::call_traits<A3>::param_type a3) {
                      signal_type::operator()(a1, a2, a3);
                  }

        template<class A1, class A2, class A3, class A4>
        void fire(typename boost::call_traits<A1>::param_type a1,
                  typename boost::call_traits<A2>::param_type a2,
                  typename boost::call_traits<A3>::param_type a3,
                  typename boost::call_traits<A4>::param_type a4) {
                      signal_type::operator()(a1, a2, a3, a4);
                  }

        template<class A1, class A2, class A3, class A4, class A5>
        void fire(typename boost::call_traits<A1>::param_type a1,
                  typename boost::call_traits<A2>::param_type a2,
                  typename boost::call_traits<A3>::param_type a3,
                  typename boost::call_traits<A4>::param_type a4,
                  typename boost::call_traits<A5>::param_type a5) {
                      signal_type::operator()(a1, a2, a3, a4, a5);
                  }

        template<class A1, class A2, class A3, class A4, class A5, class A6>
        void fire(typename boost::call_traits<A1>::param_type a1,
                  typename boost::call_traits<A2>::param_type a2,
                  typename boost::call_traits<A3>::param_type a3,
                  typename boost::call_traits<A4>::param_type a4,
                  typename boost::call_traits<A5>::param_type a5,
                  typename boost::call_traits<A6>::param_type a6) {
                      signal_type::operator()(a1, a2, a3, a4, a5, a6);
                  }

        // repeat for A4, A5,...

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
