
#include "corelib/CompletionProcessor.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Time.hpp"

#include "pkgconf/corelib_events.h"

namespace ORO_CoreLib
{

    CompletionProcessor* CompletionProcessor::cp;

    CompletionProcessor* CompletionProcessor::Instance()
    {
        if ( cp == 0 )
            {
                cp = new CompletionProcessor();
            }
        return cp;
    }

    bool CompletionProcessor::Release()
    {
        if ( cp != 0 )
            {
                delete cp;
                cp = 0;
                return true;
            }
        return false;
    }

    void CompletionProcessor::queue( EventCompleterInterface* eci )
    {
        //cout << "[CompletionProcessor] I schedule a completer" << endl;
        ORO_OS::MutexLock locker( qLock );
        eciBuffer.push_back( eci );
    }


    void CompletionProcessor::queue( EventListenerInterface* eli, CallbackInterface* cb )
    {
        //cout << "[CompletionProcessor] I schedule a listener" << endl;
        ORO_OS::MutexLock locker( qLock );
        eliBuffer.push_back( std::make_pair( eli, cb ) );
    }

    CompletionProcessor::CompletionProcessor()
        : ORODAT_CORELIB_CPBASE(  ORONUM_CORELIB_EVENTS_CP_PRIORITY, 
                                  ORODAT_CORELIB_EVENTS_CP_NAME, 
                                  ORONUM_CORELIB_EVENTS_CP_PERIOD )
    {
#ifndef OROINT_OS_STDQUEUE
        eciBuffer.reserve(ECI_BUFFERSIZE);
        eliBuffer.reserve(ECI_BUFFERSIZE);
#endif
    }

    void CompletionProcessor::nextEC()
    {
        ORO_OS::MutexLock locker( qLock );
        ec = eciBuffer.front();
        eciBuffer.pop_back();
    }

    void CompletionProcessor::nextEL()
    {
        ORO_OS::MutexLock locker( qLock );
        el = eliBuffer.front().first;
        cb = eliBuffer.front().second;
        eliBuffer.pop_back();
    }


    void CompletionProcessor::step()
    {
        // the baseclass always has a step method;
        ORODAT_CORELIB_CPBASE::step();

        // Execute all pending completions.
        while ( ! eliBuffer.empty() )
            {
                nextEL();
                el->handleEvent( cb );
            }

        while ( ! eciBuffer.empty() )
            {
                nextEC();
                ec->completeEvent();
            }
    }
}
