
#ifndef NONREALTIMETHREAD_HPP
#define NONREALTIMETHREAD_HPP

#include "EventCompleterInterface.hpp" 
#include "EventListenerInterface.hpp"

#include "os/Mutex.hpp"
#include "os/ComponentThreaded.hpp"
#include "TaskExecution.hpp"

#include "pkgconf/corelib_tasks.h"

namespace ORO_CoreLib
{
    class TaskNonRealTime;

    /**
     * It is used to run periodically TaskNonRealTime instances, which
     * will try to meet deadlines 'as good as possible'.
     */
    class NonRealTimeThread 
        : public TaskExecution
    {

        public:
        /**
         * Returns the unique (singleton) NonRealTimeThread.
         */
        static NonRealTimeThread* Instance();

        /**
         * Releases the NonRealTimeThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Add a TaskNonRealTime which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( TaskNonRealTime* t, const nsecs n );

        /**
         * Remove a TaskNonPreemptible from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( TaskNonRealTime* t );

        protected:

#ifndef OROSEM_CORELIB_TASKS_INTEGRATE_COMPLETION
        /**
         * Default Constructor
         */
        NonRealTimeThread();

        private:
        /**
         * Singleton
         */    
        static NonRealTimeThread* cp;
#else
        /**
         * Constructor to be used by CompletionProcessor.
         */
        NonRealTimeThread::NonRealTimeThread(int priority, const std::string& name, double periodicity);
#endif

        virtual ~NonRealTimeThread()
        {}

        virtual void step();

        ORO_OS::Mutex qLock;

    };
}

#endif
