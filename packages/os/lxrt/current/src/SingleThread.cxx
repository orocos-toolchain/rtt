
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
        RT_TASK *mytask;
        unsigned long mytask_name;

        /**
         * This is one time initialisation
         */
        rtos_printf("Single Thread created...\n");
        SingleThread* task = static_cast<ORO_OS::SingleThread*> (t);

        mytask_name = nam2num( task->taskNameGet() );

        // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
        if (!(mytask = rt_task_init_schmod(mytask_name, task->priority, 0, 0, SCHED_FIFO, 0xF ))) {
	  std::cout << "CANNOT INIT TASK " << mytask_name <<std::endl;
	  exit(1);
        }
    
        task->rt_task = mytask;
        task->sem     = rt_sem_init( rt_get_name(0), 0 );
        rt_sem_signal( task->confDone );
        // locking of all memory for this process
        mlockall(MCL_CURRENT|MCL_FUTURE);
        /**
         * The real task starts here.
         */
        while(1) 
            {
                task->running = false;
                // for now, we always wait in soft RT.
                // An extra check before here could
                // make the switch again...
                rt_sem_wait( task->sem );
                if ( task->prepareForExit )
		  {
		    rtos_printf("Single Thread: Got signal from Destructor\n");
                    break;
		  }

                // The configuration might have changed
                // While waiting on the sem...
                if ( task->goRealtime )
                    rt_make_hard_real_time();
                else
                    rt_make_soft_real_time();
                if ( task->runComp != 0 )
		  {
                    task->runComp->step();

		  }
                else
                    task->step();

                // We do this to be able to safely
                // process the finalize method
                if ( task->isHardRealtime() )
                    rt_make_soft_real_time();

                // Allways soft realtime.
                if ( task->runComp != 0 )
                    task->runComp->finalize();
                else
                    task->finalize();
            }
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rt_make_soft_real_time();

        rt_sem_delete(task->sem);        
	rt_sem_signal( task->confDone );
        rt_task_delete(mytask);
        return 0;
    }

    SingleThread::SingleThread(int _priority, const std::string& name, RunnableInterface* r) :
        running(false), goRealtime(false), priority(_priority), prepareForExit(false),
        runComp(r)
    {
        if ( !name.empty() )
            taskNameSet(name.c_str());
        else
            num2nam(rt_get_name(0), taskName);

        rtos_printf("SingleThread Constructor: Creating SingleThread %s.\n", taskName );

        confDone = rt_sem_init( rt_get_name(0), 0 );
        pthread_create( &thread, 0, singleThread_f, this);
        rt_sem_wait(confDone);
    }
    
    SingleThread::~SingleThread() 
    {
        // Send the message to the thread...
        prepareForExit = true;
        rt_sem_signal( sem );
        
        // Wait for the 'ok' answer of the thread.
        rt_sem_wait( confDone );
        rt_sem_delete(confDone);

        if ( pthread_join(thread,0) != 0 ) rtos_printf("Error : %s failed to join\n",taskName);
        rtos_printf("Destructor: SingleThread %s destroyed.\n", taskName );
    }

    bool SingleThread::start() 
    {
        if ( isRunning() ) return false;

        if ( runComp )
            runComp->initialize();
        else
            initialize();

        running=true;
        rt_sem_signal(sem);

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
    
