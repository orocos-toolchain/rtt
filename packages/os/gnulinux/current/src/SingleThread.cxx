
#include <os/SingleThread.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#include <iostream>
#include <sys/mman.h>
#include "corelib/Time.hpp"

namespace ORO_OS 
{
    void *singleThread_f(void* t) 
    {
        /**
         * This is one time initialisation
         */
        rtos_printf("Single Thread created...\n");

        SingleThread* task = static_cast<ORO_OS::SingleThread*> (t);

        sem_init( &task->sem, 0, 0 );

        /**
         * The real task starts here.
         */
        while(1) 
            {
                task->running = false;
                // for now, we always wait in soft RT.
                // An extra check before here could
                // make the switch again...
                sem_wait( &task->sem );

                if ( task->prepareForExit )
                    break;

                if ( task->runComp != 0 )
                    task->runComp->step();
                else
                    task->step();

                if ( task->runComp != 0 )
                    task->runComp->finalize();
                else
                    task->finalize();
            }
    
        /**
         * Cleanup stuff
         */
        sem_destroy( &task->sem);

        rtos_printf("SingleThread terminating.\n");
        sem_post( &task->confDone );

        return 0;
    }

    SingleThread::SingleThread(int _priority, const std::string& name, RunnableInterface* r) :
        running(false), priority(_priority), prepareForExit(false),
        runComp(r)
    {
        if ( !name.empty() )
            taskNameSet(name.c_str());
        else
            taskNameSet("unnamed");

        rtos_printf("Creating SingleThread %s.\n", taskName );

        sem_init( &confDone, 0, 0);
        pthread_create( &thread, 0, singleThread_f, this);
        sem_wait( &confDone );
    }
    
    SingleThread::~SingleThread() 
    {
        // Send the message to the thread...
        prepareForExit = true;
        sem_post( &sem );
        
        // Wait for the 'ok' answer of the thread.
        sem_wait( &confDone );
        sem_destroy(&confDone);

        rtos_printf("SingleThread %s destroyed.\n", taskName );
    }

    bool SingleThread::start() 
    {
        if ( isRunning() ) return false;

        if ( runComp )
            runComp->initialize();
        else
            initialize();

        running=true;
        sem_post( &sem );

        return true;
    }

    bool SingleThread::isRunning() const
    {
        return running;
    }

    void SingleThread::step()
    {
    }

    bool SingleThread::initialize()
    { return true; }

    void SingleThread::finalize()
    {}

    void SingleThread::taskNameSet(const char* nm)
    {
        snprintf(taskName,TASKNAMESIZE,"%s",nm);
    }

    const char* SingleThread::taskNameGet() const
    {
        return taskName;
    }
   
};
    
