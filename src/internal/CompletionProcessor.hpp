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

#include "../Activity.hpp"
#include "EventProcessor.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/checked_delete.hpp>

namespace RTT
{
    /**
     * @brief An activity which executes asynchronous (defered) handlers
     * of Events.
     *
     * This is an utility class which can be used to execute Event
     * callback functions in a thread. It's purpose is to allow a way
     * to process not real-time event callback functions, without
     * disturbing the emitor of the event.
     * If the user requires a
     * not real-time, complex or lengthy callback execution, it can
     * be done in this activity.
     *
     * @ingroup CoreLibEvents
     */
    class RTT_API CompletionProcessor
        : public BlockingEventProcessor,
          public Activity
    {
    public:
        typedef CompletionProcessor* CompletionProcessorPtr;

        /**
         * Returns the unique (singleton) CompletionProcessor.
         */
        static CompletionProcessorPtr Instance();

        /**
         * Releases the CompletionProcessor
         *
         * @return true on success, false on failure
         */
        static bool Release();

        virtual ~CompletionProcessor();

    protected:

        /**
         * Constructor
         */
        CompletionProcessor();
    private:
        /**
         * Singleton
         */
        static boost::shared_ptr<CompletionProcessor> cp;
    };
}

#endif
