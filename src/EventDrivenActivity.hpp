/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventDrivenActivity.hpp 

                        EventDrivenActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef TASK_EVENT_DRIVEN_HPP
#define TASK_EVENT_DRIVEN_HPP

#include "ActivityInterface.hpp"
#include "RunnableInterface.hpp"
#include "Event.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{

    /**
     * @brief An Event-driven ActivityInterface implementation.
     *
     * This class represents a Activity which can be attached
     * to an Event<void(void)> and execute its functionality each time
     * the event is fired. The EventDrivenActivity is run asynchronously 
     * in a given thread.
     */
    class EventDrivenActivity
        :public ActivityInterface
    {
        Handle h;
    public:
        /**
         * Create an EventDrivenActivity with a given event and optional 
         * RunnableInterface instance.
         * @param _event The Event which will trigger execution of this task,
         *        once this task is started.
         * @param _r The optional runner, if none, this->step() is called.
         * @param thread The EventProcessor which will execute the asynchronous completion.
         */
        EventDrivenActivity( Event<void(void)>* _event, EventProcessor* thread, RunnableInterface* _r = 0 );

        /**
         * Cleanup and notify the RunnableInterface that we are gone.
         */
        ~EventDrivenActivity();

        virtual Seconds getPeriod() const { return 0; }

        EventProcessor* processor() const { return mthread; }

        bool initialize() { return  true;}
        void step() {}
        void finalize() {}

        bool start();

        bool trigger();

        bool stop();

        bool isRunning() const {
            return running;
        }

        /**
         * Run another (or self in case of null)
         * task.
         */
        bool run(RunnableInterface* _r);

        /**
         * Set the Event which will trigger the execution
         * of this task, once started.
         */
        bool setEvent( Event<void(void)>* _event);

        bool isPeriodic() const;
    protected:

        void handler();
    private:
        Event<void(void)>* event;
        RunnableInterface*          runner;
        bool running;
        EventProcessor* mthread;
};

}


#endif
