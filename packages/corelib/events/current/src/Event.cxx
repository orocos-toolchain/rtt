
#include "corelib/Event.hpp"

#include "corelib/CompletionProcessor.hpp"


namespace ORO_CoreLib
{
    Event::Event( EventType t)
        :EventInterface(), EventStrategyMethods(t)
    {
        init();
    }

    Event::Event( EventType t, const std::string& name)
        : EventOperationInterface(name), EventInterface(name), EventStrategyMethods(t)
    {
        init();
    }

    Event::Event( EventType t, const std::string& opName, const std::string& regName)
        :EventOperationInterface(opName), EventInterface(regName), EventStrategyMethods(t)
    {
        init();
    }

    void Event::init()
    {
        evCallback.reserve(128);
        cp = CompletionProcessor::Instance();
    }

    Event::~Event()
    {}


    void Event::fire()
    {
        ORO_OS::MutexTryLock locker( evLock );

        if ( locker.isSuccessful() )
            strategy->notify( this );
    }

    void Event::complete( EventListenerInterface* eli )
    {
        strategy->complete(this,eli);
    }

    void Event::addHandler( EventListenerInterface * eli, EventCompleterInterface * eci )
    {
        insert( std::make_pair( eli, eci ) );
        evCallback.push_back( EventCallbackStub(this, eli) );
    }


    void Event::removeHandler( EventListenerInterface * eli, EventCompleterInterface* eci )
    {
        erase( eli );
        std::vector<EventCallbackStub>::iterator itl;
        itl = find_if(evCallback.begin(), evCallback.end(), bind2nd(EventCallbackStub::FindStub(),eli) );
        evCallback.erase(itl);
    }

    void Event::notifySyn()
    {
        iterator it_map;

        for ( it_map = begin(); it_map != end(); ++it_map )
        {
            // Explicit dereference from iterator to correct type needed
            // for GCC 3.2, but it crashes on 2.95 ?
            std::vector<EventCallbackStub>::iterator callback = find_if(evCallback.begin(), evCallback.end(), bind2nd(EventCallbackStub::FindStub(),it_map->first) );
            if ( callback != evCallback.end() )
                it_map->first->handleEvent( &*callback );
        }
    }

    void Event::notifyAsyn()
    {
        iterator it_map;
        for ( it_map = begin(); it_map != end(); ++it_map )
        {
            // Explicit dereference from iterator to correct type needed
            // for GCC 3.2
            ORO_CoreLib::EventCallbackStub& r = *find_if(evCallback.begin(), evCallback.end(), bind2nd(EventCallbackStub::FindStub(),it_map->first) );
            cp->queue( it_map->first, &r);
        }
    }

    void Event::completeSyn( EventListenerInterface* eli )
    {
        if ( ( *this ) [ eli ] != 0 )
            ( *this ) [ eli ] ->completeEvent();
    }

    void Event::completeAsyn( EventListenerInterface* eli )
    {
        if ( ( *this ) [ eli ] != 0 )
            cp->queue( ( *this ) [ eli ] );
    }

}
