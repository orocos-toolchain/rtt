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

#include "os/Mutex.hpp"
#include "TaskNonPeriodic.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief Complete queued events, meaning to execute the completer methods 
     * in a separate thread.
     *
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
        : public TaskNonPeriodic
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

        protected:

        /**
         * Constructor
         */
        CompletionProcessor();

        virtual ~CompletionProcessor();

    private:
        /**
         * Singleton
         */    
        static CompletionProcessor* cp;
    };
}

#endif
