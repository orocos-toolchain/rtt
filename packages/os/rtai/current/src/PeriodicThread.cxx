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
/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:12 CEST 2002  PeriodicThread.cpp 

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
#include "corelib/PeriodicThread.hpp"
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

    int PeriodicThread::run()
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

    PeriodicThread::PeriodicThread(int priority, const std::string& name, RunnableInterface* r )
            : Task( 8192, priority, 1, 0, 0 ),  // STACK, PRIORITY, FPU, SIGNAL,CPU_NR
            periodMark( 0 ), running( false ), stopped( true ), finalizer( this ), prepareForExit( false ), runComp( r )
    {
        taskNameSet( name.c_str() ) ;
        periodSet( 0, msecs_to_nsecs(10) ); // 10 ms
    }

    PeriodicThread::~PeriodicThread()
    {
        terminate();
    }

    bool PeriodicThread::start()
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

    bool PeriodicThread::stop()
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

    void PeriodicThread::setToStop()
    {
        // finalize will be called in the thread of the
        // Completion processor
        CompletionProcessor::Instance() ->queue( &finalizer );
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

    int PeriodicThread::periodSet( TIME_SPEC p )
    {
        if ( isRunning() )
            return -1;

        period = p;

        return 0;
    }

    void PeriodicThread::periodGet( secs& s, nsecs& ns ) const
    {
        s = period.tv_sec;
        ns = period.tv_nsec;
    }

    PeriodicThread::Seconds PeriodicThread::periodGet() const
    {
        return double ( period.tv_sec ) + double ( period.tv_nsec ) / ( 1000.0 * 1000.0 * 1000.0 );
    }

    void PeriodicThread::periodWait()
    {
        Time time( period.tv_sec * secs_to_nsecs(1) + period.tv_nsec );
        sleep( Count( time.to_count() ) );
    }

    void PeriodicThread::periodWaitRemaining()
    {
        wait_period();
    }

    void PeriodicThread::terminate()
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

    void PeriodicThread::taskNameSet( const char* nm )
    {
        if ( strlen( nm ) < (unsigned int) TASKNAME_SIZE )
            sprintf( taskName, "%s", nm );
    }

    const char* PeriodicThread::taskNameGet() const
    {
        return taskName;
    }

    void PeriodicThread::step()
    {}

    bool PeriodicThread::initialize()
    { return true; }

    void PeriodicThread::finalize()
    {}

}
