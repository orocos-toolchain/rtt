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

#define OROBLD_OS_LXRT_INTERNAL
#include <os/PeriodicThread.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_EVENTS
#include "corelib/Event.hpp"
#endif
#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
#endif

#include <iostream>
#include <sys/mman.h>
#include "corelib/Time.hpp"

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
/**
 * We use the Completion processor to stop the task
 */
using boost::bind;
using namespace ORO_CoreLib;

#endif



namespace ORO_OS
{
#ifdef OROPKG_CORELIB_REPORTING
    using ORO_CoreLib::Logger;
#endif

    void *ComponentThread(void* t) 
    {
        RTIME period, period_ns;

        RT_TASK *mytask;
        unsigned long mytask_name;

        /**
         * This is one time initialisation
         */

        //lock_all(8*1024, 512*1024);// stack,heap

        PeriodicThread* task = static_cast<ORO_OS::PeriodicThread*> (t);

        mytask_name = nam2num( task->getName() );

        // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
        if (!(mytask = rt_task_init_schmod(mytask_name, task->priority, 0, 0, SCHED_FIFO, 0xF ))) {
            std::cerr << task->taskName << " : CANNOT INIT LXRT TASK " << mytask_name <<std::endl;
            std::cerr << "Exiting this thread." <<std::endl;
            exit (-1); // return is no usefull since the main would remain blocked on sem.
        }

        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" created."<<Logger::endl;
#endif
    
        task->rt_task = mytask;
        task->sem     = rt_sem_init( rt_get_name(0), 0 );

        // locking of all memory for this process
        mlockall(MCL_CURRENT|MCL_FUTURE);

        period_ns = task->period;
        period = nano2count(period_ns);
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

                task->step(); // one cycle

                if (task->wait_for_step)
                    task->periodWait();
            }
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rt_make_soft_real_time();

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif

        rt_sem_delete(task->sem);
        rt_task_delete(mytask);

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
        running(false), stopped(true), goRealtime(false), priority(_priority), prepareForExit(false),
        runComp(r), wait_for_step(true)
    {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h = new ORO_CoreLib::Handle();
        stopEvent = static_cast<void*>( new Event<bool(void)>() );
#endif
        if ( !name.empty() && rt_get_adr(nam2num( name.c_str() )) == 0 )
            setName(name.c_str());
        else {
            unsigned long nname = nam2num( name.c_str() );
            while ( rt_get_adr( nname ) != 0 ) // check for existing 'NAME'
                ++nname;
            num2nam( nname, taskName); // set taskName to new name
        }

        confDone = rt_sem_init( rt_get_name(0), 0 );

        setPeriod(period);
        pthread_create( &thread, 0, ComponentThread, this);
        rt_sem_wait(confDone);
    }
    
    PeriodicThread::~PeriodicThread() 
    {
        if (isRunning()) stop();

        terminate();
        rt_sem_delete(confDone);

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h->disconnect();
        delete h;
        delete static_cast<Event<bool(void)>*>(stopEvent);
#endif
    }

    bool PeriodicThread::run( RunnableInterface* r)
    {
        if ( isRunning() )
            return false;
        runner = r;
        return true;
    }


    bool PeriodicThread::start() 
    {
        if ( isRunning() ) return false;

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< taskName <<" started."<<Logger::endl;
#endif

        bool result;
        result = this->initialize();

        if (result == false) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "Periodic Thread "<< taskName <<" failed to initialize()."<<Logger::endl;
#endif
            return false;
        }

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        *h = static_cast<Event<bool(void)>*>(stopEvent)->connect( bind( &PeriodicThread::stop, this ), ORO_CoreLib::CompletionProcessor::Instance() );
#endif
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

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< taskName <<" stopping...";
#endif

        running=false;
        int cnt = 0;

        while ( stopped == false && cnt < 1000 )
            {
                rt_sleep( nano2count(1000000) );
                cnt++;
            } 
#ifdef OROPKG_CORELIB_REPORTING
        if ( cnt == 1000 ) {
            Logger::log() << Logger::Debug << " failed."<<Logger::endl;
            Logger::log() << Logger::Critical << "The "<< taskName <<" thread seems to be blocked."<<Logger::endl;
        }
        else
            Logger::log() << Logger::Debug << " done."<<Logger::endl;
#endif
        //std::cout <<"Finalizing thread after "<<cnt<<" tries !"<<std::endl;
        // from now on, the thread waits on sem.

        this->finalize();

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h->disconnect();
#endif
        return true;
    }

    bool PeriodicThread::isRunning() const
    {
        return running;
    }

    bool PeriodicThread::makeHardRealtime() 
    { 
        if ( !running ) 
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Debug << "Making "<< taskName <<" Hard Realtime...";
#endif
                goRealtime = true; 
                rt_sem_signal(sem);
                rt_sem_wait(confDone);
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Debug << " done."<<Logger::endl;
#endif
            }
#ifdef OROPKG_CORELIB_REPORTING
        else
            Logger::log() << Logger::Warning << "Failed to make "<< taskName <<" Hard Realtime since thread is still running."<<Logger::endl;
#endif
        return goRealtime; 
    }

    bool PeriodicThread::makeSoftRealtime()
    { 
        if ( !running ) 
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Debug << "Making "<< taskName <<" Soft Realtime...";
#endif
                goRealtime = false; 
                rt_sem_signal(sem);
                rt_sem_wait(confDone);
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Debug << " done."<<Logger::endl;
#endif
            }
#ifdef OROPKG_CORELIB_REPORTING
        else
            Logger::log() << Logger::Warning << "Failed to make "<< taskName <<" Soft Realtime since thread is still running."<<Logger::endl;
#endif
        return !goRealtime; 
    }

    bool PeriodicThread::isHardRealtime()   { return rt_is_hard_real_time(rt_task); }


    void PeriodicThread::configure()
    {
        rt_set_period(rt_task, nano2count( period ));
        if ( goRealtime && !isHardRealtime() )
            {
                //std::cout <<"Going HRT!"<<std::endl;
                rt_make_hard_real_time();
                rt_sem_signal( confDone );
            }
        else if ( !goRealtime && isHardRealtime() )
            {
                rt_make_soft_real_time();
                rt_sem_signal( confDone );
                //std::cout <<"Returning to SRT!"<<std::endl;
            }
    }
        

    void PeriodicThread::step()
    {
        if ( runComp )        
            runComp->step();
    }

    bool PeriodicThread::initialize()
    {
        if ( runComp )
            return runComp->initialize();
            
        return true;
    }

    void PeriodicThread::finalize()
    {
        if ( runComp )
            runComp->finalize();
    }

    bool PeriodicThread::setPeriod( double s )
    {
        if ( isRunning() )
            return false;

        period = long(s) + long( (s - long(s) )* 1000*1000*1000);

        return true;
    }

    bool PeriodicThread::setPeriod(secs s, nsecs ns) 
    {
        if ( isRunning() ) return false;
        period = ns + 1000*1000*1000*s;
        return true;
    }

    bool PeriodicThread::setPeriod( TIME_SPEC p) 
    {
        if (isRunning()) return false;
        period = 1000*1000*1000* p.tv_sec + p.tv_nsec;
        return true;
    }

    void PeriodicThread::getPeriod(secs& s, nsecs& ns) const
    {
        s = period/(1000*1000*1000);
        ns = period - s*1000*1000*1000;
    }

    double PeriodicThread::getPeriod() const
    {
        return double(period)/(1000.0*1000.0*1000.0);
    }

    void PeriodicThread::periodWait()
    {
        rt_task_wait_period();
    }

    void PeriodicThread::continuousStepping(bool yes_no)
    {
        // XXX Test this code ! Previously, calling rt_task_make_periodic...
        // twice could cause a crash.
        wait_for_step = !yes_no;
        if ( wait_for_step )
            rt_task_make_periodic_relative_ns(rt_task, 0, period);
        else
            rt_task_make_periodic_relative_ns(rt_task, 0, 0);

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

        rt_sem_signal(sem);
    
        if ( pthread_join(thread,0) != 0 ) 
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "Failed to join "<< taskName <<"."<< Logger::endl;
#else
        ;
#endif
    }

    bool PeriodicThread::setToStop()
    {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        static_cast< Event<bool(void)>* >(stopEvent)->fire();
        return true;
#else
        return false;
#endif
    }

    void PeriodicThread::setName(const char* nm)
    {
        snprintf(taskName,TASKNAMESIZE,"%s",nm);
    }

    const char* PeriodicThread::getName() const
    {
        return taskName;
    }
   
};
    
