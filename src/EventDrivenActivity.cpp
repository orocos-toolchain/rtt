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
#include "CompletionProcessor.hpp"
#include "TimerThread.hpp"
#include <boost/bind.hpp>


namespace RTT
{
    EventDrivenActivity::EventDrivenActivity(int priority, RunnableInterface* _r )
        : NonPeriodicActivity(priority, _r) { }

    EventDrivenActivity::EventDrivenActivity(int scheduler, int priority, RunnableInterface* _r )
        : NonPeriodicActivity(scheduler, priority, _r) { }

    EventDrivenActivity::EventDrivenActivity(int priority, const std::string& name, RunnableInterface* _r )
        : NonPeriodicActivity(priority, name, _r) { }

    EventDrivenActivity::~EventDrivenActivity()
    { this->stop(); }


    bool EventDrivenActivity::start()
    {
        if (! NonPeriodicActivity::start())
            return false;

        for (Events::iterator ev = events.begin(); ev != events.end(); ++ev)
        {
            Handle h = (*ev)->connect( boost::bind(&EventDrivenActivity::trigger, this) );
            if (!h)
            {
                for_each(handles.begin(), handles.end(), boost::bind(&Handle::disconnect, _1));
                handles.clear();
                return false;
            }

            handles.push_back(h);
        }
        return true;
    }

    void EventDrivenActivity::loop()
    {
        if ( runner )
            runner->step();
    }

    bool EventDrivenActivity::stop()
    {
        for_each(handles.begin(), handles.end(), boost::bind(&Handle::disconnect, _1));
        handles.clear();
        return NonPeriodicActivity::stop();
    }

    bool EventDrivenActivity::addEvent( Event<void(void)>* _event)
    {
        if ( isActive() )
            return false;
            
        events.push_back(_event);
        return true;
    }
}
