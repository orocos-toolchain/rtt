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

// include OS-internal fosi as first file.
#include "rtt/os/fosi_internal.hpp"
#include <rtt/os/SingleThread.hpp>
#include <rtt/os/MutexLock.hpp>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif
#ifdef OROPKG_CORELIB_REPORTING
#include "rtt/Logger.hpp"
using RTT::Logger;
#endif

#include "pkgconf/os.h"

#include <iostream>

#include "rtt/Time.hpp"
#include "rtt/os/threads.hpp"
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


namespace RTT
{ namespace OS {
    using namespace detail;

    void *singleThread_f(void* t) 
    {
        /**
         * This is one time initialisation
         */
        SingleThread* task = static_cast<OS::SingleThread*> (t);

        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Single Thread "<< task->getName() <<" created."<<Logger::endl;
#endif

#ifdef OROPKG_OS_THREAD_SCOPE
#ifdef OROPKG_OS_THREAD_SCOPE_THREAD_ORDER
	// order thread scope toggle bit on thread number
	unsigned int bit = task->threadnb;
#else
        // order thread scope toggle bit on priority
        unsigned int bit = task->getPriority();
#endif

        boost::scoped_ptr<DigitalOutInterface> pp;
        DigitalOutInterface* d = 0;
        try {
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                d = DigitalOutInterface::nameserver.getObject("ThreadScope");
            else
# ifdef OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER
                {
                    pp.reset( new OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER("ThreadScope") );
                    d = pp.get();
                }
# else
                Logger::log() << Logger::Warning<< "SingleThread : Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
# endif
        } catch( ... )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Error<< "SingleThread : Failed to create ThreadScope." << Logger::endl;
#endif
            }
        if ( d ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Info
                          << "ThreadScope : Single Thread "<< task->getName() <<" toggles bit "<< bit << Logger::endl;
#endif
            d->switchOff( bit );
        }
#endif

        /**
         * The real task starts here.
         */

        if ( task->goRealtime )
      rtos_task_make_hard_real_time( task->getTask() );

        rtos_sem_signal( &(task->confDone) );

        while ( !task->prepareForExit ) {
            try {
                while(1) 
                    {
                        rtos_sem_wait( &(task->sem) );
                        if ( ! task->active ) {
                            if ( task->prepareForExit )
                                break;
                            // The configuration might have changed
                            // While waiting on the sem...
	      if ( task->goRealtime && !rtos_task_is_hard_real_time( task->getTask() ) )
		rtos_task_make_hard_real_time( task->getTask() );
	      if ( ! task->goRealtime && rtos_task_is_hard_real_time( task->getTask() ) )
		rtos_task_make_soft_real_time( task->getTask() );
                        } else {

#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( d )
                                d->switchOn( bit );
#endif
                            {
                                // this mutex guarantees that stop() waits
                                // until loop() returns.
                                MutexLock lock(task->breaker);
                                task->inloop = true;
                                task->loop();
                                task->inloop = false;
                            }
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( d )
                                d->switchOff( bit );
#endif
                        }
                    }
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( d )
                    d->switchOff( bit );
#endif
                if ( task->isHardRealtime() )
	  rtos_task_make_soft_real_time( task->getTask() );
                // set state to not running
                task->inloop = false;
                task->active = false;
#ifdef OROPKG_CORELIB_REPORTING
	Logger::log() << Logger::Fatal << "Single Thread "<< task->getName() <<" caught a C++ exception, stopping thread !"<<Logger::endl;
#endif
                task->finalize();
                if ( task->goRealtime )
	  rtos_task_make_hard_real_time( task->getTask() );
            }
        }
            
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
      rtos_task_make_soft_real_time( task->getTask() );

#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Single Thread "<< task->getName() <<" exiting."<<Logger::endl;
#endif

        rtos_sem_signal( &(task->confDone) );

        return 0;
    }

  SingleThread::SingleThread(int _priority, 
			     const std::string& name, 
			     RunnableInterface* r) :
    active(false), goRealtime(false), prepareForExit(false), 
    inloop(false), runComp(r)
    {
#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "SingleThread: Creating." <<Logger::endl;
#endif

        rtos_sem_init( &sem, 0 );
        rtos_sem_init( &confDone, 0 );
    int rv = rtos_task_create( &rtos_task, _priority, name.c_str(),OROSEM_OS_SCHEDTYPE,singleThread_f, this);
    if ( rv != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::In in("SingleThread");
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
        rtos_sem_wait( &confDone );
    }
    
    SingleThread::~SingleThread() 
    {
#ifdef OROPKG_CORELIB_REPORTING
        Logger::In in("~SingleThread");
#endif
        if ( this->isRunning() && this->stop() == false )
#ifdef OROPKG_CORELIB_REPORTING
      Logger::log() << Logger::Error << "Failed to stop thread "<< getName() <<"."<<Logger::endl;
#else
        ; // intentional !
#endif


        // Send the message to the thread...
        active = false;
        prepareForExit = true;
        rtos_sem_signal( &sem );
        
        // Wait for the 'ok' answer of the thread.
        rtos_sem_wait( &confDone );
    // Delete the task
    rtos_task_delete(&rtos_task);

        rtos_sem_destroy( &confDone );
        rtos_sem_destroy( &sem );

    }

    bool SingleThread::makeHardRealtime() 
    { 
        // This construct is so because
        // the thread itself must call the proper RTAI function.
        if ( !active ) 
            {
                goRealtime = true;
                rtos_sem_signal(&sem);
            }
        return goRealtime; 
    }

    bool SingleThread::makeSoftRealtime()
    { 
        if ( !active ) 
            {
                goRealtime = false; 
                rtos_sem_signal(&sem);
            }
        return !goRealtime; 
    }


    bool SingleThread::run( RunnableInterface* r)
    {
        if ( isActive() )
            return false;
        runComp = r;
        return true;
    }

    bool SingleThread::isHardRealtime() const
    {
    return rtos_task_is_hard_real_time( &rtos_task );
    }

    bool SingleThread::start() 
    {
        // just signal if already active.
        if ( isActive() ) {
            rtos_sem_signal(&sem);
            return true;
        }

        active=true;
        if ( this->initialize() == false ) {
            active = false;
            return false;
        }

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
        if ( inloop ) { 
            if ( !this->breakLoop() )
                return false;
            // breakLoop was ok, wait for loop() to return.
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

  int SingleThread::getPriority() const
    {
    return rtos_task_get_priority(&rtos_task);
    }
   
}}
