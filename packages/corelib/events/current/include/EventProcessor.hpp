/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  EventProcessor.hpp 

                        EventProcessor.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ORO_EVENTPROCESSOR_HPP
#define ORO_EVENTPROCESSOR_HPP

#include <os/RunnableInterface.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <os/MutexLock.hpp>
#include <vector>
#include "DataObjectInterfaces.hpp"
#include "boost/tuple/tuple.hpp"

namespace ORO_CoreLib
{
    namespace detail {
        struct EventCatcher {
            virtual ~EventCatcher() {}
            virtual void complete() = 0;

            boost::signals::connection h;

            boost::signals::connection getConnection() const
            {
                return h;
            }

        };

        template<int, class SignalType, class ContainerType>
        struct EventCatcherImpl;

        /**
         * Catch the event, dispatch later to F.
         */
        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<0, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            const Function f;

            EventCatcherImpl(const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                // What happens if a h.disconnect() or this connect occurs when the
                // event is fired ? This will probably lead to corruption,
                // since boost::signal is not thread-safe in itself. We
                // do wrap h in a handle object which could block during fire if
                // Handle had a pointer to Event...

                // Call the underlying connect of Event (to avoid recursion!) :
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<0, SignalType, ContainerType>::handler,
                                                                       this) );
            }

            Result handler( void ) {
                work = true;
                return Result();
            }

            virtual void complete() {
                if (!work)
                    return;
                f();
                work = false;
            }

            bool work;
        };

        template<class SignalType, class ContainerType >
        struct EventCatcherImpl<1, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename ContainerType::Data<typename Function::arg1_type> _a1;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<1, SignalType, ContainerType>::handler,
                                                                       this, _1) );
            }

            Result handler( typename Function::arg1_type& a1 ) {
                // the container decides if a1 needs to be stored
                _a1 = a1;
                return Result();
            }

            virtual void complete() {
                if ( !_a1 )
                    return;
                f( _a1 );
            }
        };

        struct OnlyFirstCont
        {
            template< class T>
            struct Data
            {
                Data() : work(false) {}
                bool work;
                T    val;
                typedef T type;
                operator bool() const {
                    return work;
                }
                operator T() const {
                    return val;
                }
                void operator=(T& t) {
                    if (work)
                        return;
                    val = t;
                    work = true;
                }
                void clear() {
                    work = false;
                }
            };
        };

        struct OnlyLastCont
        {
            template< class T>
            struct Data
            {
                Data() : work(false), val("EventData") {}
                bool work;
                DataObjectLockFree<T> val;
                typedef T type;
                operator bool() const {
                    return work;
                }
                void operator=(T& t) {
                    val.Set(t);
                    work = true;
                }
                operator T() const {
                    return val.Get();
                }
                void clear() {
                    work = false;
                }
            };
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<2, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type> Args;
            typename ContainerType::Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<2, SignalType, ContainerType>::handler,
                                                                       this, _1, _2) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2 ) {
                args = make_tuple( a1, a2 );
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args), get<1>(args) );
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<3, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type> Args;
            typename ContainerType::Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<3, SignalType, ContainerType>::handler,
                                                                       this, _1, _2, _3) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3 ) {
                args = make_tuple( a1, a2, a3 );
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args), get<1>(args), get<2>(args) );
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<4, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            typename Function::arg4_type _a4;
            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type,
                                 typename Function::arg4_type> Args;
            typename ContainerType::Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<4, SignalType, ContainerType>::handler,
                                                                       this, _1, _2, _3, _4) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4 ) {
                args = make_tuple( a1, a2, a3, a4 );
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args), get<1>(args), get<2>(args), get<3>(args) );
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<5, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            typename Function::arg4_type _a4;
            typename Function::arg5_type _a5;
            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type,
                                 typename Function::arg4_type,
                                 typename Function::arg5_type> Args;
            typename ContainerType::Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<5, SignalType, ContainerType>::handler,
                                                                       this, _1, _2, _3, _4, _5) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4,
                            typename Function::arg5_type& a5) {
                args = make_tuple( a1, a2, a3, a4, a5 );
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args), get<1>(args), get<2>(args), get<3>(args), get<4>(args) );
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<6, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type,
                                 typename Function::arg4_type,
                                 typename Function::arg6_type,
                                 typename Function::arg5_type> Args;
            typename ContainerType::Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<6, SignalType, ContainerType>::handler,
                                                                       this, _1, _2, _3, _4, _5, _6) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4,
                            typename Function::arg5_type& a5,
                            typename Function::arg6_type& a6) {
                args = make_tuple( a1, a2, a3, a4, a5, a6 );
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args), get<1>(args), get<2>(args), get<3>(args), get<4>(args), get<5>(args) );
            }
        };
    }

    /**
     * An Asynchronous Event Processor, which catches events and executes
     * the asynchronous callbacks in its RunnableInterface::step(). This class
     * is mostly used internally by Orocos, but users can use it to process
     * asynchronous callbacks in their own implementation. The EventProcessor
     * must be given as an argument in the Event's connect method.
     *
     * @see TaskExecution, CompletionProcessor
     */
    class EventProcessor
        : public ORO_OS::RunnableInterface
    {
        typedef std::vector<detail::EventCatcher*> List;
        List catchers;
        ORO_OS::Mutex m;
    public:
        EventProcessor() {
            // TODO define MAX_ASYN_EVENTS
            catchers.reserve(128);
        }

        ~EventProcessor() {
            // TODO All slots are only deleted on destruction. Disconnected
            // slots remain in the list. We could fix this by signal connection tracking
            // and/or defining our own asyn connection object.
            for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
                delete *it;
        }

        bool initialize() { return true;}

        void step() {
            if ( catchers.empty() )
                return;
            ORO_OS::MutexLock lock(m);
            for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
                (*it)->complete();
        }

        void finalize() {
        }


        /**
         * For Asynchronous callbacks, this enum defines
         * how the arguments are stored in case of an
         * overrun, ie, when the event is fired multiple times,
         * before the asynchronous callback can be called.
         * 
         */
        enum AsynStorageType {
            OnlyFirst, /** < Only call the callback once with the first fire() call's arguments */
            OnlyLast   /** < Only call the callback once with the last fire() call's arguments */
        };

        template<class SignalType>
        boost::signals::connection connect(const typename SignalType::SlotFunction& f, SignalType& sig, AsynStorageType t )
        {
            detail::EventCatcher* eci;
            switch ( t ) {
            case OnlyFirst:
                // Use function arity to select implementation :
                eci = new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyFirstCont>(f, sig);
                break;
            case OnlyLast:
                // Use function arity to select implementation :
                eci = new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyLastCont>(f, sig);
                break;
            }
            {
                ORO_OS::MutexLock lock(m);
                catchers.push_back( eci );
            }
            return eci->getConnection();
        }

    };
}

#endif
