/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:42:36 CEST 2002  PeriodicThread.cpp 

                       PeriodicThread.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


// TODO: check if the RunnableInterface always gets initialized and finalized.

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_EVENTS
#include "corelib/EventCompleterInterface.hpp"
#endif

#include "os/PeriodicThread.hpp"

#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
#include "corelib/CompletionProcessor.hpp"

namespace ORO_OS
{

        class Finalizer : public ORO_CoreLib::EventCompleterInterface
        {
            PeriodicThread* parent;
        public:
            Finalizer( PeriodicThread* ct ) : parent( ct )
            {}

            virtual void completeEvent();
        };

    void Finalizer::completeEvent(void)
    {
        parent->stop();
    }
}

#endif


namespace ORO_OS
{

    void *ComponentThread( void *t )
    {
        rtos_printf( "Periodic Thread created\n" );
        PeriodicThread* comp = ( PeriodicThread* ) t;

        while ( 1 )
        {
            while ( !comp->isRunning() && !comp->timeToQuit )
                comp->periodWait(); //POSIX
            if (comp->timeToQuit)
                return 0;

            if ( comp->runner != 0 )
                comp->runner->step();
            else
                comp->step(); // one cycle

            comp->periodWaitRemaining();
        }
        
        return 0;
    }

    PeriodicThread::PeriodicThread(int , const std::string& name, double period, RunnableInterface* r )
            : runner( r ), periodMark( 0 ), running( false ), timeToQuit(false)
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
              , finalizer( new Finalizer(this) )
#else
              , finalizer( 0 )
#endif
    {
        taskNameSet( name.c_str() );
        periodSet( period );
        pthread_attr_init( threadAttributeGet() );
        pthread_create( &thread, threadAttributeGet(), ComponentThread, this );
    }

    PeriodicThread::~PeriodicThread()
    {
        if ( isRunning() )
            stop();

        rtos_printf( "Terminating %s\n", taskName );

        terminate();

        // should wait until step is completed and exit then... ( FSM )
        //pthread_join(&thread); // join is not recommended
        pthread_attr_destroy( threadAttributeGet() );

        delete finalizer;
    }

    bool PeriodicThread::start()
    {
        if ( isRunning() )
            return false;

        if ( runner )
            runner->initialize();
        else
            initialize();

        running = true;

        return true;
    }

    bool PeriodicThread::stop()
    {
        if ( !isRunning() )
            return false;

        running = false;

        return true;
    }

    bool PeriodicThread::setToStop()
    {
        // finalize will be called in the thread of the
        // Completion processor
#ifdef OROINT_CORELIB_COMPLETION_INTERFACE
        ORO_CoreLib::CompletionProcessor::Instance()->queue( finalizer );
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
    {}

    bool PeriodicThread::initialize()
    { return true; }

    void PeriodicThread::finalize()
    {}

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
        pthread_join(thread, 0);
        //pthread_cancel( thread );
    }

    void PeriodicThread::periodWait()
    {
        rtos_nanosleep( &period, NULL ); // stay posix compatible
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

        if ( timeRemaining > 0 )
            rtos_nanosleep( hrt2ts( timeRemaining ), NULL );
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

