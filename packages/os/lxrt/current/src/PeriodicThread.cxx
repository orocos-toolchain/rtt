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

    void *periodicThread(void* t) 
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

        task->reconfigScheduler();

        // init lxrt task :
        // name, priority, stack_size, msg_size, policy.
        if (!(mytask = rt_task_init(mytask_name, task->priority, 0, 0 ))) {
            std::cerr << task->taskName << " : CANNOT INIT LXRT TASK " << mytask_name <<std::endl;
            std::cerr << "Exiting this thread." <<std::endl;
            exit (-1); // return is no usefull since the main would remain blocked on sem.
        }

        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" created with priority "<<task->priority<<"."<<Logger::endl;
#endif
    
        task->rt_task = mytask;

        // locking of all memory for this process
        mlockall(MCL_CURRENT|MCL_FUTURE);

        period_ns = task->period;
        period = nano2count(period_ns);
        rt_task_make_periodic_relative_ns(mytask, 0, period_ns);

        while ( !task->prepareForExit ) {
            try {
                /**
                 * The real task starts here.
                 */
                while(1) 
                    {
                        if( !task->running ) { // more efficient than calling isRunning()
                            // consider this the 'configuration state'
//                             Logger::log() << Logger::Info <<task->taskName<<"  signals done !" << Logger::endl;
                            rt_sem_signal(task->confDone); // signal we are ready for command
//                             Logger::log() << Logger::Info <<task->taskName<<"  sleeps !" << Logger::endl;
                            rt_sem_wait( task->sem );      // wait for command.
//                             Logger::log() << Logger::Info <<task->taskName<<"  awoken !" << Logger::endl;
                            task->configure();             // check for reconfigure
//                             Logger::log() << Logger::Info <<task->taskName<<"  config done !" << Logger::endl;
                            if ( task->prepareForExit )    // check for exit
                                {
                                    break; // break while(1) {}
                                }
                            // end of configuration
                        } else {
                            // task->isRunning()
                            task->step(); // one cycle

                            if (task->wait_for_step)
                                task->periodWaitRemaining();
                        }
                    }
            } catch( ... ) {
                // set state to not running
                task->running = false;
                if ( task->isHardRealtime() )
                    rt_make_soft_real_time();
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Fatal << "Periodic Thread "<< task->taskName <<" caught a C++ exception, stopping thread !"<<Logger::endl;
#endif
                task->finalize();
            }
        }
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rt_make_soft_real_time();

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif

        rt_task_delete(mytask);

        return 0;
    }

    PeriodicThread::PeriodicThread(int _priority, std::string name, Seconds periods, RunnableInterface* r) :
        running(false), goRealtime(false), priority(_priority), prepareForExit(false),
        runComp(r), wait_for_step(true), sched_type( OROSEM_OS_LXRT_SCHEDTYPE )
    {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h = new ORO_CoreLib::Handle();
        stopEvent = static_cast<void*>( new Event<bool(void)>() );
#endif
        if ( name.empty() )
            name = "PeriodicThread";
        if ( rt_get_adr(nam2num( name.c_str() )) == 0 )
            setName(name.c_str());
        else {
            unsigned long nname = nam2num( name.c_str() );
            while ( rt_get_adr( nname ) != 0 ) // check for existing 'NAME'
                ++nname;
            num2nam( nname, taskName); // set taskName to new name
        }

        sem = rt_typed_sem_init( rt_get_name(0), 0, BIN_SEM );
        if ( sem == 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "PeriodicThread : could not allocate configuration semaphore 'sem' for "<< taskName <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
            delete h;
            delete static_cast<Event<bool(void)>*>(stopEvent);
#endif
            throw std::bad_alloc();
        }

        confDone = rt_typed_sem_init( rt_get_name(0), 0, BIN_SEM );
        if ( confDone == 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "PeriodicThread : could not allocate configuration semaphore 'confDone' for "<< taskName <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
            rt_sem_delete( sem );
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
            delete h;
            delete static_cast<Event<bool(void)>*>(stopEvent);
#endif
            throw std::bad_alloc();
        }

        // Do not call setPeriod(), since the semaphores are not yet used !
        double s = periods;
        period = long(s) + long( (s - long(s) )* 1000*1000*1000);

        pthread_create( &thread, 0, periodicThread, this);
        rt_sem_wait(confDone);
    }
    
    PeriodicThread::~PeriodicThread() 
    {
        if (isRunning()) stop();

        terminate();
        rt_sem_delete(confDone);
        rt_sem_delete(sem);

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
        runComp = r;
        return true;
    }


    bool PeriodicThread::start() 
    {
        if ( running ) return false;

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

        // be sure that confDone is set to zero
        // could be in case stop() times out.
        rt_sem_wait_if( confDone );
        // signal start :
        int ret = rt_sem_signal(sem);
        if ( ret == SEM_ERR )
            Logger::log() << Logger::Critical <<"PeriodicThread::start(): sem_signal returns SEM_ERR." << Logger::endl;
        // do not wait, we did our job.

        return true;
    }

    bool PeriodicThread::stop() 
    {
        if ( !running ) return false;

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< taskName <<" stopping...";
#endif

        running=false;
        int cnt = 0;
        int ret = -1;

        // wait until the loop detects running == false
        // we wait 10 times 5*period.
#if 1
        while ( ret != 0 && cnt < 10 )
            {
                // given time in argument is relative to 'now'
                ret = rt_sem_wait_timed( confDone, nano2count(5*period) );
                // if ret == 0, confDone was signaled.
                cnt++;
            } 
#else
        // dangerous, unconditionally waiting might/will lock up your program !
        rt_sem_wait( confDone );
#endif
#ifdef OROPKG_CORELIB_REPORTING
        if ( cnt == 10 ) {
            Logger::log() << Logger::Debug << " failed."<<Logger::endl;
            Logger::log() << Logger::Critical << "The "<< taskName <<" thread seems to be blocked ( ret was "<< ret<<" .)"<<Logger::nl;
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
                if ( isHardRealtime() ) {
#ifdef OROPKG_CORELIB_REPORTING
                    Logger::log() << Logger::Debug << "Thread "<< taskName <<" already Hard Realtime...";
#endif
                    return true;
                }

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
                if ( !isHardRealtime() ) {
#ifdef OROPKG_CORELIB_REPORTING
                    Logger::log() << Logger::Debug << "Thread "<< taskName <<" already Soft Realtime...";
#endif
                    return true;
                }
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
        // reconfigure period
        if ( wait_for_step )
            rt_set_period(rt_task, nano2count( period ));
        else
            rt_set_period(rt_task, 0);
        if ( goRealtime && !isHardRealtime() )
            {
                //std::cout <<"Going HRT!"<<std::endl;
                rt_make_hard_real_time();
            }
        else if ( !goRealtime && isHardRealtime() )
            {
                rt_make_soft_real_time();
                //std::cout <<"Returning to SRT!"<<std::endl;
            }
        if ( !isHardRealtime() ) { // reconfigure scheduler type.
            int type = sched_getscheduler(0);
            if ( type != sched_type )
                this->reconfigScheduler();
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

        // signal change to thread.
        rt_sem_signal(sem);
        rt_sem_wait(confDone);

        return true;
    }

    bool PeriodicThread::setPeriod(secs s, nsecs ns) 
    {
        if ( isRunning() ) return false;
        period = ns + 1000*1000*1000*s;

        // signal change to thread.
        rt_sem_signal(sem);
        rt_sem_wait(confDone);

        return true;
    }

    bool PeriodicThread::setPeriod( TIME_SPEC p) 
    {
        return this->setPeriod( p.tv_sec, p.tv_nsec );
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

    void PeriodicThread::continuousStepping(bool yes_no)
    {
        wait_for_step = !yes_no;
        // do not change the period here, do it in configure()
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

    bool PeriodicThread::setScheduler( int sched ) {
        if ( this->isHardRealtime() || this->isRunning() || ( sched != SCHED_OTHER && sched != SCHED_FIFO && sched != SCHED_RR ))
            return false;
        sched_type = sched;
        // signal change to thread.
        rt_sem_signal(sem);
        rt_sem_wait(confDone);
        
        return true;
    }

    void PeriodicThread::reconfigScheduler() {
        // ONLY CALL FROM SRT !

        // init the scheduler. The rt_task_initschmod code is broken, so we do it ourselves.
        struct sched_param mysched;
        mysched.sched_priority = sched_get_priority_max(this->sched_type) - this->priority;
        // check lower bounds :
        if (this->sched_type == SCHED_OTHER && mysched.sched_priority != 0 ) {
            mysched.sched_priority = 0; // SCHED_OTHER must be zero
        } else if (this->sched_type == !SCHED_OTHER &&  mysched.sched_priority < 1 ) {
            mysched.sched_priority = 1; // !SCHED_OTHER must be 1 or higher
        }
        // check upper bound
        if ( mysched.sched_priority > 99)
            mysched.sched_priority = 99;
        // set scheduler
        if (sched_setscheduler(0, this->sched_type, &mysched) < 0) {
            // can this ever happen ?
            std::cerr << this->taskName << " : CANNOT INIT SCHEDULER to " << this->sched_type << " with priority "<< mysched.sched_priority <<std::endl;
        }
    }
}
    
