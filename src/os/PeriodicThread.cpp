/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  PeriodicThread.cxx 

                        PeriodicThread.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "os/fosi_internal.hpp"
#include <os/PeriodicThread.hpp>
#include <os/Time.hpp>
#include "os/threads.hpp"
#include "Logger.hpp"

#include "../rtt-config.h"

#ifdef OROPKG_OS_THREAD_SCOPE
# include "dev/DigitalOutInterface.hpp"
#endif

namespace RTT
{ namespace OS {
    using RTT::Logger;
    using namespace detail;

    void *periodicThread(void* t) 
    {
        /**
         * This is one time initialisation
         */
        PeriodicThread* task = static_cast<OS::PeriodicThread*> (t);
        // acquire the resulting scheduler type.
        //task->msched_type = rtos_task_get_scheduler( task->getTask() );
        task->configure();

#ifdef OROPKG_OS_THREAD_SCOPE
        // order thread scope toggle bit on thread number
        unsigned int bit = task->threadNumber();
        if ( task->d )
            task->d->switchOff( bit );
#endif // OROPKG_OS_THREAD_SCOPE
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
                            if ( task->d )
                                task->d->switchOn( bit );
#endif
                            {
                                task->step(); // one cycle
                            }
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( task->d )
                                task->d->switchOff( bit );
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
                    if ( task->d )
                        task->d->switchOff( bit );
#endif
                    task->emergencyStop();
                    Logger::In in(rtos_task_get_name(task->getTask()));
                    Logger::log() << Logger::Fatal << rtos_task_get_name(task->getTask()) <<" got too many periodic overruns in step() ("<< overruns << " times), stopped Thread !"<<Logger::nl;
                    Logger::log() <<" See PeriodicThread::setMaxOverrun() for info." << Logger::endl;
                }
#ifndef ORO_EMBEDDED
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( task->d )
                    task->d->switchOff( bit );
#endif
                task->emergencyStop();
                Logger::In in(rtos_task_get_name(task->getTask()));
                Logger::log() << Logger::Fatal << rtos_task_get_name(task->getTask()) <<" caught a C++ exception, stopped thread !"<<Logger::endl;
            }
#endif // !ORO_EMBEDDED
        } // while (!prepareForExit)
    
        rtos_sem_signal( &(task->confDone));

        return 0;
    }

    void PeriodicThread::emergencyStop()
    {
        // set state to not running
        this->running = false;
        // execute finalize in current mode, even if hard.
        this->finalize();
    }

    PeriodicThread::PeriodicThread(int _priority, 
                                   const std::string & name, 
                                   Seconds periods, 
                                   RunnableInterface* r) :
        msched_type(ORO_SCHED_RT), running(false), prepareForExit(false),
        wait_for_step(true), runComp(r), 
        maxOverRun( OROSEM_OS_PERIODIC_THREADS_MAX_OVERRUN),
        period(Seconds_to_nsecs(periods))    // Do not call setPeriod(), since the semaphores are not yet used !
#ifdef OROPKG_OS_THREAD_SCOPE
							 ,d(NULL)
#endif
    {
        this->setup(_priority, name);
    }

    PeriodicThread::PeriodicThread(int scheduler, int _priority, 
                                   const std::string & name, 
                                   Seconds periods, 
                                   RunnableInterface* r) :
        msched_type(scheduler), running(false), prepareForExit(false),
        wait_for_step(true), runComp(r), 
        maxOverRun( OROSEM_OS_PERIODIC_THREADS_MAX_OVERRUN),
        period(Seconds_to_nsecs(periods))    // Do not call setPeriod(), since the semaphores are not yet used !
#ifdef OROPKG_OS_THREAD_SCOPE
							 ,d(NULL)
#endif
    {
        log(Info) << "Creating PeriodicThread for scheduler: "<< scheduler << endlog();
        this->setup(_priority, name);
    }

    void PeriodicThread::setup(int _priority, const std::string& name) 
    {
        int ret;
        
        ret = rtos_sem_init(&sem, 0);
        if ( ret != 0 ) {
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not allocate configuration semaphore 'sem' for "<< rtos_task_get_name(&rtos_task) <<". Throwing std::bad_alloc."<<Logger::endl;
            rtos_sem_destroy( &sem );
#ifndef ORO_EMBEDDED
            throw std::bad_alloc();
#else
            return;
#endif
        }

        ret = rtos_sem_init(&confDone, 0);
        if ( ret != 0 ) {
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not allocate configuration semaphore 'confDone' for "<< rtos_task_get_name(&rtos_task) <<". Throwing std::bad_alloc."<<Logger::endl;
            rtos_sem_destroy( &sem );
#ifndef ORO_EMBEDDED
            throw std::bad_alloc();
#else
            return;
#endif
        }

        if (runComp)
            runComp->setThread(this);
#ifdef OROPKG_OS_THREAD_SCOPE
        // Check if threadscope device already exists
        {
            Logger::In in("PeriodicThread");
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") ){
                d = DigitalOutInterface::nameserver.getObject("ThreadScope");
            }
            else{
                log(Warning) << "Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << endlog();
            }
        }
#endif
        int rv = rtos_task_create(&rtos_task, _priority, name.c_str(), msched_type, periodicThread, this );
        if ( rv != 0 ) {
            Logger::In in("PeriodicThread");
            Logger::log() << Logger::Critical << "Could not create thread "
                          << rtos_task_get_name(&rtos_task) <<"."<<Logger::endl;
            rtos_sem_destroy( &sem );
            rtos_sem_destroy( &confDone );
#ifndef ORO_EMBEDDED
            throw std::bad_alloc();
#else
            return;
#endif
        }
 
        rtos_sem_wait(&confDone); // wait until thread is created.
 
        const char* modname = getName();
        Logger::In in(modname);
        log(Info)<< "PeriodicThread created with scheduler type '"<< getScheduler() <<"', priority " << getPriority()
                 <<" and period "<< getPeriod() << "." << endlog();
#ifdef OROPKG_OS_THREAD_SCOPE
        if (d){
            unsigned int bit = threadNumber();
            log(Info) << "ThreadScope :"<< modname <<" toggles bit "<< bit << endlog();
        }
#endif
    }
    
    PeriodicThread::~PeriodicThread() 
    {
        Logger::In in("~PeriodicThread");
        if (this->isRunning())
            this->stop();

        log(Debug) << "Terminating "<< this->getName() <<endlog();
        terminate();
        log(Debug) << " done"<< endlog();
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

        Logger::log() << Logger::Debug << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" started."<<Logger::endl;

        bool result;
        result = this->initialize();

        if (result == false) {
            Logger::log() << Logger::Critical << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" failed to initialize()."<<Logger::endl;
            return false;
        }

        running=true;

        // be sure that confDone is set to zero
        // could be in case stop() times out.
        rtos_sem_trywait( &confDone );
        // signal start :
        rtos_task_make_periodic(&rtos_task, period );
        int ret = rtos_sem_signal(&sem);
        if ( ret != 0 )
            Logger::log() << Logger::Critical <<"PeriodicThread::start(): sem_signal returns "<< ret << Logger::endl;
        // do not wait, we did our job.

        return true;
    }

    bool PeriodicThread::stop() 
    {
        if ( !running ) return false;

        Logger::log() << Logger::Debug << "Periodic Thread "<< rtos_task_get_name(&rtos_task) <<" stopping...";

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

        if ( ret != 0 ) {
            Logger::log() << Logger::Debug << " failed."<<Logger::endl;
            Logger::log() << Logger::Critical << "The "<< rtos_task_get_name(&rtos_task) <<" thread seems to be blocked ( ret was "<< ret<<" .)"<<Logger::nl;
        }
        else
            Logger::log() << Logger::Debug << " done."<<Logger::endl;
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

    bool PeriodicThread::setScheduler(int sched_type)
    {
        if ( !running ) 
            {
                if ( this->getScheduler() == sched_type ) {
                    log(Debug) << "Scheduler type for Thread "<< rtos_task_get_name(&rtos_task) <<" is already configured as "<< sched_type << endlog();
                    return true;
                }

                log(Error) << "Setting scheduler type for Thread "<< rtos_task_get_name(&rtos_task) <<" to "<< sched_type;
                msched_type = sched_type; 
                rtos_sem_signal(&sem);
                rtos_sem_wait(&confDone);
            }
        else {
            log() << "Failed to change scheduler for "<< rtos_task_get_name(&rtos_task) <<" since thread is still running."<<endlog(Warning);
            return false;
        }
        if (msched_type != rtos_task_get_scheduler(&rtos_task) )
            {
                msched_type = rtos_task_get_scheduler(&rtos_task);
                log() << " failed."<<endlog(Error);
                return false;
            }
        log() << " done."<<endlog(Debug);
        return true;

#if 0 
        // Alternative is not possible for RTAI: can only rt_make_hard_realtime() of calling thread!
        if ( rtos_task_set_scheduler(&rtos_task, sched_type) == 0)
            return true;
        return false;
#endif
    }
    int PeriodicThread::getScheduler() const
    {
        return rtos_task_get_scheduler(&rtos_task);
    }

    void PeriodicThread::configure()
    {
        // reconfigure period
        if ( wait_for_step )
            rtos_task_set_period(&rtos_task, period );
        else
            rtos_task_set_period(&rtos_task, 0);

        if ( msched_type != rtos_task_get_scheduler(&rtos_task) )
            {
                rtos_task_set_scheduler( &rtos_task, msched_type );
                msched_type = rtos_task_get_scheduler(&rtos_task);
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

    bool PeriodicThread::setPriority(int p) 
    {
        return rtos_task_set_priority(&rtos_task, p) == 0;
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

    void PeriodicThread::yield() {
        rtos_task_yield( &rtos_task );
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
    
