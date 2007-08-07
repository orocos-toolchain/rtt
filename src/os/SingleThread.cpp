/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  SingleThread.cxx 

                        SingleThread.cxx -  description
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

// include OS-internal fosi as first file.
#include "os/fosi_internal_interface.hpp"
#include <os/SingleThread.hpp>
#include <os/MutexLock.hpp>

#include "Time.hpp"
#include "Logger.hpp"
#include "os/threads.hpp"

#include "../rtt-config.h"

#ifdef OROPKG_OS_THREAD_SCOPE
# include "dev/DigitalOutInterface.hpp"
#endif

namespace RTT
{ namespace OS {
    using namespace detail;

    void *singleThread_f(void* t) 
    {
        /**
         * This is one time initialisation
         */
        SingleThread* task = static_cast<OS::SingleThread*> (t);
        // acquire the resulting scheduler type.
        if ( task->msched_type != rtos_task_get_scheduler( task->getTask()) )
            {
                rtos_task_set_scheduler( task->getTask(), task->msched_type );
                task->msched_type = rtos_task_get_scheduler(task->getTask());
            }
        //task->msched_type = rtos_task_get_scheduler( task->getTask() );

#ifdef OROPKG_OS_THREAD_SCOPE
        // order thread scope toggle bit on thread number
        unsigned int bit = task->threadNumber();
        if ( task->d )
            task->d->switchOff( bit );
#endif // OROPKG_OS_THREAD_SCOPE

        /**
         * The real task starts here.
         */
        rtos_sem_signal( &(task->confDone) );

        while ( !task->prepareForExit ) {
            try {
                while(1) 
                    {
                        rtos_sem_wait( &(task->sem) );
                        // this mutex guarantees that stop() waits
                        // until loop() returns.
                        MutexLock lock(task->breaker);
                        if ( ! task->runloop ) {
                            if ( task->prepareForExit )
                                break;
                            // The configuration might have changed
                            // While waiting on the sem...
                            if ( task->msched_type != rtos_task_get_scheduler( task->getTask()) )
                                {
                                    rtos_task_set_scheduler( task->getTask(), task->msched_type );
                                    task->msched_type = rtos_task_get_scheduler(task->getTask());
                                }

                        } else {

#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( task->d )
                                task->d->switchOn( bit );
#endif
                            {
                                task->inloop = true;
                                task->loop();
                                task->inloop = false;
                            }
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( task->d )
                                task->d->switchOff( bit );
#endif
                        }
                    }
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( task->d )
                    task->d->switchOff( bit );
#endif
                // set state to not running
                task->inloop = false;
                task->active = false;
                Logger::log() << Logger::Fatal << "Single Thread "<< task->getName() <<" caught a C++ exception, stopping thread !"<<Logger::endl;
                task->finalize();
            }
        }
            
        rtos_sem_signal( &(task->confDone) );

        return 0;
    }

  SingleThread::SingleThread(int _priority, 
                             const std::string& name, 
                             RunnableInterface* r) 
      : msched_type(ORO_SCHED_RT),
        active(false), prepareForExit(false), 
        inloop(false), runloop(false), runComp(r)
#ifdef OROPKG_OS_THREAD_SCOPE
        , d(NULL)
#endif
    {
        this->setup(_priority, name);
    }

    SingleThread::SingleThread(int scheduler, int _priority, 
                               const std::string& name, 
                               RunnableInterface* r)
        : msched_type(scheduler),
          active(false), prepareForExit(false), 
          inloop(false), runloop(false), runComp(r)
#ifdef OROPKG_OS_THREAD_SCOPE
        , d(NULL)
#endif
    {
        this->setup(_priority, name);
    }

    void SingleThread::setup(int _priority, const std::string& name) 
    {
        Logger::In in("SingleThread");
        Logger::log() << Logger::Debug << "Creating." <<Logger::endl;

        rtos_sem_init( &sem, 0 );
        rtos_sem_init( &confDone, 0 );
	
#ifdef OROPKG_OS_THREAD_SCOPE
        // Check if threadscope device already exsists
        {
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") ){
                d = DigitalOutInterface::nameserver.getObject("ThreadScope");
	    }
	    else{
                log(Warning) << "SingleThread : Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << endlog();
            }
	}
#endif
	int rv = rtos_task_create( &rtos_task, _priority, name.c_str(), msched_type, singleThread_f, this);
	if ( rv != 0 ) {
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
	rtos_sem_wait( &confDone ); // wait until thread is created !

    if (runComp)
        runComp->setThread(this);
	
	const char* modname = getName();
	Logger::In in2(modname);
	log(Info)<< "SingleThread created with priority " << getPriority()
		 << " and period " << getPeriod() << "." << endlog();
	log(Info) << "Scheduler type was set to `"<< getScheduler() << "'."<<endlog();

#ifdef OROPKG_OS_THREAD_SCOPE
	if (d){
	  unsigned int bit = threadNumber();
	  log(Info) << "ThreadScope :"<< modname <<" toggles bit "<< bit << endlog();
	}
#endif
    }
    
    SingleThread::~SingleThread() 
    {
        Logger::In in("~SingleThread");
        if ( this->isRunning() && this->stop() == false )
            Logger::log() << Logger::Error << "Failed to stop thread "<< getName() <<"."<<Logger::endl;


        // Send the message to the thread...
        active = false;
        runloop = false;
        prepareForExit = true;
        rtos_sem_signal( &sem );
        
        // Wait for the 'ok' answer of the thread.
        rtos_sem_wait( &confDone );
        log(Debug) << "Terminating "<< this->getName();
        // Delete the task, it's name etc...
        rtos_task_delete(&rtos_task);
        log(Debug) << " done."<< endlog();

        rtos_sem_destroy( &confDone );
        rtos_sem_destroy( &sem );

        if (runComp)
            runComp->setThread(0);

    }

    bool SingleThread::setScheduler(int sched_type)
    {
        if ( !active ) 
            {
                msched_type = sched_type;
                rtos_sem_signal(&sem);
                return true;
            }
        log() << "Failed to change scheduler for "<< rtos_task_get_name(&rtos_task) <<" since thread is still running."<<endlog(Warning);
        return false;

#if 0 
        // Alternative is not possible for RTAI: can only rt_make_hard_realtime() of current thread!
        if ( rtos_task_set_scheduler(&rtos_task, sched_type) == 0)
            return true;
        return false;
#endif
    }

    int SingleThread::getScheduler() const
    {
        return rtos_task_get_scheduler(&rtos_task);
    }

    bool SingleThread::run( RunnableInterface* r)
    {
        if ( isActive() )
            return false;
        if (runComp)
            runComp->setThread(0);
        runComp = r;
        if (runComp)
            runComp->setThread(this);
        return true;
    }

    bool SingleThread::start() 
    {
        // just signal if already active.
        if ( isActive() ) {
            if ( rtos_sem_value(&sem) > 0 )
                return false;
            rtos_sem_signal(&sem);
            return true;
        }

        active=true;
        if ( this->initialize() == false ) {
            active = false;
            return false;
        }

        runloop = true;
        rtos_sem_signal(&sem);

        return true;
    }

    void SingleThread::yield() {
        rtos_task_yield( &rtos_task );
    }

    bool SingleThread::stop() 
    {
        if ( !isActive() ) return false;

        // if inloop and breakloop does not work, sorry.
        {
            runloop = false;
            if ( inloop ) { 
                if ( !this->breakLoop() ) {
                    runloop = true;
                    return false;
                }
                // breakLoop was ok, wait for loop() to return.
            }
            // always take this lock, but after breakLoop was called !
            MutexLock lock(breaker);
        }

        // from this point on, inloop == false

        this->finalize();
        active = false;
        return true;
    }

    bool SingleThread::isRunning() const
    {
        return inloop;
    }

    bool SingleThread::isActive() const
    {
        return active;
    }

    void SingleThread::loop()
    {
        if ( runComp != 0 )
            runComp->loop();
    }

    bool SingleThread::breakLoop()
    {
        if ( runComp != 0 )
            return runComp->breakLoop();
        return false;
    }

    bool SingleThread::initialize()
    { 
        if ( runComp != 0 )
            return runComp->initialize();
        return true;
    }

    void SingleThread::finalize()
    {
        if ( runComp != 0 )
            runComp->finalize();
    }

    const char* SingleThread::getName() const
    {
        return rtos_task_get_name(&rtos_task);
    }
    
    bool SingleThread::setPriority(int p) 
    {
        return rtos_task_set_priority(&rtos_task, p) == 0;
    }

    int SingleThread::getPriority() const
    {
        return rtos_task_get_priority(&rtos_task);
    }
   
}}
