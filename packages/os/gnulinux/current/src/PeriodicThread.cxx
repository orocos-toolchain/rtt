/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  PeriodicThread.cxx 

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

// TODO: check if the RunnableInterface always gets initialized and finalized.

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_EVENTS
#include <corelib/Event.hpp>

using boost::bind;

namespace
{
    // our internal event to stop a thread.
    ORO_CoreLib::Event<bool(void)> stopEvent;
}

#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
#endif

#include "os/PeriodicThread.hpp"



namespace ORO_OS
{
#ifdef OROPKG_CORELIB_REPORTING
    using ORO_CoreLib::Logger;
#endif

    void *ComponentThread( void *t )
    {
        PeriodicThread* comp = ( PeriodicThread* ) t;

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< comp->taskName <<" created."<<Logger::endl;
#endif
        while ( 1 )
        {
            while ( !comp->isRunning() && !comp->timeToQuit ) {
                sem_post( &comp->msyncer );
                sem_wait( &comp->tsyncer );
            }
            if (comp->timeToQuit)
                break;

            comp->step(); // one cycle

            if (comp->wait_for_step)
                comp->periodWaitRemaining();
        }
        
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< comp->taskName <<" exiting."<<Logger::endl;
#endif
        return 0;
    }

    PeriodicThread::PeriodicThread(int , const std::string& name, double period, RunnableInterface* r )
        : runner( r ), periodMark( 0 ), running( false ), timeToQuit(false), wait_for_step(true)
    {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h = new ORO_CoreLib::Handle();
#endif
        taskNameSet( name.c_str() );
        periodSet( period );
        sem_init( &tsyncer, 0, 0 );
        sem_init( &msyncer, 0, 0 );
        pthread_attr_init( threadAttributeGet() );
        pthread_create( &thread, threadAttributeGet(), ComponentThread, this );
        sem_wait( &msyncer );
    }

    PeriodicThread::~PeriodicThread()
    {
        if ( isRunning() )
            stop();

        terminate();

        // should wait until step is completed and exit then... ( FSM )
        //pthread_join(&thread); // join is not recommended
        pthread_attr_destroy( threadAttributeGet() );
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        delete h;
#endif
    }

    bool PeriodicThread::start()
    {
        if ( isRunning() )
            return false;

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< taskName <<" started."<<Logger::endl;
#endif

        running = initialize();

#ifdef OROPKG_CORELIB_REPORTING
        if ( running == false )
            Logger::log() << Logger::Critical << "Periodic Thread "<< taskName <<" failed to initialize()."<<Logger::endl;
#endif

        if ( running ) {
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
            *h = stopEvent.connect( bind( &PeriodicThread::stop, this ), ORO_CoreLib::CompletionProcessor::Instance() );
#endif
            sem_post( &tsyncer );
        }
        return running;
    }

    bool PeriodicThread::stop()
    {
        if ( !isRunning() )
            return false;

#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Debug << "Periodic Thread "<< taskName <<" stopping."<<Logger::endl;
#endif

        running = false;
        sem_wait( &msyncer );
        finalize();

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        h->disconnect();
#endif
        return true;
    }

    bool PeriodicThread::setToStop()
    {
        // finalize will be called in the thread of the
        // Completion processor
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        stopEvent();
        return true;
#else
        return false;
#endif
    }


    int PeriodicThread::periodSet( double s )
    {
        if ( isRunning() )
            return -1;

        period.tv_sec = long(s);

        period.tv_nsec = long( (s - period.tv_sec )* 1000*1000*1000);

        return 0;
    }

    int PeriodicThread::periodSet( const secs s, const nsecs ns )
    {
        if ( isRunning() )
            return -1;

        period.tv_sec = s;

        period.tv_nsec = ns;

        return 0;
    }

    int PeriodicThread::periodSet( TIME_SPEC p )
    {
        if ( isRunning() )
            return -1;

        period = p;

        return 0;
    }

    void PeriodicThread::step()
    {
        if ( runner )
            runner->step();
    }

    bool PeriodicThread::initialize()
    {
        if ( runner )
            return runner->initialize();
        return true;
    }

    void PeriodicThread::finalize()
    {
        if ( runner )
            runner->finalize();
    }

    void PeriodicThread::periodGet( secs& s, nsecs& ns ) const
    {
        s = period.tv_sec;
        ns = period.tv_nsec;
    }

    PeriodicThread::Seconds PeriodicThread::periodGet() const
    {
        return ( double ) period.tv_sec + ( double ) period.tv_nsec / ( 1000.0 * 1000.0 * 1000.0 );
    }

    void PeriodicThread::terminate()
    {
        // by PS : use this when proper 2-phase stuff works
        timeToQuit = true;
        sem_post(&tsyncer);
        pthread_join(thread, 0);
        //pthread_cancel( thread );
    }

    void PeriodicThread::periodWait()
    {
        rtos_nanosleep( &period, NULL ); // stay posix compatible
    }

    void PeriodicThread::continuousStepping(bool yes_no)
    {
        wait_for_step = !yes_no;
    }

    // this is non POSIX and will be removed later on
    // when time events come in
    void PeriodicThread::periodWaitRemaining()
    {
        if ( periodMark == 0 )
        {
            periodMark = rtos_get_time_ns();
        }

        // CALCULATE in nsecs
        NANO_TIME timeRemaining = period.tv_sec * 1000000000LL + period.tv_nsec - ( rtos_get_time_ns() - periodMark );

        //rtos_printf("Waiting for %d nsec\n",timeRemaining);

        TIME_SPEC ts( ticks2timespec( timeRemaining ) );
        if ( timeRemaining > 0 )
            rtos_nanosleep( &ts , NULL );
        else
            ;//rtos_printf( "%s did not get deadline !", taskNameGet() );

        periodMark = rtos_get_time_ns() ;

    }

    pthread_attr_t* PeriodicThread::threadAttributeGet()
    {
        return & threadAttribute;
    }


    void PeriodicThread::taskNameSet( const char* nm )
    {
        if ( strlen( nm ) < TASKNAME_SIZE )
            snprintf( taskName, TASKNAME_SIZE - 1, "%s", nm );
    }

    const char* PeriodicThread::taskNameGet() const
    {
        return taskName;
    }
}

