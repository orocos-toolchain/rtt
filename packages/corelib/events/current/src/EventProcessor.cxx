
#include "corelib/EventProcessor.hpp"

namespace ORO_CoreLib
{

    namespace detail
    {
        using ORO_OS::Semaphore;

        EventCatcher::EventCatcher(Semaphore* s)
            : sem(s)
        {}

        EventCatcher::~EventCatcher() {}

        boost::signals::connection EventCatcher::getConnection() const
        {
            return h;
        }

    }

    EventProcessor::EventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s) : sem(s)
    {
        // TODO define MAX_ASYN_EVENTS
        catchers.reserve(128);
    }

    EventProcessor::EventProcessor() : sem( boost::shared_ptr<ORO_OS::Semaphore>() )
    {
        // TODO define MAX_ASYN_EVENTS
        catchers.reserve(128);
    }

    EventProcessor::~EventProcessor() {
        // TODO All slots are only deleted on destruction. Disconnected
        // slots remain in the list. We could fix this by signal connection tracking
        // and/or defining our own asyn connection object.
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
            delete *it;
    }

    bool EventProcessor::initialize() { return true;}

    void EventProcessor::step() {
        if ( catchers.empty() )
            return;
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it) {
            (*it)->complete();
        }
    }

    void EventProcessor::finalize() {
    }

    BlockingEventProcessor::BlockingEventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s )
        : EventProcessor( s ), doloop(false)
    {
    }

    BlockingEventProcessor::~BlockingEventProcessor() {
        this->breakloop();
        // race condition here, this->getTask()->isRunning() better be false
        // before EventProcessor is destructed, otherwise, solve with extra semaphore
    }
    void BlockingEventProcessor::loop() {
        //build critical section around loop()
        ORO_OS::MutexLock lock( breaker );
        doloop = true;
        while ( doloop )
            {
                this->step();
                // wait/block for the next Event
                sem->wait();
            }
    }

    void BlockingEventProcessor::breakloop() {
        doloop = false;
        sem->signal();
        {
            ORO_OS::MutexLock lock( breaker ); // force a block until loop returns.
        }
    }

    void BlockingEventProcessor::setSemaphore( boost::shared_ptr<ORO_OS::Semaphore> s) {
        ORO_OS::MutexLock lock(m);
        sem = s;
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it) {
            (*it)->sem = s.get();
        }
            
    }
}
