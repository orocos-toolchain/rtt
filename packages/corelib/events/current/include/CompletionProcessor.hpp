/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:32:45 CEST 2002  CompletionProcessor.hpp 

                       CompletionProcessor.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef COMPLETIONPROCESSOR_HPP
#define COMPLETIONPROCESSOR_HPP

#include "EventCompleterInterface.hpp" 
#include "EventListenerInterface.hpp"

#include "os/Mutex.hpp"
#include "os/PeriodicThread.hpp"

#include "pkgconf/system.h"

#ifndef ORODAT_CORELIB_CPBASE_H
// a good alternative when not defined :
#define ORODAT_CORELIB_CPBASE ORO_OS::PeriodicThread
#define ORODAT_CORELIB_CPBASE_H "os/PeriodicThread.hpp"
#endif

// Load alternate base for CP
#include ORODAT_CORELIB_CPBASE_H


// prefer queue above vector
#if   defined OROINT_OS_STDQUEUE
#include <queue>
#elif defined OROINT_OS_STDVECTOR
#include <vector>
#endif

namespace ORO_CoreLib
{
    /**
     * Complete queued events, meaning to execute the completer methods 
     * in a separate thread.
     * Each asynchronous operation of the system will be executed in the
     * thread of the CompletionProcessor. Because of the nature of asynchronosity,
     * the CompletionProcessor is considered non realtime, and so is all
     * code executed in its queue.
     *
     * The completion processor is used for all blocking / not-realtime
     * operations, you can do whatever you want inhere.
     *
     */
    class CompletionProcessor 
        : public ORODAT_CORELIB_CPBASE
    {
    public:
        /**
         * Returns the unique (singleton) CompletionProcessor.
         */
        static CompletionProcessor* Instance();

        /**
         * Releases the CompletionProcessor
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Queue a Completer for event completion
         *
         * @param eci 
         *        The completer to be called
         */
        void queue( EventCompleterInterface* eci );

        /**
         * Queue a Listener for event handling
         *
         * @param eli 
         *        The Listener to be called
         * @param cb
         *        The Callback for the Listener, this may not be null (0) !
         */
        void queue( EventListenerInterface* eli, CallbackInterface* cb );

        protected:

        /**
         * Constructor
         */
        CompletionProcessor();

        virtual ~CompletionProcessor()
        {}

        EventCompleterInterface* ec;
        
        EventListenerInterface*  el;
        
        CallbackInterface* cb;

        inline
        void nextEC();

        inline
        void nextEL();

        virtual void step();

#if   defined OROINT_OS_STDQUEUE
        std::deque< EventCompleterInterface* > eciBuffer;
        std::deque< std::pair<EventListenerInterface*,  CallbackInterface*> > eliBuffer;
#elif defined OROINT_OS_STDVECTOR
        const static int ECI_BUFFERSIZE = 1024;
        std::vector< EventCompleterInterface* > eciBuffer;
        std::vector< std::pair<EventListenerInterface*,  CallbackInterface*> > eliBuffer;
#endif
        ORO_OS::Mutex qLock;

    private:
        /**
         * Singleton
         */    
        static CompletionProcessor* cp;
    };
}

#endif
