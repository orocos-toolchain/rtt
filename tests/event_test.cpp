/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  event_test.cpp 

                        event_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#include <corelib/Event.hpp>
#include <corelib/PriorityThread.hpp>
#include <corelib/RunnableInterface.hpp>
#include <corelib/TaskSimulation.hpp>

#include "event_test.hpp"
#include <boost/bind.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );

using namespace ORO_CoreLib;
using namespace boost;

void 
EventTest::setUp()
{
    t_event = new Event<void(void)>();
    t_event_string = new Event<void(std::string)>();
    event_proc = new EventProcessor();
    reset();
}


void 
EventTest::tearDown()
{
    delete t_event;
    delete t_event_string;
    delete event_proc;
    SimulationThread::Release();
}

void EventTest::listener(void)
{
    t_listener_value = true;
}

void EventTest::listenerString(const std::string& what)
{
    t_listener_what = what;
}

void EventTest::completer(void)
{
    t_completer_value = true;
}

void EventTest::reset()
{
    t_listener_value = false;
    t_completer_value = false;
    t_listener_what = "";
}

void 
EventTest::testEmpty()
{
    t_event->fire();
}

struct Runner : public RunnableInterface
{
    bool result;
    int data;
    Event<void(int)>& e;
    Handle h;
    Runner( Event<void(int)>& e_) : e(e_) {}
    bool initialize() {
        result = false;
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h = e.connect( bind(&Runner::handle,this, _1), bind(&Runner::complete,this,_1), this->getTask() );
        return true;
    }
    void step() {
        e.fire( 123456 );
    }
    void finalize() {
        h.disconnect();
    }

    void handle(int i) {
        data = i;
    }
    void complete(int i) {
        result = (i == data);
    }
};
        

void EventTest::testTask()
{
    Event<void(int)> event;
    Runner runobj(event);
    TaskSimulation task(0.01, &runobj);
    SimulationThread::Instance()->start();
    task.start();
    //CPPUNIT_ASSERT( runobj.data == 123456 );
    sleep(1);
    task.stop();
    SimulationThread::Instance()->stop();

    CPPUNIT_ASSERT( runobj.result );
}

void EventTest::testSyncListener()
{
    // No completer:
    reset();
    Handle h = t_event->connect( boost::bind(&EventTest::listener, this) );
    CPPUNIT_ASSERT( h.connected() );
    t_event->fire();
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );
}

void EventTest::testSyncListenerThreadCompleter()
{
    // in thread completer:
    reset();
    // Manually call step
    event_proc->step();
    Handle h = t_event->connect( boost::bind(&EventTest::listener,this),
                                 boost::bind(&EventTest::completer,this),
                                 event_proc );
    CPPUNIT_ASSERT( h.connected() );

    // Manually call step
    event_proc->step();

    t_event->fire();

    // Manually call step
    event_proc->step();

    // This will block until all completers are processed.
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    // Manually call step
    event_proc->step();

    // now, both must be called.
    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( t_completer_value );
}

void EventTest::testSyncListenerString()
{
    // No completer:
    reset();
    Handle h = t_event_string->connect( boost::bind(&EventTest::listenerString,this,_1) );
    CPPUNIT_ASSERT( h.connected() );
    t_event_string->fire( std::string("What") );
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    CPPUNIT_ASSERT_EQUAL( t_listener_what, std::string("What") );
    CPPUNIT_ASSERT( !t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );
}

void EventTest::testCompletionProcessor()
{
    // in thread completer:
    reset();

    Handle h = t_event->connect( boost::bind(&EventTest::listener,this),
                                 boost::bind(&EventTest::completer,this) // default == CP
                                 );
    CPPUNIT_ASSERT( h.connected() );

    t_event->fire();

    // This will block until all completers are processed.
    sleep(1);
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );
    // both must be called.
    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( t_completer_value );
}

    

