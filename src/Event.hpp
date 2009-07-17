/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:49:58 CEST 2002  Event.h

                       Event.hpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CORELIB_EVENT_HPP
#define ORO_CORELIB_EVENT_HPP

#include "Signal.hpp"
#include "Handle.hpp"
#include "LocalEvent.hpp"
#include <boost/call_traits.hpp>
#include <boost/function.hpp>
#include "NameServerRegistrator.hpp"
#include "Invoker.hpp"

#include "Logger.hpp"
#include "EventProcessor.hpp"
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
     * Emit (call,raise, fire) the event by writing:
     * @code my_event(a, b, c);
     @endcode
     *
     * @param SignatureT The function type signature of the callback functions
     * this event will call. For example void(int, double) if you want the
     * event to call a function 'void foo(int x, double y)';
     * @see The Orocos CoreLib manual for usage.
     * @ingroup CoreLibEvents
     * @ingroup RTTComponentInterface
     * @todo Remove NameServer functionality, as this is deprecated by the EventService
     * class.
     */
    template<
        typename SignatureT // function type R (T1, T2, ..., TN)
    >
    class Event
        : public detail::InvokerSignature<boost::function_traits<SignatureT>::arity,
                                          SignatureT,
                                          boost::shared_ptr< detail::EventBase<SignatureT> > >,
          private NameServerRegistrator<Event<SignatureT>*>
    {
        typedef SignatureT FunctionT;
        std::string mname;
        typedef boost::shared_ptr< detail::EventBase<FunctionT> > EventBasePtr;
        typedef detail::InvokerSignature<boost::function_traits<FunctionT>::arity,
                                         FunctionT,
                                         EventBasePtr > Base;
    public:
       /**
        * @see EventProcessor::AsynStorageType
        */
        typedef EventProcessor::AsynStorageType AsynStorageType;

        typedef boost::function<SignatureT> SlotFunction;

        typedef Event<SignatureT> EventType;

        typedef SignatureT Signature;

        /**
         * The result type of the function signature.
         */
        typedef typename boost::function_traits<SignatureT>::result_type result_type;

        /**
         * Create a named Synchronous/Asynchronous Event.
         */
        explicit Event(const std::string name )
            : Base( EventBasePtr(new detail::LocalEvent<Signature>()) ),
              mname(name)
        {
            Logger::log() << Logger::Debug << "Event Created with name  : "<< name << Logger::endl;
        }

        /**
         * Create a Synchronous/Asynchronous Event.
         */
        Event()
        {
        }

        /**
         * Event objects may be copied.
         *
         * @param m the original
         */
        Event(const Event& m)
            : Base(m),
              mname(m.mname)
        {}

        /**
         * Event objects may be assigned
         *
         * @param m the original
         *
         * @return *this
         */
        Event& operator=(const Event& m)
        {
            if ( this == &m )
                return *this;
            mname = m.mname;
            this->impl = m.impl;
            return *this;
        }

        /**
         * Initialise a nameless Event object from an implementation.
         *
         * @param implementation The implementation which is acquired
         * by the Event object. If it has the wrong type, it is freed.
         */
        Event(boost::shared_ptr<ActionInterface> implementation)
            : Base( boost::dynamic_pointer_cast< detail::EventBase<Signature> >(implementation) ),
              mname()
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct Event from incompatible type."<< endlog();
            }
        }

        /**
         * Event objects may be assigned to an implementation.
         *
         * @param implementation the implementation, if it is not suitable,
         * it is freed.
         *
         * @return *this;
         */
        Event& operator=(boost::shared_ptr<ActionInterface> implementation)
        {
            if (this->impl == implementation)
                return *this;
            this->impl = boost::dynamic_pointer_cast< detail::EventBase<Signature> >(implementation);
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to assign Event '"<< mname <<"' from incompatible type."<< endlog();
            }
            return *this;
        }

        /**
         * Check if this Event is ready for execution.
         *
         * @return true if so.
         */
        bool ready() const {
            return this->impl;
        }

        /**
         * Returns the name of this event, given upon construction.
         */
        const std::string& getName() const { return mname; }

        /**
         * Return the arity of this event.
         */
        int arity() const { return this->impl ? this->impl->arity() : -1; }

        /**
         * @brief Connect a synchronous event slot to this event.
         */
        Handle connect(const SlotFunction& f)
        {
            return this->impl ? this->impl->connect( f ) : Handle();
        }

        /**
         * @brief Connect an Asynchronous event slot to this event.
         */
        Handle connect( const SlotFunction& l, EventProcessor* ep, AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->impl ? ep->connect( l, *this, t ) : Handle();
        }

        /**
         * @brief Setup a synchronous event slot to this event.
         */
        Handle setup(const SlotFunction& f)
        {
            return this->impl ? this->impl->setup( f ) : Handle();
        }

        /**
         * @brief Setup an Asynchronous event slot to this event.
         */
        Handle setup( const SlotFunction& l, EventProcessor* ep, AsynStorageType t = EventProcessor::OnlyFirst)
        {
            return this->impl ? ep->setup( l, *this, t ) : Handle();
        }

        EventBasePtr getImplementation() const { return this->impl; }

        /**
         * @brief Public nameserver for Events.
         * @see also EventService for a 'type-less'/universal
         * Event nameserver.
         */
        static NameServer<EventType*> nameserver;
    };


    template<
        typename SignatureT
    >
    NameServer<Event<SignatureT>*>
    Event<SignatureT>::nameserver;

}

#endif
