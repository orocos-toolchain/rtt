#ifndef MAINTHREAD_HPP
#define MAINTHREAD_HPP

#include "ThreadInterface.hpp"
#include <pkgconf/os_xenomai.h>
#include "fosi.h"

namespace ORO_OS
{

    /**
     * This class initializes the scheduler on construction
     */
    class SchedPolicy
    {
        static const int SELECT_THIS_PID=0;
    public:
        SchedPolicy();
            
        ~SchedPolicy();
    protected:
        RTOS_TASK* rt_task;
    };

    /**
     * This class simulates a main() realtime thread so that ipc is 
     * possible for realtime primitives
     *
     * This is Xenomai specific
     */
    class MainThread
        : protected SchedPolicy
    {
        static MainThread* mt;

    public:
        /**
         * Returns the single instance of MainThread.
         * @return The MainThread instance.
         */
        static MainThread* Instance()
        {
            if ( mt == 0 )
                mt = new MainThread();

            return mt;
        }

        static void Release()
        {
            if ( mt != 0 )
                delete mt;

            mt = 0;
        }

        bool start() { return true; }
        bool stop() { return true; }

        /**
         * The function to be queued
         * Use this as a base class if you want a function
         * to be execute()'d
         */
        struct Functor
        {
            virtual ~Functor() {};
            virtual void operator() () const = 0;
        };

        /**
         * Starts a task and returns when the
         * task effectively runs
         */
        void startTask( ThreadInterface* t )
        {
            t->start();
        }

        /**
         * Stops a task and returns when the
         * task has effectively stopped
         */
        void stopTask ( ThreadInterface* t )
        {
            t->stop();
        }

        /**
         * Executes a function object in the main thread
         */
        void execute( void (*funcObj)(void) )
        {
            (*funcObj)();
        }

        /**
         * Complete all pending requests
         */
        void complete()
        {
            // nothing to do since all is processed immediately
        }

    protected:

        MainThread()
            : SchedPolicy()
        {
        }

        ~MainThread()
        {
        }

    private:

    };

}

#endif
