#ifndef ORO_EVENTPROCESSOR_HPP
#define ORO_EVENTPROCESSOR_HPP

#include <os/RunnableInterface.hpp>
#include <os/MutexLock.hpp>
#include <boost/signals.hpp>
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
            //Result r;
            EventCatcherImpl(const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
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

            function< Result(void) > f_nullary;
            Function f;
            //Result r;
            EventCatcherImpl( const Function& f_, SignalType& sig )
                : f(f_), work(false)
            {
                h = sig.SignalType::signal_type::connect( boost::bind( &EventCatcherImpl<1, SignalType>::handler,
                                                                       this, _1) );
            }

            Result handler( typename Function::arg1_type& a1 ) {
                ORO_OS::MutexLock locker( lock );
                f_nullary = bind(f, a1);
                work = true;
                return Result();
            }

            virtual void complete() {
                ORO_OS::MutexLock locker( lock );
                if (work)
                    f_nullary();
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
