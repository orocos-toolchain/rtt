/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  TaskEventDriven.hpp 

                        TaskEventDriven.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef TASK_EVENT_DRIVEN_HPP
#define TASK_EVENT_DRIVEN_HPP

#include "TaskInterface.hpp"
#include "RunnableInterface.hpp"
#include "Event.hpp"
#include <boost/bind.hpp>

namespace ORO_CoreLib
{

    /**
     * @brief An Event-driven task implementation.
     *
     * This class represents a Task which can be attached
     * to an event and execute its functionality each time
     * the event is fired. By default, only the listener
     * action triggers step(), but one can configure it to
     * do otherwise.
     */
    class TaskEventDriven
        :public TaskInterface
    {
        Handle h;
    public:
        /**
         * Create an TaskEventDriven with a given event and optional 
         * RunnableInterface instance.
         * @param _event The Event which will trigger execution of this task,
         *        once this task is started.
         * @param _r The optional runner, if none, this->step() is called.
         */
        TaskEventDriven( Event<void(void)>* _event, RunnableInterface* _r = 0 )
            : event(_event), runner(_r), running(false)
        {}

        virtual Seconds periodGet() { return 0; }

        bool initialize() { return  true;}
        void step() {}
        void finalize() {}

        bool start()
        {
            if ( !running && event )
                if ( runner ? runner->initialize() : this->initialize() )
                    {
                        h = event->connect( boost::bind(&TaskEventDriven::handler, this) );
                        return true;
                    }
            return false;
        }

        bool stop()
        {
            if ( running && event )
                {
                    h.disconnect();
                    if (runner)
                        runner->finalize() ;
                    else 
                        this->finalize();
                    return true;
                }
            else 
                return false;
        }

        bool isRunning() { return running; }

        /**
         * Run another (or none in case of null)
         * task.
         */
        bool run(RunnableInterface* _r)
        {
            if ( running )
                return false;
            runner = _r;
            return true;
        }

        /**
         * Set the Event which will trigger the execution
         * of this task, once started.
         */
        bool setEvent( Event<void(void)>* _event)
        {
            if ( running )
                return false;
            
            event = _event;
            return true;
        }

    protected:

        void handler()
        {
                if (runner)
                    runner->step() ;
                else 
                    this->step();
        }

    private:
        Event<void(void)>* event;
        RunnableInterface*          runner;
        bool running;
};

}


#endif
