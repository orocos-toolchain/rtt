/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  PeriodicThread.cxx 

                        PeriodicThread.cxx -  description
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
/***************************************************************************
  tag: Peter Soetens  Mon Jun 10 14:43:12 CEST 2002  PeriodicThread.cpp 

                        PeriodicThread.cpp -  description
                           -------------------
    begin                : Mon June 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

#include <os/PeriodicThread.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_EVENTS
#include "corelib/EventCompleterInterface.hpp"
#endif

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
#include "corelib/CompletionProcessor.hpp"
#endif

#include <iostream>
#include <sys/mman.h>
#include "corelib/Time.hpp"

namespace ORO_OS 
{
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        /**
         * The Finalizer was invented because setToStop() wanted
         * to have finalize() called after step() completed.
         * This is only possible from another thread.
         * So we use the Completion processor to stop the task
         */
        class Finalizer : public ORO_CoreLib::EventCompleterInterface
        {
            PeriodicThread* parent;

        public:
            Finalizer( PeriodicThread* ct ) : parent( ct )
            {}

            void completeEvent()
            {
                parent->stop();
            }
        };
#endif

    void *ComponentThread(void* t) 
    {
        RTIME period, period_ns;

        RT_TASK *mytask;
        unsigned long mytask_name;

        /**
         * This is one time initialisation
         */
        rtos_printf("Component thread created...\n");

        //lock_all(8*1024, 512*1024);// stack,heap

        PeriodicThread* task = static_cast<ORO_OS::PeriodicThread*> (t);

        mytask_name = nam2num( task->taskNameGet() );

        // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
        if (!(mytask = rt_task_init_schmod(mytask_name, task->priority, 0, 0, SCHED_FIFO, 0xF ))) {
            std::cout << "CANNOT INIT TASK " << mytask_name <<std::endl;
            exit(1);
        }
    
        task->rt_task = mytask;
        task->sem     = rt_sem_init( rt_get_name(0), 0 );

        // locking of all memory for this process
        mlockall(MCL_CURRENT|MCL_FUTURE);

        period_ns = task->period;
        period = nano2count(period_ns);
        std::cout <<"Period is "<<period_ns<<std::endl;
        rt_task_make_periodic_relative_ns(mytask, 0, period_ns);
    
        //we were made periodic.
        rt_sem_signal(task->confDone);

        /**
         * The real task starts here.
         */
        while(1) 
            {
                while(!task->isRunning() && !task->prepareForExit ) 
                    {
                        // consider this the 'configuration state'
                        task->stopped = true;
                        rt_sem_wait( task->sem );
                        task->configure();
                        // end of configuration
                    }

                if ( task->prepareForExit ) 
                    {
                        break; // break while(1) {}
                    }
                task->stopped = false;
                if (task->runComp != 0)        
                    task->runComp->step(); // one cycle
                else
                    task->step();
                task->periodWait();
            }
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rt_make_soft_real_time();

        rt_sem_delete(task->sem);
        rt_task_delete(mytask);

        rtos_printf("Breaked : Killing myself\n");
        return 0;
    }

    /**
     * Generates unique ids starting with an underscore.
     * XXX use rt_get_adr(0).
     */
    unsigned long rt_generate_unique_id()
    {
        static unsigned long id = nam2num("_AAAAA");
        while ( rt_get_adr(id) )
            ++id;
        return id;
    }
            

    PeriodicThread::PeriodicThread(int _priority, const std::string& name, Seconds period, RunnableInterface* r) :
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        finalizer(new Finalizer(this) ), 
#else   
        finalizer(0),
#endif
        running(false), stopped(true), goRealtime(false), priority(_priority), prepareForExit(false),
        runComp(r)
    {
        if ( !name.empty() )
            taskNameSet(name.c_str());
        else
            num2nam(rt_get_name(0), taskName);

        rtos_printf("Constructing %s\n", taskName );

        confDone = rt_sem_init( rt_get_name(0), 0 );

        periodSet(period);
        pthread_create( &thread, 0, ComponentThread, this);
        rt_sem_wait(confDone);
    }
    
    PeriodicThread::~PeriodicThread() 
    {
        if (isRunning()) stop();

        terminate();
        rt_sem_delete(confDone);

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        delete finalizer;
#endif
        rtos_printf("%s destroyed\n", taskName );
    }

    bool PeriodicThread::start() 
    {
        if ( isRunning() ) return false;

        if ( runComp )
            runComp->initialize();
        else
            initialize();

        running=true;

        rt_sem_signal(sem);

        // twice make_periodic causes crash
        //rt_task_suspend(rt_task);
        //rt_task_make_periodic_relative_ns(rt_task, 0, nano2count( secs_to_nsecs(period.tv_sec) + period.tv_nsec) );

        return true;
    }

    bool PeriodicThread::stop() 
    {
        if ( !isRunning() ) return false;

        running=false;
        int cnt = 0;

        std::cout << "Stopping !" << std::endl;

        while ( stopped == false && cnt < 1000 )
            {
                rt_sleep( nano2count(1000000) );
                cnt++;
            } 

        std::cout <<"Finalizing thread after "<<cnt<<" tries !"<<std::endl;
        // from now on, the thread waits on sem.

        if ( runComp )
            runComp->finalize();
        else
            finalize();

        return true;
    }

    bool PeriodicThread::isRunning() const
    {
        return running;
    }

    void PeriodicThread::configure()
    {
        rt_set_period(rt_task, nano2count( period ));
        if ( goRealtime && !isHardRealtime() )
            {
                std::cout <<"Going HRT!"<<std::endl;
                rt_make_hard_real_time();
                rt_sem_signal( confDone );
            }
        else if ( !goRealtime && isHardRealtime() )
            {
                rt_make_soft_real_time();
                rt_sem_signal( confDone );
                std::cout <<"Returning to SRT!"<<std::endl;
            }
    }
        

    void PeriodicThread::step()
    {
    }

    bool PeriodicThread::initialize()
    { return true; }

    void PeriodicThread::finalize()
    {}

    int PeriodicThread::periodSet( double s )
    {
        if ( isRunning() )
            return -1;

        period = long(s) + long( (s - long(s) )* 1000*1000*1000);

        return 0;
    }

    int PeriodicThread::periodSet(secs s, nsecs ns) 
    {
        if ( isRunning() ) return -1;
        period = ns + 1000*1000*1000*s;
        return 0;
    }

    int PeriodicThread::periodSet( TIME_SPEC p) 
    {
        if (isRunning()) return -1;
        period = 1000*1000*1000* p.tv_sec + p.tv_nsec;
        return 0;
    }

    void PeriodicThread::periodGet(secs& s, nsecs& ns) const
    {
        s = period/(1000*1000*1000);
        ns = period - s*1000*1000*1000;
    }

    double PeriodicThread::periodGet() const
    {
        return double(period)/(1000.0*1000.0*1000.0);
    }

    void PeriodicThread::periodWait()
    {
        rt_task_wait_period();
    }

    void PeriodicThread::periodWaitRemaining()
    {
        rt_task_wait_period();
    }

    void PeriodicThread::terminate()
    {
        // avoid callling twice.
        if (prepareForExit) return;

        prepareForExit = true;

        rtos_printf("Terminating %s\n", taskNameGet());
        rt_sem_signal(sem);
    
        if ( pthread_join(thread,0) != 0 ) rtos_printf("Error : %s failed to join\n",taskName);
    }

    bool PeriodicThread::setToStop()
    {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        ORO_CoreLib::CompletionProcessor::Instance()->queue( finalizer );
        return true;
#else
        return false;
#endif
    }

    void PeriodicThread::taskNameSet(const char* nm)
    {
        snprintf(taskName,TASKNAMESIZE,"%s",nm);
    }

    const char* PeriodicThread::taskNameGet() const
    {
        return taskName;
    }
   
};
    
