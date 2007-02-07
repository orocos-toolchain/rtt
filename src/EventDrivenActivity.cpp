/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  EventDrivenActivity.cxx 

                        EventDrivenActivity.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "EventDrivenActivity.hpp"
#include "TimerThread.hpp"
#include <boost/bind.hpp>


namespace RTT
{

    EventDrivenActivity::EventDrivenActivity( Event<void(void)>* _event, EventProcessor* thread, RunnableInterface* _r  )
        : event(_event), runner(_r), running(false), mthread(thread)
    {
        if (runner)
            runner->setActivity(this);
    }

    EventDrivenActivity::~EventDrivenActivity()
    {
        stop();
        if (runner)
            runner->setActivity(0);
    }


    bool EventDrivenActivity::start()
    {
        if ( !running && event )
            if ( runner ? runner->initialize() : this->initialize() )
                {
                    // handler is executed asynchronously
                    h = event->connect( boost::bind(&EventDrivenActivity::handler, this), mthread );
                    return true;
                }
        return false;
    }

    bool EventDrivenActivity::stop()
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

    bool EventDrivenActivity::trigger()
    {
        return false;
    }

     bool EventDrivenActivity::run(RunnableInterface* _r)
    {
        if ( running )
            return false;

        if (runner)
            runner->setActivity(0);
        runner = _r;
        if (runner)
            runner->setActivity(this);
        return true;
    }

    bool EventDrivenActivity::setEvent( Event<void(void)>* _event)
    {
        if ( running )
            return false;
            
        event = _event;
        return true;
    }


    void EventDrivenActivity::handler()
    {
        if (runner)
            runner->step() ;
        else 
            this->step();
    }

    bool EventDrivenActivity::isPeriodic() const
    {
        return true;
    }
}
