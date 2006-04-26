/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  EventDrivenActivity.cxx 

                        EventDrivenActivity.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "corelib/EventDrivenActivity.hpp"
#include "corelib/TimerThread.hpp"
#include <boost/bind.hpp>


namespace ORO_CoreLib
{

    EventDrivenActivity::EventDrivenActivity( Event<void(void)>* _event, EventProcessor* thread, RunnableInterface* _r  )
        : event(_event), runner(_r), running(false), mthread(thread)
    {}

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

     bool EventDrivenActivity::run(RunnableInterface* _r)
    {
        if ( running )
            return false;
        runner = _r;
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
