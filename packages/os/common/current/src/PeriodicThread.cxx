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

#include "rtt/os/fosi_internal.hpp"
#include <rtt/os/PeriodicThread.hpp>
#include <rtt/os/Time.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "rtt/Logger.hpp"
#endif

#include "pkgconf/os.h"

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

#include "rtt/Time.hpp"
#include "rtt/os/threads.hpp"
#include "pkgconf/os.h"
#ifdef OROPKG_DEVICE_INTERFACE
# include "pkgconf/device_interface.h"
# include <boost/scoped_ptr.hpp>
# ifdef OROPKG_OS_THREAD_SCOPE
#  include "rtt/dev/DigitalOutInterface.hpp"
   using namespace RTT;
#  ifdef ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
#   include ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
    using namespace RTT;
#  endif
# endif
#endif

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
/**
 * We use the Completion processor to stop the task
 */
using boost::bind;
using namespace RTT;

#endif

namespace RTT
{ namespace OS {
#ifdef OROPKG_CORELIB_REPORTING
    using RTT::Logger;
#endif

    using namespace detail;

    void *periodicThread(void* t) 
    {
        /**
         * This is one time initialisation
         */
        PeriodicThread* task = static_cast<OS::PeriodicThread*> (t);

        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        const char* modname = task->getName();
        {
            Logger::In in(modname);
            Logger::log() << Logger::Debug << rtos_task_get_name(task->getTask()) <<" created with priority "<<rtos_task_get_priority(task->getTask())<<"."<<Logger::endl;
            Logger::log() << Logger::Debug << "(Priority "<<task->getPriority()<<".)"<<Logger::endl;
        }
#endif

#ifdef OROPKG_OS_THREAD_SCOPE
#ifdef OROINT_OS_THREAD_SCOPE_THREAD_ORDER
	// order thread scope toggle bit on thread number
	unsigned int bit = task->threadNumber();
#else
        // order thread scope toggle bit on priority
        unsigned int bit = task->getPriority();
#endif

        boost::scoped_ptr<DigitalOutInterface> pp;
        DigitalOutInterface* d = 0;
#ifndef ORO_EMBEDDED
        try {
#endif // !ORO_EMBEDDED
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                d = DigitalOutInterface::nameserver.getObject("ThreadScope");
            else
# ifdef OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER
                {
                    pp.reset( new OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER() );
                    d = pp.get();
                }
# else
            {
                Logger::In in(modname);
                Logger::log() << Logger::Warning<< "Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
            }
# endif
#ifndef ORO_EMBEDDED
        } catch( ... )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::In in(modname);
                Logger::log() << Logger::Error<< "Failed to create ThreadScope." << Logger::endl;
#endif // !ORO_EMBEDDED
            }
#endif
        if ( d ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in(modname);
            Logger::log() << Logger::Info
                          << "ThreadScope :"<< rtos_task_get_name(task->getTask()) <<" toggles bit "<< bit << Logger::endl;
#endif
            d->switchOff( bit );
        }
#endif
        int overruns = 0;
        while ( !task->prepareForExit ) {
#ifndef ORO_EMBEDDED
            try {
#endif // !ORO_EMBEDDED
                /**
                 * The real task starts here.
                 */
                while(1) 
                    {
                        if( !task->running ) { // more efficient than calling isRunning()
                            // consider this the 'configuration state'
                            overruns = 0;
                            // drop out of periodic mode:
                            rtos_task_set_period(task->getTask(), 0);
	      //                             Logger::log() << Logger::Info <<rtos_task_get_name(&rtos_task)<<"  signals done !" << Logger::endl;
                            rtos_sem_signal( &(task->confDone) ); // signal we are ready for command
	      //                             Logger::log() << Logger::Info <<rtos_task_get_name(&rtos_task)<<"  sleeps !" << Logger::endl;
                            rtos_sem_wait( &(task->sem) );      // wait for command.
	      //                             Logger::log() << Logger::Info <<rtos_task_get_name(&rtos_task)<<"  awoken !" << Logger::endl;
                            task->configure();             // check for reconfigure
	      //                             Logger::log() << Logger::Info <<rtos_task_get_name(&rtos_task)<<"  config done !" << Logger::endl;
                            if ( task->prepareForExit )    // check for exit
                                {
                                    break; // break while(1) {}
                                }
                            // end of configuration
                        } else {
                            // task->isRunning()
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( d )
                                d->switchOn( bit );
#endif
                            {
#ifdef OROPKG_CORELIB_REPORTING
                                Logger::In in(modname);
#endif
                                task->step(); // one cycle
                            }
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( d )
                                d->switchOff( bit );
#endif
                            // return non-zero to indicate overrun.
                            if ( rtos_task_wait_period( task->getTask() ) != 0) {
                                ++overruns;
                                if ( overruns == task->maxOverRun )
                                    break;
                            } else if ( overruns != 0 )
                                --overruns;
                        }
                    }
                if ( overruns == task->maxOverRun ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                    if ( d )
                        d->switchOff( bit );
#endif
            task->emergencyStop();
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in(modname);
            Logger::log() << Logger::Fatal << rtos_task_get_name(task->getTask()) <<" got too many periodic overruns in step() ("<< overruns << " times), stopped Thread !"<<Logger::nl;
            Logger::log() <<" See PeriodicThread::setMaxOverrun() for info." << Logger::endl;
#endif
		}
#ifndef ORO_EMBEDDED
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( d )
                    d->switchOff( bit );
#endif
                task->emergencyStop();
#ifdef OROPKG_CORELIB_REPORTING
                Logger::In in(modname);
                Logger::log() << Logger::Fatal << rtos_task_get_name(task->getTask()) <<" caught a C++ exception, stopped thread !"<<Logger::endl;
#endif
            }
#endif // !ORO_EMBEDDED
        } // while (!prepareForExit)
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rtos_task_make_soft_real_time( task->getTask() );

#ifdef OROPKG_CORELIB_REPORTING
        Logger::In in(modname);
        Logger::log() << Logger::Debug << rtos_task_get_name(task->getTask()) <<" exiting."<<Logger::endl;
#endif

        rtos_sem_signal( &(task->confDone));

        return 0;
    }

  void PeriodicThread::emergencyStop()
  {
    // set state to not running
    this->running = false;
    // execute finalize in current mode, even if hard.
    this->finalize();
    // this is not strictly required...
    if ( this->isHardRealtime() )
      rtos_task_make_soft_real_time( &rtos_task );
  }

  PeriodicThread::PeriodicThread(int _priority, 
				 const std::string & name, 
				 Seconds periods, 
				 RunnableInterface* r) :
      running(false), goRealtime(false), prepareForExit(false),
      wait_for_step(true), runComp(r), 
      maxOverRun( OROSEM_OS_PERIODIC_THREADS_MAX_OVERRUN)
    {
        int ret;
        
        ret = rtos_sem_init(&sem, 0);
        if ( ret != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not allocate configuration semaphore 'sem' for "<< rtos_task_get_name(&rtos_task) <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
            rtos_sem_destroy( &sem );
#ifndef ORO_EMBEDDED
            throw std::bad_alloc();
#else
            return;
#endif
        }

        ret = rtos_sem_init(&confDone, 0);
        if ( ret != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not allocate configuration semaphore 'confDone' for "<< rtos_task_get_name(&rtos_task) <<". Throwing std::bad_alloc."<<Logger::endl;
#endif
            rtos_sem_destroy( &sem );
#ifndef ORO_EMBEDDED
            throw std::bad_alloc();
#else
            return;
#endif
        }

        // Do not call setPeriod(), since the semaphores are not yet used !
        period = Seconds_to_nsecs(periods);

        if (runComp)
            runComp->setThread(this);
    int rv = rtos_task_create(&rtos_task, _priority, name.c_str(),OROSEM_OS_SCHEDTYPE,periodicThread, this );
    if ( rv != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not create thread "
                          << rtos_task_get_name(&rtos_task) <<"."<<Logger::endl;
#endif
      rtos_sem_destroy( &sem );
      rtos_sem_destroy( &confDone );
#ifndef ORO_EMBEDDED
      throw std::bad_alloc();
#else
            return;
#endif
    }
        
        rtos_sem_wait(&confDone);
    }
    
    PeriodicThread::~PeriodicThread() 
    {
#ifdef OROPKG_CORELIB_REPORTING
    Logger::In in("~PeriodicThread");
#endif
        if (isRunning()) stop();

        terminate();
        rtos_sem_destroy(&confDone);
        rtos_sem_destroy(&sem);

	if (runComp)
	  runComp->setThread(0);

    }

    bool PeriodicThread::run( RunnableInterface* r)
    {
        if ( isRunning() )
            return false;
	if (runComp)
	  runComp->setThread(0);
        runComp = r;
	if (runComp)
	  runComp->setThread(this);
        return true;
    }


    bool PeriodicThread::start() 
    {
        if ( running ) return false;

#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" started."<<Logger::endl;
#endif

        bool result;
        result = this->initialize();

        if (result == false) {
#ifdef OROPKG_CORELIB_REPORTING
      Logger::log() << Logger::Critical << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" failed to initialize()."<<Logger::endl;
#endif
            return false;
        }

        running=true;

        // be sure that confDone is set to zero
        // could be in case stop() times out.
        rtos_sem_trywait( &confDone );
        // signal start :
    rtos_task_make_periodic(&rtos_task, period );
        int ret = rtos_sem_signal(&sem);
#ifdef OROPKG_CORELIB_REPORTING
        if ( ret != 0 )
            Logger::log() << Logger::Critical <<"PeriodicThread::start(): sem_signal returns "<< ret << Logger::endl;
#endif
        // do not wait, we did our job.

        return true;
    }

    bool PeriodicThread::stop() 
    {
        if ( !running ) return false;

#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" stopping...";
#endif

        running=false;
        int cnt = 0;
        int ret = -1;

        // wait until the loop detects running == false
        // we wait 10 times 5*period.
        while ( ret != 0 && cnt < 10 )
            {
                // given time in argument is relative to 'now'
                ret = rtos_sem_wait_timed( &confDone, 5*period );
                // if ret == 0, confDone was signaled.
                cnt++;
            } 

#ifdef OROPKG_CORELIB_REPORTING
        if ( ret != 0 ) {
            Logger::log() << Logger::Debug << " failed."<<Logger::endl;
      Logger::log() << Logger::Critical << "The "<< rtos_task_get_name(&rtos_task) <<" thread seems to be blocked ( ret was "<< ret<<" .)"<<Logger::nl;
        }
        else
            Logger::log() << Logger::Debug << " done."<<Logger::endl;
#endif
        // drop out of periodic mode.
    rtos_task_make_periodic(&rtos_task, 0);

        //std::cout <<"Finalizing thread after "<<cnt<<" tries !"<<std::endl;
        // from now on, the thread waits on sem.

        this->finalize();

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
	  Logger::log() << Logger::Debug << "Thread "<< rtos_task_get_name(&rtos_task) <<" already Hard Realtime...";
#endif
                    return true;
                }

#ifdef OROPKG_CORELIB_REPORTING
	Logger::log() << Logger::Debug << "Making "<< rtos_task_get_name(&rtos_task) <<" Hard Realtime...";
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
      Logger::log() << Logger::Warning << "Failed to make "<< rtos_task_get_name(&rtos_task) <<" Hard Realtime since thread is still running."<<Logger::endl;
#endif
        return goRealtime; 
    }

    bool PeriodicThread::makeSoftRealtime()
    { 
        if ( !running ) 
            {
                if ( !isHardRealtime() ) {
#ifdef OROPKG_CORELIB_REPORTING
	  Logger::log() << Logger::Debug << "Thread "<< rtos_task_get_name(&rtos_task) <<" already Soft Realtime...";
#endif
                    return true;
                }
#ifdef OROPKG_CORELIB_REPORTING
	Logger::log() << Logger::Debug << "Making "<< rtos_task_get_name(&rtos_task) <<" Soft Realtime...";
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
      Logger::log() << Logger::Warning << "Failed to make "<< rtos_task_get_name(&rtos_task) <<" Soft Realtime since thread is still running."<<Logger::endl;
#endif
        return !goRealtime; 
    }

  bool PeriodicThread::isHardRealtime() const { return rtos_task_is_hard_real_time(&rtos_task); }

    void PeriodicThread::configure()
    {
        // reconfigure period
        if ( wait_for_step )
      rtos_task_set_period(&rtos_task, period );
        else
      rtos_task_set_period(&rtos_task, 0);
        if ( goRealtime && !isHardRealtime() )
            {
                //std::cout <<"Going HRT!"<<std::endl;
	rtos_task_make_hard_real_time(&rtos_task);
            }
        else if ( !goRealtime && isHardRealtime() )
            {
	rtos_task_make_soft_real_time(&rtos_task);
                //std::cout <<"Returning to SRT!"<<std::endl;
            }
    // Only works for posix OS-es !!
    /*
        if ( !isHardRealtime() ) { // reconfigure scheduler type.
            int type = sched_getscheduler(0);
            if ( type != sched_type )
                this->reconfigScheduler();
        }
    */
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

        period = Seconds_to_nsecs(s);

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
    return rtos_task_get_priority(&rtos_task);
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
    
    rtos_sem_wait(&confDone);
    rtos_task_delete(&rtos_task);

    // if ( pthread_join(thread,0) != 0 ) 
    // #ifdef OROPKG_CORELIB_REPORTING
    // Logger::log() << Logger::Critical << "Failed to join "<< rtos_task_get_name(&rtos_task) <<"."<< Logger::endl;
    // #else
    //     ;
    // #endif
    }

    bool PeriodicThread::setToStop()
    {
        running = false;
        this->finalize();
        return true;
    }

    const char* PeriodicThread::getName() const
    {
    return rtos_task_get_name(&rtos_task);
    }

  void PeriodicThread::setMaxOverrun( int m )
  {
    maxOverRun = m;
  }

  int PeriodicThread::getMaxOverrun() const
  {
    return maxOverRun;
  }

}}
    
