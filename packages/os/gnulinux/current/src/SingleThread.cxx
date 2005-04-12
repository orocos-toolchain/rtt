/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  SingleThread.cxx 

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
        /**
         * This is one time initialisation
         */

        SingleThread* task = static_cast<ORO_OS::SingleThread*> (t);

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" created."<<Logger::endl;
#endif
        sem_post( &task->confDone );

        /**
         * The real task starts here.
         */
        while ( !task->prepareForExit ) {
            try {
                while(1) {
                    task->running = false;

                    sem_wait( &task->sem );

                    if ( task->prepareForExit )
                        break;

                    task->loop();

                    task->finalize();
                }
            } catch( ... ) {
                // set state to not running
                task->running = false;
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Fatal << "Single Thread "<< task->taskName <<" caught a C++ exception, stopping thread !"<<Logger::endl;
#endif
                task->finalize();
            }
        }
    
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Single Thread "<< task->taskName <<" exiting."<<Logger::endl;
#endif
        /**
         * Cleanup stuff
         */
        sem_post( &task->confDone );
        return 0;
    }

    SingleThread::SingleThread(int _priority, const std::string& name, RunnableInterface* r) :
        running(false), priority(_priority), prepareForExit(false),
        runComp(r)
    {
        if ( !name.empty() )
            taskNameSet(name.c_str());
        else
            taskNameSet("unnamed");

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "SingleThread: Creating "<< taskName <<"."<<Logger::endl;
#endif

        sem_init( &confDone, 0, 0);
        sem_init( &sem, 0, 0 );
        pthread_create( &thread, 0, singleThread_f, this);
        sem_wait( &confDone );
    }
    
    SingleThread::~SingleThread() 
    {
        this->stop();

        // Send the message to the thread...
        prepareForExit = true;
        sem_post( &sem );
        
        // Wait for the 'ok' answer of the thread.
        sem_wait( &confDone );
        sem_destroy( &confDone );
        sem_destroy( &sem );


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

        this->initialize();

        running=true;
        sem_post( &sem );

        return true;
    }

    bool SingleThread::stop() 
    {
        if ( !isRunning() ) return false;

        return this->breakLoop();
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

    void SingleThread::taskNameSet(const char* nm)
    {
        snprintf(taskName,TASKNAMESIZE,"%s",nm);
    }

    const char* SingleThread::taskNameGet() const
    {
        return taskName;
    }
   
};
    
