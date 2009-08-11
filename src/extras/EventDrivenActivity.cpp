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
#include "../internal/CompletionProcessor.hpp"
#include "TimerThread.hpp"
#include <boost/bind.hpp>


namespace RTT
{
    EventDrivenActivity::EventDrivenActivity(int priority, RunnableInterface* _r, const std::string& name )
        : Activity(priority, 0.0, _r), m_pending_events(0) { }

    EventDrivenActivity::EventDrivenActivity(int scheduler, int priority, RunnableInterface* _r, const std::string& name )
        : Activity(scheduler, priority, 0.0, _r), m_pending_events(0) { }

    EventDrivenActivity::~EventDrivenActivity()
    {
        this->stop();
        delete m_pending_events;
    }

    void EventDrivenActivity::event_trigger(Event<void()>* event)
    {
        if (!isRunning())
            return;

        m_pending_events->Push( event );
        trigger();
    }

    bool EventDrivenActivity::start()
    {
        delete m_pending_events;
        m_pending_events = new Triggers(4 * m_events.size());
        m_wakeup.reserve(m_events.size());

        for (Events::iterator it = m_events.begin(); it != m_events.end(); ++it)
        {
            Event<void(void)>& ev = **it;
            Handle h = ev.connect( boost::bind(&EventDrivenActivity::event_trigger, this, *it) );
            if (!h)
            {
                for_each(m_handles.begin(), m_handles.end(), boost::bind(&Handle::disconnect, _1));
                m_handles.clear();
                return false;
            }

            m_handles.push_back(h);
        }

        if (! Activity::start())
        {
            for_each(m_handles.begin(), m_handles.end(), boost::bind(&Handle::disconnect, _1));
            m_handles.clear();
            return false;
        }

        return true;
    }

    bool EventDrivenActivity::breakLoop()
    {
        m_pending_events->Push(NULL);
        return true;
    }

    void EventDrivenActivity::loop()
    {
        while (true)
        {
            m_wakeup.clear();

            do
            {
                Event<void()>* pending;
                m_pending_events->Pop(pending);
                if (! pending)
                    return;

                if (std::find(m_wakeup.begin(), m_wakeup.end(), pending) == m_wakeup.end())
                m_wakeup.push_back(pending);
            } while(!m_pending_events->empty());

            if ( runner )
                runner->step();
        }
    }

    std::vector<Event<void()>*> const& EventDrivenActivity::getWakeupEvents() const
    {
        return m_wakeup;
    }

    bool EventDrivenActivity::stop()
    {
        for_each(m_handles.begin(), m_handles.end(), boost::bind(&Handle::disconnect, _1));
        m_handles.clear();
        return Activity::stop();
    }

    bool EventDrivenActivity::addEvent( Event<void(void)>* _event)
    {
        if ( isActive() )
            return false;
            
        m_events.push_back(_event);
        return true;
    }
}
