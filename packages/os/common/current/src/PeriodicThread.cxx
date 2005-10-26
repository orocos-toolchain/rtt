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

#include "os/fosi_internal.hpp"
#include <os/PeriodicThread.hpp>
#include <os/Time.hpp>

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

#include "pkgconf/os.h"

#include <iostream>

#include "corelib/Time.hpp"
#include "os/threads.hpp"
#include "pkgconf/os.h"
#ifdef OROPKG_DEVICE_INTERFACE
#include "pkgconf/device_interface.h"
#ifdef OROPKG_OS_THREAD_SCOPE
#include ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
using namespace ORO_DeviceInterface;
using namespace ORO_DeviceDriver;
#endif
#endif

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

    using namespace detail;

    void *periodicThread(void* t) 
    {
        /**
         * This is one time initialisation
         */
        PeriodicThread* task = static_cast<ORO_OS::PeriodicThread*> (t);

        task->reconfigScheduler();

        task->rtos_task = detail::rtos_task_init( task );

        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" created with priority "<<task->priority<<"."<<Logger::endl;
#endif

#ifdef OROPKG_OS_THREAD_SCOPE
        unsigned int bit = task->threadnb;

        boost::scoped_ptr<DigitalOutInterface> pp;
        try {
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                pp.reset( DigitalOutInterface::nameserver.getObject("ThreadScope") );
            else
# ifdef OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER
                pp.reset( new OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER() );
# else
                Logger::log() << Logger::Error<< "PeriodicThread : Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
# endif
        } catch( ... )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Error<< "PeriodicThread : Failed to create ThreadScope." << Logger::endl;
#endif
            }
        if ( pp ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Info
                          << "ThreadScope : Periodic Thread "<< task->taskName <<" toggles bit "<< bit << Logger::endl;
#endif
            pp->switchOff( bit );
        }
#endif

        rtos_task_make_periodic(task->rtos_task, task->period );

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
                            rtos_sem_signal( &(task->confDone) ); // signal we are ready for command
//                             Logger::log() << Logger::Info <<task->taskName<<"  sleeps !" << Logger::endl;
                            rtos_sem_wait( &(task->sem) );      // wait for command.
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
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( pp )
                                pp->switchOn( bit );
#endif
                            task->step(); // one cycle
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( pp )
                                pp->switchOff( bit );
#endif

                            if (task->wait_for_step)
                                rtos_task_wait_period( task->rtos_task );
                        }
                    }
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( pp )
                    pp->switchOff( bit );
#endif
                // set state to not running
                task->running = false;
                if ( task->isHardRealtime() )
                    rtos_task_make_soft_real_time( task->rtos_task );
                task->finalize();
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Fatal << "Periodic Thread "<< task->taskName <<" caught a C++ exception, stopped thread !"<<Logger::endl;
#endif
            }
        }
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rtos_task_make_soft_real_time( task->rtos_task );

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif

        rtos_task_delete( task->rtos_task );

        return 0;
    }

    PeriodicThread::PeriodicThread(int _priority, std::string name, Seconds periods, RunnableInterface* r) :
        running(false), goRealtime(false), priority(_priority), prepareForExit(false),
        runComp(r), wait_for_step(true), sched_type( OROSEM_OS_SCHEDTYPE )
    {
        int ret;
        rtos_thread_init( this, name );
        
        ret = rtos_sem_init(&sem, 0);
        if ( ret != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "PeriodicThread : could not allocate configuration semaphore 'sem' for "<< taskName <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
            throw std::bad_alloc();
        }

        ret = rtos_sem_init(&confDone, 0);
        if ( ret != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Critical << "PeriodicThread : could not allocate configuration semaphore 'confDone' for "<< taskName <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
            rtos_sem_destroy( &sem );
            throw std::bad_alloc();
        }

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h = new ORO_CoreLib::Handle();
        stopEvent = static_cast<void*>( new Event<bool(void)>() );
#endif

        // Do not call setPeriod(), since the semaphores are not yet used !
        period = Seconds_to_nsecs(periods);

        pthread_create( &thread, 0, periodicThread, this);
        rtos_sem_wait(&confDone);
    }
    
    PeriodicThread::~PeriodicThread() 
    {
        if (isRunning()) stop();

        terminate();
        rtos_sem_destroy(&confDone);
        rtos_sem_destroy(&sem);

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
        rtos_sem_trywait( &confDone );
        // signal start :
        int ret = rtos_sem_signal(&sem);
        if ( ret != 0 )
            Logger::log() << Logger::Critical <<"PeriodicThread::start(): sem_signal returns "<< ret << Logger::endl;
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
                ret = rtos_sem_wait_timed( &confDone, 5*period );
                // if ret == 0, confDone was signaled.
                cnt++;
            } 
#else
        // dangerous, unconditionally waiting might/will lock up your program !
        rtos_sem_wait( &confDone );
#endif
#ifdef OROPKG_CORELIB_REPORTING
        if ( ret != 0 ) {
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
                rtos_sem_signal(&sem);
                rtos_sem_wait(&confDone);
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
                rtos_sem_signal(&sem);
                rtos_sem_wait(&confDone);
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

    bool PeriodicThread::isHardRealtime() const { return rtos_task_is_hard_real_time(rtos_task); }


    void PeriodicThread::configure()
    {
        // reconfigure period
        if ( wait_for_step )
            rtos_task_set_period(rtos_task, period );
        else
            rtos_task_set_period(rtos_task, 0);
        if ( goRealtime && !isHardRealtime() )
            {
                //std::cout <<"Going HRT!"<<std::endl;
                rtos_task_make_hard_real_time( rtos_task);
            }
        else if ( !goRealtime && isHardRealtime() )
            {
                rtos_task_make_soft_real_time( rtos_task );
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
        rtos_sem_signal(&sem);
        rtos_sem_wait(&confDone);

        return true;
    }

    bool PeriodicThread::setPeriod(secs s, nsecs ns) 
    {
        if ( isRunning() ) return false;
        period = ns + 1000*1000*1000*s;

        // signal change to thread.
        rtos_sem_signal(&sem);
        rtos_sem_wait(&confDone);

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

    int PeriodicThread::getPriority() const 
    {
        return priority;
    }

    double PeriodicThread::getPeriod() const
    {
        return nsecs_to_Seconds(period);
    }

    nsecs PeriodicThread::getPeriodNS() const
    {
        return period;
    }

    void PeriodicThread::continuousStepping(bool yes_no)
    {
        wait_for_step = !yes_no;
        // do not change the period here, do it in configure()
    }

    void PeriodicThread::terminate()
    {
        // avoid callling twice.
        if (prepareForExit) return;

        prepareForExit = true;

        rtos_sem_signal(&sem);
    
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
        taskName = nm;
    }

    const char* PeriodicThread::getName() const
    {
        return taskName.c_str();
    }

    bool PeriodicThread::setScheduler( int sched ) {
        if ( this->isHardRealtime() || this->isRunning() || ( sched != SCHED_OTHER && sched != SCHED_FIFO && sched != SCHED_RR ))
            return false;
        sched_type = sched;
        // signal change to thread.
        rtos_sem_signal( &sem);
        rtos_sem_wait( &confDone);
        
        return true;
    }

    void PeriodicThread::reconfigScheduler() {
        // ONLY CALL FROM SRT !
        rtos_set_scheduler(this->sched_type, this->priority);
    }
}
    
