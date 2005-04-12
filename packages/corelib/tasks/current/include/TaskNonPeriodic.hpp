/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  TaskNonPeriodic.hpp 

                        TaskNonPeriodic.hpp -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef TASK_NON_PERIODIC_HPP
#define TASK_NON_PERIODIC_HPP

#include "TaskInterface.hpp"
#include "RunnableInterface.hpp"
#include "EventProcessor.hpp"
#include "os/SingleThread.hpp"

#pragma interface

namespace ORO_CoreLib
{

    /**
     * @brief A TaskInterface implementation which is run in a SingleThread.
     *
     * By default, (if no RunnableInterface is given and no overloads
     * are present ) \a loop() will block for Events to be processed and
     * can be stopped by calling stop(). To check for events from your overloads or the RunnableInterface,
     * call this->processor()->step(), for non blocking checks and this->processor()->loop() for blocking
     * on Events.
     */
    class TaskNonPeriodic
        :public TaskInterface,
         private ORO_OS::SingleThread
    {
    public:
        /**
         * Create an TaskNonPeriodic with a given priority and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        TaskNonPeriodic(int priority, RunnableInterface* _r = 0 )
            : ORO_OS::SingleThread(priority, "TaskNonPeriodic" ),proc( new BlockingEventProcessor() ), runner(_r)
        {
            if ( runner )
                runner->setTask(this);
        }

        /**
         * Create an TaskNonPeriodic with a given priority, name and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param name The name of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        TaskNonPeriodic(int priority, const std::string& name, RunnableInterface* _r = 0 )
            : ORO_OS::SingleThread(priority, name ),proc( new BlockingEventProcessor() ),
              runner(_r)
        {
            if ( runner )
                runner->setTask(this);
        }

        virtual ~TaskNonPeriodic() 
        {
            this->stop();
            if ( runner )
                runner->setTask( 0 );
            delete proc;
        }
        
        /**
         * Run another (or self in case of null)
         * task.
         */
        bool run( RunnableInterface* r )
        {
            if ( isRunning() )
                return false;
            if (runner)
                runner->setTask(0);
            runner = r;
            if (runner)
                runner->setTask(this);
            return true;
        }

        virtual Seconds getPeriod() const { return 0; }

        virtual EventProcessor* processor() const { return proc; }

        virtual bool initialize() {
            bool result = proc->initialize();
            if ( runner && result )
                result = result && runner->initialize();
            return  result;
        }

        virtual void loop() { 
            if ( runner )
                runner->loop();
            else
                proc->loop(); // block in EventProcessor.
        }

        virtual bool breakLoop() {
            if ( runner )
                return runner->breakLoop();
            return proc->breakLoop(); // return from EventProcessor.
        }


        virtual void finalize() {
            if ( runner )
                runner->finalize();
            proc->finalize();
        }

        virtual bool start() {
            return SingleThread::start();
        }

        /**
         * Stop the EventProcessor's loop.
         */
        virtual bool stop() {
            return SingleThread::stop();
        }

        virtual bool isRunning() const {
            return SingleThread::isRunning();
        }

    private:
        BlockingEventProcessor* proc;
        RunnableInterface*      runner;
};

}


#endif
