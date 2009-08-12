/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  EventService.hpp

                        EventService.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef EVENT_SERVICE_HPP
#define EVENT_SERVICE_HPP

#include "../Logger.hpp"
#include "../internal/FunctorFactory.hpp"
#include "../internal/OperationFactory.hpp"
#include "../internal/EventHook.hpp"
#include "../Event.hpp"
#include "../internal/EventC.hpp"
#include "../internal/ConnectionC.hpp"
#include "../internal/DataSourceArgsEvent.hpp"
#include <boost/static_assert.hpp>
#include <boost/type_traits/function_traits.hpp>

namespace RTT
{

    class ExecutionEngine;

    /**
     * The EventService represents the event interface. It stores pointers
     * to event objects and allows clients to retrieve event objects which
     * can then be emitted or subscribed to.
     *
     */
    class RTT_API EventService
        : public OperationFactory< ActionInterface* >
    {
        // creates event hooks
        typedef std::map<std::string, detail::FunctorFactoryPart<detail::EventHookBase*>* > Hooks;
        Hooks mhooks;
        // creates emittor objects
        typedef std::map<std::string, boost::shared_ptr<ActionInterface> > Events;
        Events mevents;

        ExecutionEngine* eeproc;
        EventProcessor* eproc;
    public:
        typedef OperationFactory< ActionInterface* > Factory;

        /**
         * Create an EventService with an associated ExecutionEngine.
         * If you want the owner task of this object to process an event use
         * EventService::getEventProcessor() in the \a setup functions below.
         */
        EventService( ExecutionEngine* ee );

        /**
         * Create an EventService with an associated EventService.
         * The EventProcessor is optional and defaults to the CompletionProcessor.
         * If you want the owner task of this object to process an event use
         * EventService::getEventProcessor() in the \a setup functions below.
         */
        EventService( EventProcessor* ep = 0 );

        ~EventService();

        EventProcessor* getEventProcessor();

        void setEventProcessor(EventProcessor* ep);

        /**
         * Clear all added methods from the repository, saving memory space.
         */
        void clear();

        /**
         * Returns the names of all events added to this interface.
         * @see getNames() to get a list of all events available to scripting.
         */
        std::vector<std::string> getEvents() const;

        /**
         * Add an Event to the event interface. This version
         * of addEvent only adds \a e to the C++ interface and
         * not to the scripting interface.
         *
         * @param e The event to add
         *
         * @return True if the event's name was unique
         * and could be added.
         */
        template< class EventT>
        bool addEvent( EventT* e ) {
            Logger::In in("EventService");
            // All addEvent functions below first call this function.
            // Hence all these checks are always performed.
            std::string ename = e->getName();
            if ( ename.empty() || e->ready() == false ) {
                log(Error) << "Can not use addEvent with uninitialised Event: give your event a name upon construction." << endlog();
                return false;
            }

            if ( mevents.count(ename) != 0 ) {
                log(Error) << "Can not add Event '"<<ename<<"': name already used." << endlog();
                return false;
            }

            mevents[ename] = e->getImplementation();
            return true;
        }

        /**
         * Get a previously added event for use in a C++ Event object. Store the result of this
         * method in an Event<\a Signature> object.
         * @param Signature Signature of the Event, for example void(int, int)
         * @param ename The name of the event to lookup.
         * @return A shared pointer which is to be assigned to an Event object.
         * @internal We need Signature for two reasons: 1. for consistency with the general
         * getPrimitive<T>(name) API in RTT and 2. because we may need the type to reconstruct the
         * object dynamically, for example, when the Event is remote. See the CommandRepository
         * for and example where this is necessary (see also RTT::RemoteCommand )
         */
        template<class Signature>
        boost::shared_ptr<ActionInterface> getEvent(const std::string& ename)
        {
            if ( mevents.count(ename) )
                return mevents[ename];
            return boost::shared_ptr<ActionInterface>();
        }

        /**
         * Add an Event to the event interface. This version of
         * addEvent adds the event \a e to the C++ interface and to the
         * scripting interface.
         * @param e An event which takes no arguments. It must be ready().
         * @param description The description of this event.
         * @return true if e->ready() and the name e->getName() was not yet
         * used in this EventService.
         */
        template< class EventT>
        bool addEvent( EventT* e, const char* description )
        {
            BOOST_STATIC_ASSERT( boost::function_traits<typename EventT::Signature>::arity == 0 );

            if ( this->addEvent( e ) == false)
                return false;

            this->add(e->getName(),
                      new detail::OperationFactoryPart0<ActionInterface*,
                      detail::DataSourceArgsEvent<typename EventT::Signature> >( boost::bind(&EventT::operator(),e), description) );

            this->mhooks[e->getName()]
                = new detail::FunctorFactoryPart0<detail::EventHookBase*, detail::EventHookGenerator<EventT> >( detail::EventHookGenerator<EventT>(e) );

            return true;
        }

        /**
         * Add an Event to the event interface. This version of
         * addEvent adds the event \a e to the C++ interface and to the
         * scripting interface.
         * @param e An event which takes one argument. It must be ready().
         * @param description The description of this event.
         * @param arg1 The name of the first argument of \a e.
         * @param arg1_description A description of the first argument of \a e.
         * @return true if e->ready() and the name e->getName() was not yet
         * used in this EventService.
         */
        template< class EventT>
        bool addEvent( EventT* e, const char* description,
                       const char* arg1, const char* arg1_description )
        {
            BOOST_STATIC_ASSERT( boost::function_traits<typename EventT::Signature>::arity == 1 );

            if ( this->addEvent( e ) == false)
                return false;
            this->add(e->getName(),
                      new detail::OperationFactoryPart1<ActionInterface*,
                      detail::DataSourceArgsEvent<typename EventT::Signature> >( boost::bind(&EventT::operator(),e, _1),
                      description,
                      arg1, arg1_description) );

            this->mhooks[e->getName()]
                = new detail::FunctorFactoryPart1<detail::EventHookBase*, detail::EventHookGenerator<EventT> >( detail::EventHookGenerator<EventT>(e) );

            return true;
        }

        /**
         * Add an Event to the event interface. This version of
         * addEvent adds the event \a e to the C++ interface and to the
         * scripting interface.
         * @param e An event which takes two arguments. It must be ready().
         * @param description The description of this event.
         * @param arg1 The name of the first argument of \a e.
         * @param arg1_description A description of the first argument of \a e.
         * @param arg2 The name of the second argument of \a e.
         * @param arg2_description A description of the second argument of \a e.
         * @return true if e->ready() and the name e->getName() was not yet
         * used in this EventService.
         */
        template< class EventT>
        bool addEvent( EventT* e, const char* description,
                       const char* arg1, const char* arg1_description,
                       const char* arg2, const char* arg2_description )
        {
            BOOST_STATIC_ASSERT( boost::function_traits<typename EventT::Signature>::arity == 2 );

            if ( this->addEvent( e ) == false)
                return false;
            this->add(e->getName(),
                      new detail::OperationFactoryPart2<ActionInterface*,
                      detail::DataSourceArgsEvent<typename EventT::Signature> >( boost::bind(&EventT::operator(),e,_1,_2),
                      description,
                      arg1, arg1_description,
                      arg2, arg2_description) );

            this->mhooks[e->getName()]
                = new detail::FunctorFactoryPart2<detail::EventHookBase*, detail::EventHookGenerator<EventT> >( detail::EventHookGenerator<EventT>(e) );

            return true;
        }

        /**
         * Add an Event to the event interface. This version of
         * addEvent adds the event \a e to the C++ interface and to the
         * scripting interface.
         * @param e An event which takes four arguments. It must be ready().
         * @param description The description of this event.
         * @param arg1 The name of the first argument of \a e.
         * @param arg1_description A description of the first argument of \a e.
         * @param arg2 The name of the second argument of \a e.
         * @param arg2_description A description of the second argument of \a e.
         * @param arg3 The name of the third argument of \a e.
         * @param arg3_description A description of the third argument of \a e.
         * @return true if e->ready() and the name e->getName() was not yet
         * used in this EventService.
         */
        template< class EventT>
        bool addEvent( EventT* e, const char* description,
                       const char* arg1, const char* arg1_description,
                       const char* arg2, const char* arg2_description,
                       const char* arg3, const char* arg3_description)
        {
            BOOST_STATIC_ASSERT( boost::function_traits<typename EventT::Signature>::arity == 3 );

            if ( this->addEvent( e ) == false)
                return false;
            this->add(e->getName(),
                      new detail::OperationFactoryPart3<ActionInterface*,
                      detail::DataSourceArgsEvent<typename EventT::Signature> >( boost::bind(&EventT::operator(),e,_1,_2,_3),
                      description,
                      arg1, arg1_description,
                      arg2, arg2_description,
                      arg3, arg3_description));

            this->mhooks[e->getName()]
                = new detail::FunctorFactoryPart3<detail::EventHookBase*, detail::EventHookGenerator<EventT> >( detail::EventHookGenerator<EventT>(e) );
            return true;
        }

        /**
         * Add an Event to the event interface. This version of
         * addEvent adds the event \a e to the C++ interface and to the
         * scripting interface.
         * @param e An event which takes four arguments. It must be ready().
         * @param description The description of this event.
         * @param arg1 The name of the first argument of \a e.
         * @param arg1_description A description of the first argument of \a e.
         * @param arg2 The name of the second argument of \a e.
         * @param arg2_description A description of the second argument of \a e.
         * @param arg3 The name of the third argument of \a e.
         * @param arg3_description A description of the third argument of \a e.
         * @param arg4 The name of the fourth argument of \a e.
         * @param arg4_description A description of the fourth argument of \a e.
         * @return true if e->ready() and the name e->getName() was not yet
         * used in this EventService.
         */
        template< class EventT>
        bool addEvent( EventT* e, const char* description,
                       const char* arg1, const char* arg1_description,
                       const char* arg2, const char* arg2_description,
                       const char* arg3, const char* arg3_description,
                       const char* arg4, const char* arg4_description )
        {
            BOOST_STATIC_ASSERT( boost::function_traits<typename EventT::Signature>::arity == 4 );

            if ( this->addEvent( e ) == false)
                return false;
            this->add(e->getName(),
                      new detail::OperationFactoryPart4<ActionInterface*,
                      detail::DataSourceArgsEvent<typename EventT::Signature> >( boost::bind(&EventT::operator(),e,_1,_2,_3,_4),
                      description,
                      arg1, arg1_description,
                      arg2, arg2_description,
                      arg3, arg3_description,
                      arg4, arg4_description));

            this->mhooks[e->getName()]
                = new detail::FunctorFactoryPart4<detail::EventHookBase*, detail::EventHookGenerator<EventT> >( detail::EventHookGenerator<EventT>(e) );
            return true;
        }


        /**
         * Query for the existence of an Event in this Service.
         * @see hasMember() to verify if an event is available to scripting as well.
         */
        bool hasEvent(const std::string& ename) const;

        /**
         * Return the number of arguments a given event has.
         * @retval -1 The event does not exist.
         * @return The number of arguments (may be zero).
         */
        int arity(const std::string& name) const;

        /**
         * Remove an added Event from this Service.
         */
        bool removeEvent( const std::string& ename );

        /**
         * Setup an object to emit events with arguments.
         * Use this method as in
         @verbatim
         createEmit("EventName").arg(2.0).arg(1.0).emit();
         // or:
         EventC em = createEmit("EventName").arg(2.0).arg(1.0);
         em.emit();
         @endverbatim
         * Also variables or reference to variables may be given
         * within arg().
         * @see EventC
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
        */
        EventC setupEmit(const std::string& ename) const;

        /**
         * Setup a ConnectionC object to connect a number of synchronous or asynchronous callbacks to an event.
         * Use this method as in
         @verbatim
         Handle h = setupConnection("EventName").callback( &my_function ).handle();
         h.connect();
         // or for adding a class method:
         h = setupConnection("EventName").callback( boost::bind( &MyClass::function, obj) ).handle();
         h.connect();
         @endverbatim
         * @see ConnectionC
         * @see Handle
         * @throw name_not_found_exception
         */
        ConnectionC setupConnection(const std::string& ename) const;

        /**
         * For internal use only. Setup a synchronous Event handler which will set \a args and
         * then call \a func synchronously when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param func  A function object which will be called.
         * @param args  The arguments which will be set before \a func is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         */
        Handle setupSyn(const std::string& ename,
                        boost::function<void(void)> func,
                        std::vector<DataSourceBase::shared_ptr> args ) const;

        /**
         * For internal use only. Setup a asynchronous Event handler which will set \a args and
         * call \a afunc asynchronously (in task \a t) when event \a ename occurs.
         * @param ename The name of the previously added Event.
         * @param afunc  A function object which will be called.
         * @param args  The arguments which will be set before \a afunc is called.
         * They must be of type \a AssignableDataSource<Tn> or \a DataSource<Tn&>,
         * where \a Tn is the type of the n'th argument of the Event.
         * @param t The task in which the \a args will be set and \a afunc will be called.
         * @param ep The EventProcessor in which the \a args will be set and \a afunc will be called.
         * @param s_type The method used when event overruns happen. By default, only the first event
         * is propagated to the callbacks.
         * @{
         */
        Handle setupAsyn(const std::string& ename,
                         boost::function<void(void)> afunc,
                         const std::vector<DataSourceBase::shared_ptr>& args,
                         ActivityInterface* t,
                         EventProcessor::AsynStorageType s_type = EventProcessor::OnlyFirst) const;

        Handle setupAsyn(const std::string& ename,
                         boost::function<void(void)> afunc,
                         const std::vector<DataSourceBase::shared_ptr>& args,
                         EventProcessor* ep = 0,
                         EventProcessor::AsynStorageType s_type = EventProcessor::OnlyFirst) const;
        //!@}

        /**
         * For internal use only. Get an event which takes its arguments by Datasource.
         * Call \a result->execute() to emit the event with the given \a args.
         *
         * @param ename The name of the previously added Event.
         * @param args  DataSources holding the values for each parameter of the event.
         * They are read at the moment of execute().
         */
        ActionInterface* getEvent(const std::string& ename,const std::vector<DataSourceBase::shared_ptr>& args) const;

    };


}

#endif
