/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  SingleThread.cxx 

                        SingleThread.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#define OROBLD_OS_LXRT_INTERNAL
#include <os/SingleThread.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif
#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
using ORO_CoreLib::Logger;
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
        SingleThread* task = static_cast<ORO_OS::SingleThread*> (t);

        mytask_name = nam2num( task->taskNameGet() );

        // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
        if (!(mytask = rt_task_init_schmod(mytask_name, task->priority, 0, 0, SCHED_FIFO, 0xF ))) {
            std::cerr << task->taskName << " : CANNOT INIT LXRT TASK " << mytask_name <<std::endl;
            std::cerr << "Exiting this thread." <<std::endl;
            return 0;
        }
    
        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" created."<<Logger::endl;
#endif
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
                    break;

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

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif
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

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "SingleThread: Creating "<< taskName <<"."<<Logger::endl;
#endif

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

        if ( pthread_join(thread,0) != 0 ) 
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Error << "SingleThread: Failed to join with thread "<< taskName <<"."<<Logger::endl;
        else
            Logger::log() << Logger::Debug << "SingleThread: Joined with thread "<< taskName <<"."<<Logger::endl
#endif
                ;
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
    
