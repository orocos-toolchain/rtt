#ifndef ORO_EVENTPROCESSOR_HPP
#define ORO_EVENTPROCESSOR_HPP

#include <os/RunnableInterface.hpp>
#include <os/MutexLock.hpp>
#include <boost/signal.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>

namespace ORO_CoreLib
{
    using boost::function;

        struct EventCatcher {
            virtual ~EventCatcher() {}
            virtual void complete() = 0;

            boost::signals::connection h;

            boost::signals::connection getConnection() const
            {
                return h;
            }

        };

    template<int, class SignalType>
    struct EventCatcherImpl;

        /**
         * Catch the event, dispatch later to F.
         */
        template<class SignalType>
        struct EventCatcherImpl<0, SignalType>
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
                // do wrap h in a handle object which could lock on fire if
                // Handle had a pointer to Event...

                // Call the underlying connect of Event (to avoid recursion!) :
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<0, SignalType>::handler,
                                                                       this) );
            }

            Result handler( void ) {
                ORO_OS::MutexLock locker( lock );
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f();
                work = false;
            }

            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<1, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<1, SignalType>::handler,
                                                                       this, _1) );
            }

            Result handler( typename Function::arg1_type& a1 ) {
                ORO_OS::MutexLock locker( lock );
                _a1 = a1; // save
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<2, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<2, SignalType>::handler,
                                                                       this, _1, _2) );
            }

            Result handler( typename Function::arg1_type& a1, typename Function::arg2_type& a2 ) {
                ORO_OS::MutexLock locker( lock );
                //f_nullary = bind(f, a1, a2);
                _a1 = a1; _a2 = a2;
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1, _a2 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<3, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<3, SignalType>::handler,
                                                                       this, _1, _2, _3) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3 ) {
                ORO_OS::MutexLock locker( lock );
                _a1 = a1; _a2 = a2; _a3 = a3;
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1, _a2, _a3 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<4, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            typename Function::arg4_type _a4;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<4, SignalType>::handler,
                                                                       this, _1, _2, _3, _4) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4 ) {
                ORO_OS::MutexLock locker( lock );
                _a1 = a1; _a2 = a2; _a3 = a3; _a4 = a4;
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1, _a2, _a3, _a4 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<5, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            typename Function::arg4_type _a4;
            typename Function::arg5_type _a5;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<5, SignalType>::handler,
                                                                       this, _1, _2, _3, _4, _5) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4,
                            typename Function::arg5_type& a5) {
                ORO_OS::MutexLock locker( lock );
                _a1 = a1; _a2 = a2; _a3 = a3; _a4 = a4; _a5 = a5;
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1, _a2, _a3, _a4, _a5 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

        template<class SignalType>
        struct EventCatcherImpl<6, SignalType>
            : public EventCatcher
        {
            typedef typename SignalType::SlotFunction Function;
            typedef typename Function::result_type Result;

            typename Function::arg1_type _a1;
            typename Function::arg2_type _a2;
            typename Function::arg3_type _a3;
            typename Function::arg4_type _a4;
            typename Function::arg5_type _a5;
            typename Function::arg6_type _a6;
            Function f;

            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<6, SignalType>::handler,
                                                                       this, _1, _2, _3, _4, _5, _6) );
            }

            Result handler( typename Function::arg1_type& a1,
                            typename Function::arg2_type& a2,
                            typename Function::arg3_type& a3,
                            typename Function::arg4_type& a4,
                            typename Function::arg5_type& a5,
                            typename Function::arg6_type& a6) {
                ORO_OS::MutexLock locker( lock );
                _a1 = a1; _a2 = a2; _a3 = a3; _a4 = a4; _a5 = a5; _a6 = a6;
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f( _a1, _a2, _a3, _a4, _a5, _a6 );
                work = false;
            }
            ORO_OS::Mutex lock;
            bool work;
        };

    class EventProcessor
        : public ORO_OS::RunnableInterface
    {
        typedef std::vector<EventCatcher*> List;
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

        template<class SignalType>
        boost::signals::connection connect(const typename SignalType::SlotFunction& f, SignalType& sig )
        {
            // Use function arity to select implementation :
            EventCatcher* eci = new EventCatcherImpl<SignalType::SlotFunction::arity, SignalType>(f, sig);
            {
                ORO_OS::MutexLock lock(m);
                catchers.push_back( eci );
            }
            return eci->getConnection();
        }

    };
}

#endif
