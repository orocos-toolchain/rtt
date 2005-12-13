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

#include "RunnableInterface.hpp"
#include "Signal.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <os/MutexLock.hpp>
#include <os/Semaphore.hpp>
#include <vector>
#include "DataObjectInterfaces.hpp"
#include "boost/tuple/tuple.hpp"

namespace ORO_CoreLib
{
    class EventProcessor;

    namespace detail {
        using ORO_OS::Semaphore;
        using boost::make_tuple;

        struct EventCatcher {
            EventCatcher(Semaphore* s);
            virtual ~EventCatcher();
            virtual void complete() = 0;

            EventProcessor* mep;
            // optionally signal this semaphore in non blocking mode
            Semaphore* sem;
            // if the catcher is not enabled ( EventProcessor not running)
            // do not accept 'work'
            bool enabled;

            int refCount;

            typedef boost::intrusive_ptr< EventCatcher > shared_ptr;
            
        };

        void intrusive_ptr_add_ref( EventCatcher* p );
        void intrusive_ptr_release( EventCatcher* p );

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

            EventCatcherImpl(const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_), work(false)
            {
            }
            
            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<0, SignalType, ContainerType>::handler,
                                                                        shared_ptr(this)) );
            }

            Result handler( void ) {
                work = this->enabled;
                if ( sem && work )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if (!work)
                    return;
                f();
                work = false;
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

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {
            }
            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<1, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1) );
            }

            Result handler( typename Function::arg1_type a1 ) {
                if ( !this->enabled )
                    return Result();
                // the container decides if a1 needs to be stored
                _a1 = a1;
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !_a1 )
                    return;
                f( _a1.val() );
                _a1.clear();
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
                Data() : work(false), val_("EventData") {}
                bool work;
                DataObjectLockFree<T> val_;
                typedef T type;
                operator bool() const {
                    return work;
                }
                void operator=(const T& t) {
                    val_.Set(t);
                    work = true;
                }
                T val() const {
                    return val_.Get();
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
            typedef boost::intrusive_ptr< EventCatcherImpl<2, SignalType, ContainerType> > shared_ptr;
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typedef boost::tuple<typename Function::arg1_type,
                                 typename Function::arg2_type> Args;
            typename ContainerType::template Data< Args > args;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {}
            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<2, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2 ) {
                if ( !this->enabled )
                    return Result();
                args = make_tuple( a1, a2 );
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args.val()), get<1>(args.val()) );
                args.clear();
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

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {}

            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<3, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3 ) {
                if ( !this->enabled )
                    return Result();
                args = make_tuple( a1, a2, a3 );
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args.val()), get<1>(args.val()), get<2>(args.val()) );
                args.clear();
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

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {
            }
            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<4, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4 ) {
                if ( !this->enabled )
                    return Result();
                args = make_tuple( a1, a2, a3, a4 );
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args.val()), get<1>(args.val()), get<2>(args.val()), get<3>(args.val()) );
                args.clear();
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

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {
            }

            sigslot::handle setup( SignalType& sig ) {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<5, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4, _5) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4,
                            typename Function::arg5_type a5) {
                if ( !this->enabled )
                    return Result();
                args = make_tuple( a1, a2, a3, a4, a5 );
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args.val()), get<1>(args.val()), get<2>(args.val()), get<3>(args.val()), get<4>(args.val()) );
                args.clear();
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

            EventCatcherImpl( const Function& f_, SignalType& sig, Semaphore* s )
                : EventCatcher(s), f(f_)
            {
            }

            sigslot::handle setup( SignalType& sig )
            {
                return sig.SignalType::signal_type::setup( boost::bind( &EventCatcherImpl<6, SignalType, ContainerType>::handler,
                                               shared_ptr(this), _1, _2, _3, _4, _5, _6) );
            }

            Result handler( typename Function::arg1_type a1,
                            typename Function::arg2_type a2,
                            typename Function::arg3_type a3,
                            typename Function::arg4_type a4,
                            typename Function::arg5_type a5,
                            typename Function::arg6_type a6) {
                if ( !this->enabled )
                    return Result();
                args = make_tuple( a1, a2, a3, a4, a5, a6 );
                if ( sem )
                    sem->signal();
                return Result();
            }

            virtual void complete() {
                if ( !args )
                    return;
                f( get<0>(args.val()), get<1>(args.val()), get<2>(args.val()), get<3>(args.val()), get<4>(args.val()), get<5>(args.val()) );
                args.clear();
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
     * @see TimerThread, CompletionProcessor
     */
    class EventProcessor
        : public RunnableInterface
    {
    protected:
        /**
         * The EC is released when the connection it is used in is
         * deleted *and* it is removed from this vector.
         */
        typedef std::vector<detail::EventCatcher*> List;
        List catchers;
        mutable ORO_OS::MutexRecursive m;
        boost::shared_ptr<ORO_OS::Semaphore> sem;
        bool active;
        /**
         *  Used by derived classes.
         */
        EventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s);

        friend class detail::EventCatcher;
        void destroyed( detail::EventCatcher* ec );

    public:
        /**
         * Create a periodic EventProcessor
         */
        EventProcessor();

        ~EventProcessor();

        bool initialize();

        void step();

        void finalize();

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
         * Connect a function to a signal and process upon each event the function in this
         * event processor.
         */
        template<class SignalType>
        sigslot::handle connect(const typename SignalType::SlotFunction& f, SignalType& sig, AsynStorageType t )
        {
            sigslot::handle h = this->setup( f, sig, t);
            h.connect();
            return h;
        }

        template<class SignalType>
        sigslot::handle setup(const typename SignalType::SlotFunction& f, SignalType& sig, AsynStorageType t )
        {
            detail::EventCatcher::shared_ptr eci;
            sigslot::handle h;
            switch ( t ) {
            case OnlyFirst:
                {
                    // Use function arity to select implementation :
                    typename detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyFirstCont>::shared_ptr ecf
                        (new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyFirstCont>(f, sig, sem.get()));
                    h = ecf->setup( sig );
                    eci = ecf;
                }
                break;
            case OnlyLast:
                {
                    // Use function arity to select implementation :
                    typename detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyLastCont>::shared_ptr ecl
                        (new detail::EventCatcherImpl<SignalType::SlotFunction::arity, SignalType, detail::OnlyLastCont>(f, sig, sem.get()));
                    h = ecl->setup( sig );
                    eci = ecl;
                }
                break;
            }
            {
                ORO_OS::MutexLock lock(m);
                catchers.push_back( eci.get() );
            }
            eci->enabled = this->active;
            eci->mep = this;
            return h;
        }

    };

    /**
     * The Blocking EventProcessor, extended with a blocking implementation, waiting
     * for one Event to complete in its \a loop().
     */
    class BlockingEventProcessor
        : public EventProcessor
    {
    protected:
        ORO_OS::Mutex breaker;
    public:
        /**
         * Create a blocking (non periodic) EventProcessor, which will wait on ORO_OS::Semaphore  \a s.
         * All connected Events will signal this semaphore if an Event needs 
         * processing. Also, signal this semaphore to break loop(), or call breakLoop().
         * The semaphore is shared through a shared_ptr, meaning that it will only delete
         * \a s if it holds the last reference to it.
         */
        BlockingEventProcessor( boost::shared_ptr<ORO_OS::Semaphore> s = boost::shared_ptr<ORO_OS::Semaphore>( new ORO_OS::Semaphore(0) ) );

        ~BlockingEventProcessor();

        bool initialize();

        /**
         * Process Events until \a breakLoop() (or destructor) is called.
         */
        void loop();

        /**
         * Force the loop() method to return.
         */
        bool breakLoop();

        /**
         * Let all registered (and future) connections use
         * ORO_OS::Semaphore \a s to signal work to be done.
         */
        void setSemaphore( boost::shared_ptr<ORO_OS::Semaphore> s);
    };

}

#endif
