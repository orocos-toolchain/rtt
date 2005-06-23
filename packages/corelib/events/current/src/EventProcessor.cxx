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

    EventProcessor::EventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s)
        : sem(s), active(false)
    {
        // TODO define MAX_ASYN_EVENTS
        catchers.reserve(128);
    }

    EventProcessor::EventProcessor() : sem( boost::shared_ptr<ORO_OS::Semaphore>() )
    {
        // TODO define MAX_ASYN_EVENTS
        catchers.reserve(128);
    }

    EventProcessor::~EventProcessor() {
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
            if (*it)
                (*it)->mep = 0; // clear us out
        catchers.clear();
    }

    void EventProcessor::destroyed( detail::EventCatcher* eci )
    {
        ORO_OS::MutexLock lock(m);
        
        List::iterator it = std::find(catchers.begin(), catchers.end(), eci );
        if ( it != catchers.end() ) {
            *it = 0;
        }
    }

    bool EventProcessor::initialize() {
        active = true;
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
            if (*it)
                (*it)->enabled = true;
        return true;
    }

    void EventProcessor::step() {
        if ( catchers.empty() )
            return;
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it) {
            if (*it)
                (*it)->complete();
        }
    }

    void EventProcessor::finalize() {
        active = false;
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
            if (*it)
                (*it)->enabled = false;
    }

    BlockingEventProcessor::BlockingEventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s )
        : EventProcessor( s )
    {
    }

    BlockingEventProcessor::~BlockingEventProcessor() {
        this->breakLoop();
        // race condition here, this->getTask()->isRunning() better be false
        // before EventProcessor is destructed, otherwise, solve with extra semaphore
    }

    bool BlockingEventProcessor::initialize() {
        // we set active to true inhere to avoid a race condition
        // it was originally set in loop(), but it went wrong there.
        active = true;
        return true;
    }

    void BlockingEventProcessor::loop() {
        //build critical section around loop()
        ORO_OS::MutexLock lock( breaker );
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
        {
            ORO_OS::MutexLock lock( breaker ); // force a block until loop returns.
        }
        return true;
    }

    void BlockingEventProcessor::setSemaphore( boost::shared_ptr<ORO_OS::Semaphore> s) {
        ORO_OS::MutexLock lock(m);
        sem = s;
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it) {
            (*it)->sem = s.get();
        }
            
    }
}
