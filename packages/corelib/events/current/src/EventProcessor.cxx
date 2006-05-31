/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  EventProcessor.cxx 

                        EventProcessor.cxx -  description
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
 
 

#include "corelib/EventProcessor.hpp"
#include <os/threads.hpp>

namespace ORO_CoreLib
{

    namespace detail
    {
        using ORO_OS::Semaphore;

        EventCatcher::EventCatcher(Semaphore* s)
            : mep(0), sem(s), enabled(false), refCount(0) // special trick !
        {}

        EventCatcher::~EventCatcher() {
            // notify EP that we were deleted :
            if (mep)
                mep->destroyed(this);
        }

        void intrusive_ptr_add_ref( EventCatcher* p ) { ++p->refCount; }
        void intrusive_ptr_release( EventCatcher* p ) { if ( --p->refCount == 0) delete p; }

    }

    using namespace detail;

    EventProcessor::EventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s)
        : catchers(4), sem(s), active(false)
    {
    }

    EventProcessor::EventProcessor()
        : catchers(4),
          sem( boost::shared_ptr<ORO_OS::Semaphore>() )
    {
    }

    static void clearEP(EventCatcher* ec) {
        ec->mep = 0;
    }

    EventProcessor::~EventProcessor() {
        // clear us out from catchers such that they do not try
        // to call destroyed() lateron.
        catchers.apply( boost::bind(&clearEP,_1) );
        catchers.clear();
    }

    void EventProcessor::destroyed( detail::EventCatcher* eci )
    {
        catchers.erase( eci );
        catchers.shrink();
    }

    static void enableAll( EventCatcher* eci ) {
        eci->enabled = true;
    }

    static void disableAll( EventCatcher* eci ) {
        eci->enabled = false;
    }

    bool EventProcessor::initialize() {
        active = true;
        catchers.apply( boost::bind(&enableAll, _1 ) );
        return true;
    }

    void EventProcessor::step() {
        if ( catchers.empty() )
            return;
        catchers.apply( boost::bind(&EventCatcher::complete, _1 ) );
    }

    void EventProcessor::finalize() {
        active = false;
        catchers.apply( boost::bind(&disableAll, _1 ) );
    }

    BlockingEventProcessor::BlockingEventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s )
        : EventProcessor( s )
    {
    }

    BlockingEventProcessor::~BlockingEventProcessor() {
        this->breakLoop();
        // race condition here, this->getActivity()->isRunning() better be false
        // before EventProcessor is destructed, otherwise, solve with extra semaphore
    }

    bool BlockingEventProcessor::initialize() {
        // we set active to true inhere to avoid a race condition
        // it was originally set in loop(), but it went wrong there.
        active = true;
        return true;
    }

    void BlockingEventProcessor::loop() {
        while ( active )
            {
                this->step();
                // wait/block for the next Event
                sem->wait();
            }
    }

    bool BlockingEventProcessor::breakLoop() {
        active = false;
        sem->signal();
        return true;
    }

    static void setSem(boost::shared_ptr<ORO_OS::Semaphore> s, EventCatcher* eci) {
        eci->sem = s.get();
    }

    void BlockingEventProcessor::setSemaphore( boost::shared_ptr<ORO_OS::Semaphore> s) {
        catchers.apply( boost::bind(&setSem, s, _1 ) );
    }
}
