/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:12 CEST 2002  ComponentThreaded.cpp 

                       ComponentThreaded.cpp -  description
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
#include "corelib/ComponentThreaded.hpp"
#include "rtstl/rtstreams.hpp"
#include "corelib/CompletionProcessor.hpp"
#include "corelib/Time.hpp"

namespace ORO_OS
{
    /**
     * This wraps the better written RTAI::Task class by Erwin Rol
     *
     * TODO : semaphores keep acting strangely, so i threw them out
     *        the waitInModule function allways returned when i 
     *        expected it to block, so i believe the bug is inthere
     */

    int ComponentThreaded::run()
    {
        //cout << "GO !" << endl;

        while ( 1 )
        {
            if ( prepareForExit )
            {
                return 0;
            }

            if ( running )
            {
                if ( runComp != 0 )
                    runComp->step(); // one cycle
                else
                    step();
            }

            else
            {
                stopped = true;
            }

            wait_period();
        }

        return 0;
    }

    ComponentThreaded::ComponentThreaded(int priority, const std::string& name, RunnableInterface* r )
            : Task( 8192, priority, 1, 0, 0 ),  // STACK, PRIORITY, FPU, SIGNAL,CPU_NR
            periodMark( 0 ), running( false ), stopped( true ), finalizer( this ), prepareForExit( false ), runComp( r )
    {
        taskNameSet( name.c_str() ) ;
        periodSet( 0, msecs_to_nsecs(10) ); // 10 ms
    }

    ComponentThreaded::~ComponentThreaded()
    {
        terminate();
    }

    bool ComponentThreaded::start()
    {
        if ( isRunning() )
            return false;

        if ( runComp )
            runComp->initialize();
        else
            this->initialize();

        running = true;
        stopped = false; // will be set to true by thread

        // this schedules the task for periodic execution
        make_periodic_relative( Time( ) , Time( period.tv_sec * 1000000000 + period.tv_nsec ) );

        return true;
    }

    bool ComponentThreaded::stop()
    {
        if ( !isRunning() )
            return false;

        running = false;
        int cnt = 0;

        //cout << "Stopping !" << endl;

        while ( stopped == false && cnt < 1000 )
        {
            rt_sleep( 1000000 ); //leave this number as is.
            cnt++;
        } // waiting... (need join() or condition variable !)

        // we suspend to be sure it is no longer scheduled
        cout <<"Suspending after "<<cnt<<" tries !"<<endl;
        this->suspend();

        if ( runComp )
            runComp->finalize();
        else
            this->finalize();

        return true;
    }

    void ComponentThreaded::setToStop()
    {
        // finalize will be called in the thread of the
        // Completion processor
        CompletionProcessor::Instance() ->queue( &finalizer );
    }

    bool ComponentThreaded::isRunning() const
    {
        return running;
    }

    int ComponentThreaded::periodSet( secs s, nsecs ns )
    {
        if ( isRunning() )
            return -1;

        period.tv_sec = s;

        period.tv_nsec = ns;

        return 0;
    }

    int ComponentThreaded::periodSet( TIME_SPEC p )
    {
        if ( isRunning() )
            return -1;

        period = p;

        return 0;
    }

    void ComponentThreaded::periodGet( secs& s, nsecs& ns ) const
    {
        s = period.tv_sec;
        ns = period.tv_nsec;
    }

    ComponentThreaded::Seconds ComponentThreaded::periodGet() const
    {
        return double ( period.tv_sec ) + double ( period.tv_nsec ) / ( 1000.0 * 1000.0 * 1000.0 );
    }

    void ComponentThreaded::periodWait()
    {
        Time time( period.tv_sec * secs_to_nsecs(1) + period.tv_nsec );
        sleep( Count( time.to_count() ) );
    }

    void ComponentThreaded::periodWaitRemaining()
    {
        wait_period();
    }

    void ComponentThreaded::terminate()
    {
        if ( prepareForExit )
            return ;

        if ( isRunning() )
        {
            running = false;

            int cnt = 0;

            while ( stopped == false && cnt < 1000 )
            {
                rt_busy_sleep( 100000 );
                cnt++;
            } // waiting... (need join() or condition variable !)

            prepareForExit = true;

            if ( runComp )
                runComp->finalize();
            else
                this->finalize();
        } else
            {
                prepareForExit = true;
                this->resume(); // when not running, is suspended...
            }
    }

    void ComponentThreaded::taskNameSet( const char* nm )
    {
        if ( strlen( nm ) < (unsigned int) TASKNAME_SIZE )
            sprintf( taskName, "%s", nm );
    }

    const char* ComponentThreaded::taskNameGet() const
    {
        return taskName;
    }

    void ComponentThreaded::step()
    {}

    bool ComponentThreaded::initialize()
    { return true; }

    void ComponentThreaded::finalize()
    {}

}
