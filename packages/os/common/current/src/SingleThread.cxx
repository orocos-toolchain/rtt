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
#include "os/fosi_internal.hpp"
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

#include "pkgconf/os.h"

#include <iostream>

#include "corelib/Time.hpp"
#include "os/threads.hpp"
#ifdef OROPKG_DEVICE_INTERFACE
#include "pkgconf/device_interface.h"
#ifdef OROPKG_OS_THREAD_SCOPE
#include ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
using namespace ORO_DeviceInterface;
using namespace ORO_DeviceDriver;
#include <boost/scoped_ptr.hpp>
#endif
#endif

namespace ORO_OS 
{
    using namespace detail;

    void *singleThread_f(void* t) 
    {
        /**
         * This is one time initialisation
         */
        SingleThread* task = static_cast<ORO_OS::SingleThread*> (t);

        task->rtos_task = detail::rtos_task_init( task );
    
        // Reporting available from this point :
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" created."<<Logger::endl;
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
        try {
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                pp.reset( DigitalOutInterface::nameserver.getObject("ThreadScope") );
            else
# ifdef OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER
                pp.reset( new OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER("ThreadScope") );
# else
                Logger::log() << Logger::Error<< "SingleThread : Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
# endif
        } catch( ... )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Error<< "SingleThread : Failed to create ThreadScope." << Logger::endl;
#endif
            }
        if ( pp ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Info
                          << "ThreadScope : Single Thread "<< task->taskName <<" toggles bit "<< bit << Logger::endl;
#endif
            pp->switchOff( bit );
        }
#endif

        /**
         * The real task starts here.
         */

        if ( task->goRealtime )
            rtos_task_make_hard_real_time( task->rtos_task );

        rtos_sem_signal( &(task->confDone) );

        while ( !task->prepareForExit ) {
            try {
                while(1) 
                    {
                        rtos_sem_wait( &(task->sem) );
                        if ( ! task->running ) {
                            if ( task->prepareForExit )
                                break;
                            // The configuration might have changed
                            // While waiting on the sem...
                            if ( task->goRealtime && !rtos_task_is_hard_real_time( task->rtos_task ) )
                                rtos_task_make_hard_real_time( task->rtos_task );
                            if ( ! task->goRealtime && rtos_task_is_hard_real_time( task->rtos_task ) )
                                rtos_task_make_soft_real_time( task->rtos_task );
                        } else {

#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( pp )
                                pp->switchOn( bit );
#endif
                            task->loop();
#ifdef OROPKG_OS_THREAD_SCOPE
                            if ( pp )
                                pp->switchOff( bit );
#endif

                            // We do this to be able to safely
                            // process the finalize method
                            if ( task->isHardRealtime() )
                                rtos_task_make_soft_real_time( task->rtos_task );

                            task->finalize();
                            task->running = false;
                            // if goRealtime, wait on sem in RT.
                            if ( task->goRealtime )
                                rtos_task_make_hard_real_time( task->rtos_task );
                        }
                    }
            } catch( ... ) {
#ifdef OROPKG_OS_THREAD_SCOPE
                if ( pp )
                    pp->switchOff( bit );
#endif
                if ( task->isHardRealtime() )
                    rtos_task_make_soft_real_time( task->rtos_task );
                // set state to not running
                task->running = false;
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Fatal << "Single Thread "<< task->taskName <<" caught a C++ exception, stopping thread !"<<Logger::endl;
#endif
                task->finalize();
                if ( task->goRealtime )
                    rtos_task_make_hard_real_time( task->rtos_task );
            }
        }
            
    
        /**
         * Cleanup stuff
         */
        if ( task->isHardRealtime() )
            rtos_task_make_soft_real_time( task->rtos_task );

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif

        rtos_sem_signal( &(task->confDone) );
        detail::rtos_task_delete( task->rtos_task );
        return 0;
    }

    SingleThread::SingleThread(int _priority, const std::string& name, RunnableInterface* r) :
        running(false), goRealtime(false), prepareForExit(false), priority(_priority),
        runComp(r)
    {
        rtos_thread_init( this, name);

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "SingleThread: Creating "<< taskName <<"."<<Logger::endl;
#endif

        rtos_sem_init( &sem, 0 );
        rtos_sem_init( &confDone, 0 );
        pthread_create( &thread, 0, singleThread_f, this);
        rtos_sem_wait( &confDone );
    }
    
    SingleThread::~SingleThread() 
    {
        this->stop();

        // Send the message to the thread...
        prepareForExit = true;
        rtos_sem_signal( &sem );
        
        // Wait for the 'ok' answer of the thread.
        rtos_sem_wait( &confDone );
        rtos_sem_destroy( &confDone );
        rtos_sem_destroy( &sem );

        if ( pthread_join(thread,0) != 0 ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Error << "SingleThread: Failed to join with thread "<< taskName <<"."<<Logger::endl;
#endif
        }
        else {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Debug << "SingleThread: Joined with thread "<< taskName <<"."<<Logger::endl;
#endif
        }
    }

    bool SingleThread::run( RunnableInterface* r)
    {
        if ( isRunning() )
            return false;
        runComp = r;
        return true;
    }

    bool SingleThread::isHardRealtime() const
    {
        return rtos_task_is_hard_real_time( rtos_task );
    }

    bool SingleThread::start() 
    {
        if ( isRunning() ) return false;

        this->initialize();

        running=true;
        rtos_sem_signal(&sem);

        return true;
    }

    bool SingleThread::stop() 
    {
        if ( !isRunning() ) return false;

        if ( this->breakLoop() == true )
            running = false;
        return !running;
    }

    bool SingleThread::isRunning() const
    {
        return running;
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

    void SingleThread::setName(const char* nm)
    {
        taskName = nm;
    }

    const char* SingleThread::getName() const
    {
        return taskName.c_str();
    }
   
}
