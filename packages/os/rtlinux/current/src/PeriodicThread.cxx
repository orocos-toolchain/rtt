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
/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:13 CEST 2002  PeriodicThread.cpp 

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



#include "os/PeriodicThread.hpp" 
//#include <time.h>
#include "corelib/Time.hpp"

namespace ORO_OS
{

    void *ComponentThread( void *t )
    {
        rtos_printf( "Component thread created\n" );
        PeriodicThread* comp = ( PeriodicThread* ) t;
        // must become a check on condition objects
        //    pthread_attr_setfp_np(comp->threadAttributeGet(), 1);
        //pthread_setfp_np(pthread_self(),1);

        while ( 1 )
        {
            //        rtos_printf(".");

            while ( !comp->isRunning() )
                comp->periodWait(); //POSIX

            if ( comp->runner != 0 )
                comp->runner->step();
            else
                comp->step(); // one cycle

            comp->periodWaitRemaining();
        }
    }

    PeriodicThread::PeriodicThread(int priority, const string& name, RunnableInterface* r = 0 )
            : runner( r )
    {
        rtos_printf( "PeriodicThread Constructor\n" );
        running = 0;
        taskNameSet( name.c_str() );
        periodSet( 0, msecs_to_nsecs(10) ); // 10 ms
        periodMark = 0;
        pthread_attr_init( threadAttributeGet() );
        //    pthread_attr_setstacksize(threadAttributeGet(), 40*1024);
        // NON PORTABLE !!
        //pthread_attr_setfp_np(threadAttributeGet(), 1);
        rtos_enable_fpu( threadAttributeGet() );
        struct sched_param sp;
        sp.sched_priority = priority;
        
        pthread_attr_setschedparam(threadAttributeGet(), &sp );
        //    pthread_create(&thread, NULL, ComponentThread, (void*) this);
        rtos_printf( "creating...." );
        pthread_create( &thread, threadAttributeGet(), ComponentThread, this );
        //    rt_task_init(&thread, ComponentThread, (int)this, 8096, 1, 1, NULL);
        //    rt_task_resume(&thread);
    }

    PeriodicThread::~PeriodicThread()
    {
        if ( isRunning() )
            stop();

        rtos_printf( "Terminating %s\n", taskName );

        //    terminate();
        // should wait until step is completed and exit then... ( FSM )
        pthread_cancel( thread );

        //pthread_join(&thread); // join is not recommended
        pthread_attr_destroy( threadAttributeGet() );
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

        if ( runner )
            runner->finalize();
        else
            finalize();

        return true;
    }

    void PeriodicThread::setToStop()
    {
        // finalize will be called in the thread of the
        // Completion processor
        stop();
    }

    bool PeriodicThread::isRunning() const
    {
        return running;
    }

    int PeriodicThread::periodSet( secs s, nsecs ns )
    {
        if ( isRunning() )
            return -1;

        period.tv_sec = s;

        period.tv_nsec = ns;

        return 0;
    }

    void PeriodicThread::periodGet( secs& s, nsecs& ns ) const
    {
        s = period.tv_sec;
        ns = period.tv_nsec;
    }

    double PeriodicThread::periodGet() const
    {
        return ( double ) period.tv_sec + ( double ) period.tv_nsec / ( 1000.0 * 1000.0 * 1000.0 );
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
        NANO_TIME timeRemaining = secs_to_nsecs(period.tv_sec) + period.tv_nsec - ( rtos_get_time_ns() - periodMark );

        //rtos_printf("Waiting for %d nsec\n",timeRemaining);

        if ( timeRemaining > 0 )
            rtos_nanosleep( hrt2ts( timeRemaining ), NULL );
        else
            rtos_printf( "%s did not get deadline !", taskNameGet() );

        periodMark = rtos_get_time_ns() ;

    }

    void PeriodicThread::periodWait()
    {
        rtos_nanosleep( &period, NULL );
    }

    void PeriodicThread::terminate()
    {
        rtos_printf( "Called fake terminate() in PeriodicThread, task %s", taskNameGet() );
    }

    pthread_attr_t* PeriodicThread::threadAttributeGet()
    {
        return & threadAttribute;
    }


    void PeriodicThread::taskNameSet( const char* nm )
    {
        snprintf( taskName, 31, "%s", nm );
    }

    const char* PeriodicThread::taskNameGet() const
    {
        return taskName;
    }

    void PeriodicThread::step()
    {
        periodWait();
    }

    bool PeriodicThread::initialize()
    {   return true; }

    void PeriodicThread::finalize()
    {}

}

;
