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
            : sem(s)
        {}

        EventCatcher::~EventCatcher() {}

        boost::signals::connection EventCatcher::getConnection() const
        {
            return h;
        }

    }

    EventProcessor::EventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s) : sem(s)
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
        // TODO All slots are only deleted on destruction. Disconnected
        // slots remain in the list. We could fix this by signal connection tracking
        // and/or defining our own asyn connection object.
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it)
            delete *it;
    }

    bool EventProcessor::initialize() { return true;}

    void EventProcessor::step() {
        if ( catchers.empty() )
            return;
        ORO_OS::MutexLock lock(m);
        for( List::iterator it = catchers.begin(); it != catchers.end(); ++it) {
            (*it)->complete();
        }
    }

    void EventProcessor::finalize() {
    }

    BlockingEventProcessor::BlockingEventProcessor(boost::shared_ptr<ORO_OS::Semaphore> s )
        : EventProcessor( s ), doloop(false)
    {
    }

    BlockingEventProcessor::~BlockingEventProcessor() {
        this->breakLoop();
        // race condition here, this->getTask()->isRunning() better be false
        // before EventProcessor is destructed, otherwise, solve with extra semaphore
    }
    void BlockingEventProcessor::loop() {
        //build critical section around loop()
        ORO_OS::MutexLock lock( breaker );
        doloop = true;
        while ( doloop )
            {
                this->step();
                // wait/block for the next Event
                sem->wait();
            }
    }

    bool BlockingEventProcessor::breakLoop() {
        doloop = false;
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
