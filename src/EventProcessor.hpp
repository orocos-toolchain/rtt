/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  EventProcessor.hpp

                        EventProcessor.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
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


#ifndef ORO_EVENTPROCESSOR_HPP
#define ORO_EVENTPROCESSOR_HPP

#include "RunnableInterface.hpp"
#include "Signal.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "DataObjectInterfaces.hpp"
#include "List.hpp"
#include "boost/tuple/tuple.hpp"
#include "NA.hpp"
#include "os/Atomic.hpp"

namespace RTT
{
    class EventProcessor;

    namespace detail {
        using boost::make_tuple;

        struct RTT_API EventCatcher {
            EventCatcher();
            virtual ~EventCatcher();
            virtual void complete() = 0;

            /**
             * Calls mep->getActivity()->trigger()
             * and increases work count.
             */
            void signalWork();

            /**
             * Decreases work count.
             */
            void signalWorkDone();

            /**
             * EventProcessor is used to signal the destruction
             * of this EventCatcher.
             */
            EventProcessor* mep;

            /**
             * Boost Intrusive pointer reference count.
             */
            int refCount;

            // do not accept 'work'
            bool enabled;

            typedef boost::intrusive_ptr< EventCatcher > shared_ptr;

        };

        RTT_API void intrusive_ptr_add_ref( EventCatcher* p );
        RTT_API void intrusive_ptr_release( EventCatcher* p );

        using boost::tuples::get;

        template<int, class SignalType, class ContainerType>
        struct EventCatcherImpl;

        /**
         * Catch the event, dispatch later to F.
         */
        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<0, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<0, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            const Function f;
            bool work;

            EventCatcherImpl(const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
            }

            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<0, SignalType, ContainerType>::handler,
                                                                        shared_ptr(this)) );
            }

            Result handler( void ) {
                work = this->enabled;
                if ( work && mep ) // if enabled and mep, there is an getActivity() present.
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if (!work)
                    return;
                f();
                work = false;
                signalWorkDone();
            }
        };

        template<class SignalType, class ContainerType >
        struct EventCatcherImpl<1, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<1, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename ContainerType::template Data<typename Function::arg1_type> _a1;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
            }
            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<1, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1) );
            }

            Result handler( typename Function::arg1_type a1 ) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                // the container decides if a1 needs to be stored
                _a1 = a1;
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !_a1 )
                    return;
                f( _a1.val() );
                _a1.clear();
                signalWorkDone();
            }
        };

        struct OnlyFirstCont
        {
            template< class T>
            struct Data
            {
                Data() : work(false) {}
                bool work;
                T    val_;
                typedef T type;
                operator bool() const {
                    return work;
                }
                T val() const {
                    return val_;
                }
                void operator=(const T& t) {
                    if (work)
                        return;
                    val_ = t;
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
                Data() : tag(0), tagged(0), val_("EventData") {}
                volatile int tag;
                mutable int tagged;
                DataObjectLockFree<T> val_;
                typedef T type;
                operator bool() const {
                	/*Check for work.*/
                    return (tagged != tag);
                }
                void operator=(const T& t) {
                    val_.Set(t);
                    tag++;
                }
                T val() const {
                	tagged = tag;
                    return val_.Get();
                }
                void clear() {
                    if(OS::CAS(&tag, tagged, tagged+1))
                    {
                    	/*Try to clear work. If pre-empted here by operator=() tag will be incremented toO.*/
                    	tagged++;
                    }
                }
            };
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<2, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<2, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type> Args;
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {}
            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<2, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2 ) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                args = boost::make_tuple( a1, a2 );
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !args )
                    return;
                Args tmp_args = args.val();
                f( get<0>(tmp_args), get<1>(tmp_args) );
                args.clear();
                signalWorkDone();
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<3, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<3, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type> Args;
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {}

            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<3, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3 ) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                args = boost::make_tuple( a1, a2, a3 );
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !args )
                    return;
                Args tmp_args = args.val();
                f( get<0>(tmp_args), get<1>(tmp_args), get<2>(tmp_args) );
                args.clear();
                signalWorkDone();
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<4, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<4, SignalType, ContainerType> > shared_ptr;
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
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
            }
            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<4, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4 ) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                args = make_tuple( a1, a2, a3, a4 );
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !args )
                    return;
                Args tmp_args = args.val();
                f( get<0>(tmp_args), get<1>(tmp_args), get<2>(tmp_args), get<3>(tmp_args) );
                args.clear();
                signalWorkDone();
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<5, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<5, SignalType, ContainerType> > shared_ptr;
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
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
            }

            Handle setup( SignalType& sig ) {
                return sig.setup( boost::bind( &EventCatcherImpl<5, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4, _5) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4,
                            typename Function::arg5_type a5) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                args = make_tuple( a1, a2, a3, a4, a5 );
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !args )
                    return;
                Args tmp_args = args.val();
                f( get<0>(tmp_args), get<1>(tmp_args), get<2>(tmp_args), get<3>(tmp_args), get<4>(tmp_args) );
                args.clear();
                signalWorkDone();
            }
        };

        template<class SignalType, class ContainerType>
        struct EventCatcherImpl<6, SignalType, ContainerType>
            : public EventCatcher
        {
            typedef boost::intrusive_ptr< EventCatcherImpl<6, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type,
                                 typename Function::arg3_type,
                                 typename Function::arg4_type,
                                 typename Function::arg5_type,
                                 typename Function::arg6_type> Args;
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_)
            {
            }

            Handle setup( SignalType& sig )
            {
                return sig.setup( boost::bind( &EventCatcherImpl<6, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4, _5, _6) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4,
                            typename Function::arg5_type a5,
                            typename Function::arg6_type a6) {
                if ( !this->enabled )
                    return detail::NA<Result>::na();
                args = make_tuple( a1, a2, a3, a4, a5, a6 );
                if ( mep )
                    signalWork();
                return detail::NA<Result>::na();
            }

            virtual void complete() {
                if ( !args )
                    return;
                Args tmp_args = args.val();
                f( get<0>(tmp_args), get<1>(tmp_args), get<2>(tmp_args), get<3>(tmp_args), get<4>(tmp_args), get<5>(tmp_args) );
                args.clear();
                signalWorkDone();
            }
        };
    }

    /**
     * An Asynchronous Event Processor, which catches events and executes
     * the asynchronous callbacks in its RunnableInterface::step(). This class
     * is mostly used internally by Orocos, but users can use it to process
     * asynchronous callbacks in their own implementation. The EventProcessor
     * is an argument in the Event's asynchronous connect method.
     *
     * @par Changing the Event Processing Policy.
     *
     * The default policy of the EventProcessor is to process all asynchronous
     * callbacks in step().
     * In order to change the processing of the asynchronous events,
     * subclass this class and override step() or other virtual functions.
     * The BlockingEventProcessor is an example of an EventProcessor with
     * a different policy, and can be subclassed in turn.
     *
     * @see CompletionProcessor
     * @ingroup CoreLibEvents
     * @ingroup Processor
     */
    class RTT_API EventProcessor
        : public RunnableInterface
    {
    protected:
        friend class EventCatcher;
        /**
         * The EC is released when the connection it is used in is
         * deleted *and* it is removed from this vector.
         */
        typedef List<detail::EventCatcher*> ECList;
        ECList catchers;

        friend struct detail::EventCatcher;
        void destroyed( detail::EventCatcher* ec );

        OS::AtomicInt has_work;
    public:

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

        /**
         * Create a periodic EventProcessor
         */
        EventProcessor();

        ~EventProcessor();

        bool initialize();

        void step();

        void finalize();

        /**
         * Force the loop() method to return.
         */
        bool breakLoop();

        bool hasWork();

        /**
         * Connect a function to an Event and process upon each event the function in this
         * event processor. The returned handle holds the connection between \a f and \a sig.
         * @param f will be called within this EventProcessor when \a sig is emitted.
         * @param sig The Event to which f will react.
         * @param t specifies the policy in case of over-runs. That is, when \a sig
         * is emitted multiple times before \a f could be called.
         * @return An connected handle. Call Handle::disconnect() upon this object in order
         * to remove the link between \a f and \a sig.
         */
        template<class SignalType>
        Handle connect(const typename SignalType::SlotFunction& f, SignalType& sig, AsynStorageType t )
        {
            Handle h = this->setup( f, sig, t);
            h.connect();
            return h;
        }

        /**
         * Setup the processing of an asynchronous event. The returned handle does not
         * yet connect \a f and \a sig. Use Handle::connect().
         * @param f will be called within this EventProcessor when \a sig is emitted.
         * @param sig The Event to which f will react.
         * @param t specifies the policy in case of over-runs. That is, when \a sig
         * is emitted multiple times before \a f could be called.
         * @return An unconnected handle. Call Handle::connect() upon this object in order
         * to activate the link between \a f and \a sig.
         */
        template<class SignalType>
        Handle setup(const typename SignalType::SlotFunction& f, SignalType& sig, AsynStorageType t )
        {
            detail::EventCatcher::shared_ptr eci;
            Handle h;
            switch ( t ) {
            case OnlyFirst:
                {
                    // Use function arity to select implementation :
                    typename detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyFirstCont>::shared_ptr ecf
                        (new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyFirstCont>(f, sig));
                    h = ecf->setup( sig );
                    eci = ecf;
                }
                break;
            case OnlyLast:
                {
                    // Use function arity to select implementation :
                    typename detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyLastCont>::shared_ptr ecl
                        (new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyLastCont>(f, sig));
                    h = ecl->setup( sig );
                    eci = ecl;
                }
                break;
            }
            catchers.grow();
            catchers.append( eci.get() );
            eci->enabled = this->getActivity() && this->getActivity()->isActive();
            eci->mep = this;
            return h;
        }

    };

    /**
     * The Blocking EventProcessor, extended with a blocking implementation, waiting
     * for one Event to complete in its \a loop().
     */
    class RTT_API BlockingEventProcessor
        : public EventProcessor
    {
    public:
        /**
         * Create a blocking (non periodic) EventProcessor, which will trigger()
         * the activity if an event needs processing.
         */
        BlockingEventProcessor();

        ~BlockingEventProcessor();

    };

}

#endif
