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

#include "RunnableTaskInterface.hpp"

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
        :public RunnableTaskInterface
    {
    public:
        /**
         * Create an TaskEventDriven with a given event and optional 
         * RunnableInterface instance.
         * @param _event The Event which will trigger execution of this task,
         *        once this task is started.
         * @param _r The optional runner, if none, this->step() is called.
         */
        TaskEvenDriven( EventRegistrationInterface* _event, RunnableInterface _r = 0 )
            : onListener(true), onCompleter(false), event(_event), runner(_r), running(false),
              listener( new StandardListener(TaskEventDriven::handler, this) ),
              completer( new StandardCompleter(TaskEventDriven::completer, this) )
        {}

        virtual Seconds periodGet() { return 0; }

        bool start()
        {
            if ( !running && event )
                if ( runner ? runner->initialise() : this->initialise() )
                    {
                        event.addHandler( listener, completer );
                        return true;
                    }
            return false;
        }

        bool stop()
        {
            if ( running && event )
                {
                    event.removeHandler( listener, completer );
                    if (runner)
                        runner->initialise() ;
                    else 
                        this->initialise();
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
        void setEvent( EventRegistrationInterface* _event)
        {
            if ( running )
                return false;
            
            event = _event;
            return true;
        }

        /**
         * Configure to run when the Listeners of the
         * Event are called.
         */
        void runOnListener(bool doit)
        {
            onListener = doit;
        }

        /**
         * Configure to run when the Completers of the
         * Event are called.
         */
        void runOnCompleter(bool doit)
        {
            onCompleter = doit;
        }

    protected:

        void handler( CallBackInterface* cb )
        {
            callback = cb;
            if ( onListener )
                if (runner)
                    runner->step() ;
                else 
                    this->step();
            if ( onCompleter )
                cb->complete();
        }

        void completer()
        {
            if ( onCompleter)
                if (runner)
                    runner->step() ;
                else 
                    this->step();
        }

    private:
        bool onListener;
        bool onCompleter;
        EventRegistrationInterface* event;
        RunnableInterface*          runner;
        bool running;
        EventListenerInterface*     listener;
        EventCompleterInterface*    completer;
        CallBackInterface*          callback;


};



}


#endif
