/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  NonPeriodicActivity.hpp 

                        NonPeriodicActivity.hpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef TASK_NON_PERIODIC_HPP
#define TASK_NON_PERIODIC_HPP

#include "ActivityInterface.hpp"
#include "RunnableInterface.hpp"
#include "os/SingleThread.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{

    /**
     * @brief A ActivityInterface implementation which is run in a SingleThread.
     *
     * By default, (if no RunnableInterface is given and no overloads
     * are present ) loop() returns immediately.
     * @ingroup CoreLibActivities
     */
    class NonPeriodicActivity
        :public ActivityInterface,
         private OS::SingleThread
    {
    public:
        using OS::SingleThread::run;

        /**
         * Create an NonPeriodicActivity with a given priority and
         * RunnableInterface instance. The default scheduler for NonPeriodicActivity
         * objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        NonPeriodicActivity(int priority, RunnableInterface* _r = 0 );

        /**
         * Create an NonPeriodicActivity with a given scheduler type, priority and
         * RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        NonPeriodicActivity(int scheduler, int priority, RunnableInterface* _r = 0 );

        /**
         * Create an NonPeriodicActivity with a given priority, name and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param name The name of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        NonPeriodicActivity(int priority, const std::string& name, RunnableInterface* _r = 0 );

        virtual ~NonPeriodicActivity();

        /**
         * Run another (or self in case of null)
         * task.
         */
        bool run( RunnableInterface* r );

        /**
         * Run another (or self in case of null)
         * task. This overload is needed to redirect
         * the call of run( 0 ) to the correct function.
         */
        bool run( int ) { RunnableInterface* d(0); return NonPeriodicActivity::run( d ); }

        virtual Seconds getPeriod() const ;

        virtual ThreadInterface* thread();

        virtual bool initialize();

        virtual void loop();

        virtual bool breakLoop();

        virtual void finalize();

        virtual bool isPeriodic() const;

        virtual bool start();

        virtual bool execute();

        virtual bool trigger();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool isActive() const;

    private:
        RunnableInterface*      runner;
};

}


#endif
