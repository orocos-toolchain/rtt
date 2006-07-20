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
 
 

#include <rtt/Event.hpp>
#include <rtt/Logger.hpp>
#include <rtt/RunnableInterface.hpp>
#include <rtt/SimulationActivity.hpp>
#include <rtt/SimulationThread.hpp>
#include <rtt/NonPeriodicActivity.hpp>

#include "event_test.hpp"
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );

using namespace RTT;
using namespace boost;

void 
EventTest::setUp()
{
    t_event = new Event<void(void)>();
    t_event_string = new Event<void(std::string)>();
    t_event_float = new Event<int(float, float)>();
    event_proc = new EventProcessor();
    reset();
}


void 
EventTest::tearDown()
{
    delete t_event;
    delete t_event_string;
    delete t_event_float;
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

int EventTest::float_listener(float a, float b)
{
    Logger::log() << Logger::Debug << "float_listener "<< a<<", "<<b<<Logger::endl;
    float_sum += a + b;
    return 1;
}

int EventTest::float_completer(float a, float b)
{
    Logger::log() << Logger::Debug << "float_completer "<< a<<", "<<b<<Logger::endl;
    float_sub -= (a + b);
    return 1; // ignored...
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
    Runner( Event<void(int)>& e_ ) : e(e_) {}
    bool initialize() {
        result = false;
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h = e.connect( bind(&Runner::handle,this, _1), bind(&Runner::complete,this,_1), this->getActivity() );
        return true;
    }
    void step() {
        e.fire( 123456 );
    }

    // blocking implementation
    void loop() {
        e.fire( 123456 );
        this->getActivity()->getEventProcessor()->loop(); // wait for our own event.
    }

    bool breakLoop() {
        return this->getActivity()->getEventProcessor()->breakLoop();
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
        
struct SelfRemover : public RunnableInterface
{
    Event<void(void)>& e;
    Handle h;
    SelfRemover( Event<void(void)>& e_ ) : e(e_) {}
    bool initialize() {
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h = e.setup( bind(&SelfRemover::handle,this), bind(&SelfRemover::complete,this), this->getActivity() );
        return true;
    }
    void step() {
        h.connect();
        e.emit();
        // repeat for complete :
        h.connect();
    }

    void finalize() {
    }

    void handle(void) {
        // this is dirty, user !
        // first disconnect self, then emit() within emit() !
        h.disconnect();
        e.emit();
        h.connect();
        // emit within emit should not come through (possible recursive stack blowup).
        e.emit();
        h.disconnect();
    }
    void complete(void) {
        h.disconnect();
    }
};
        
struct CrossRemover : public RunnableInterface
{
    Event<void(void)>& e;
    Handle h;
    CrossRemover( Event<void(void)>& e_ ) : e(e_), count(0) {}
    int count;
    bool initialize() {
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        e.connect( bind(&CrossRemover::handle,this), bind(&CrossRemover::complete,this), this->getActivity() );
        h = e.connect( bind(&CrossRemover::handle,this), bind(&CrossRemover::complete,this), this->getActivity() );
        e.connect( bind(&CrossRemover::handle,this), bind(&CrossRemover::complete,this), this->getActivity() );
        return true;
    }
    void step() {
        // for syn :
        count = 0;
        e.emit();
        h.disconnect(); // disconnect !
        // for asyn :
        count = 0;
    }

    void finalize() {
    }

    void handle(void) {
        if ( count == 0 )
            h.disconnect(); // remove next handler
        if ( count == 1 )
            h.connect();  // insert again
    }
    void complete(void) {
        // these connect/disconnect have no effect on times the complete
        // function is called, since it is already queued in the CP.
        if ( count == 0 )
            h.connect();  // insert a handler
        if ( count == 1 ) // remove it again
            h.disconnect();
        if ( count == 2 ) // add it again
            h.connect();
    }
};
        

void EventTest::testTask()
{
    Event<void(int)> event;
    Runner runobj(event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT(task.start());
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( runobj.result );
}

void EventTest::testSelfRemoval()
{
    SelfRemover runobj(*t_event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT( task.start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( task.stop() );
}

void EventTest::testCrossRemoval()
{
    CrossRemover runobj(*t_event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT( task.start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( task.stop() );
}

void EventTest::testBlockingTask()
{
    Event<void(int)> event;
    Runner runobj(event);
    NonPeriodicActivity task(15, &runobj);
    CPPUNIT_ASSERT(task.start());
    sleep(1);
    CPPUNIT_ASSERT(task.stop());

    CPPUNIT_ASSERT( runobj.result );
}

void EventTest::testEventArgs()
{
    float_sum = 0;
    float_sub = 0;
    // use CompletionProcessor for completer
    CPPUNIT_ASSERT(CompletionProcessor::Instance()->stop());
    Handle h = t_event_float->connect(boost::bind(&EventTest::float_listener, this,_1,_2),
                                      boost::bind(&EventTest::float_completer, this, _1, _2));

    t_event_float->fire(1.0, 4.0);
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );

    float a = 10.0, b = 5.0;
    t_event_float->fire(a, b);
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );
    CPPUNIT_ASSERT_EQUAL( float(0.0),  float_sub );

    CPPUNIT_ASSERT(CompletionProcessor::Instance()->start());
    
    h.disconnect();
    float_sum = 0;
    float_sub = 0;

    // use event processor
    event_proc->initialize();

    h = t_event_float->connect(boost::bind(&EventTest::float_listener, this,_1,_2),
                                      boost::bind(&EventTest::float_completer, this, _1, _2),
                                      event_proc, EventProcessor::OnlyLast);

    // simulate overrun :
    t_event_float->fire(1.0, 4.0);
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );

    t_event_float->fire(a, b);
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );

    event_proc->step();
    event_proc->finalize();
    // asyn handlers should reach only last total.
    CPPUNIT_ASSERT_EQUAL( float(-15.0), float_sub );
    h.disconnect();
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
    event_proc->initialize();
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
    event_proc->finalize();

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

void EventTest::testRTEvent()
{
    reset();
    event_proc->initialize();
    Handle hl( t_event->setup( boost::bind(&EventTest::listener,this) ) );
    Handle hc( t_event->setup( boost::bind(&EventTest::completer,this), event_proc ) );
    Handle hlc( t_event->setup( boost::bind(&EventTest::listener,this),
                                boost::bind(&EventTest::completer,this), event_proc ) );
    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( !hc.connected() );
    CPPUNIT_ASSERT( !hlc.connected() );

    hl.connect();
    hc.connect();
    CPPUNIT_ASSERT( hl.connected() );
    CPPUNIT_ASSERT( hc.connected() );

    t_event->fire();
    

    hl.disconnect();
    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );

    event_proc->step();
    CPPUNIT_ASSERT( t_completer_value );
    hc.disconnect();
    CPPUNIT_ASSERT( !hc.connected() );

    reset();
    t_event->fire();
    event_proc->step();

    CPPUNIT_ASSERT( t_completer_value == false );
    CPPUNIT_ASSERT( t_listener_value == false );

    hl.connect();
    hc.connect();
    t_event->fire();

    hl.disconnect();
    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( t_listener_value == true );
    CPPUNIT_ASSERT( t_completer_value == false );

    event_proc->step();
    hc.disconnect();
    CPPUNIT_ASSERT( !hc.connected() );
    CPPUNIT_ASSERT( t_completer_value == true );

    reset();
    hlc.connect();
    CPPUNIT_ASSERT( hlc.connected() );
    t_event->fire();
    CPPUNIT_ASSERT( t_listener_value == true );
    CPPUNIT_ASSERT( t_completer_value == false );

    event_proc->step();
    hlc.disconnect();
    CPPUNIT_ASSERT( !hlc.connected() );
    CPPUNIT_ASSERT( t_completer_value == true );

}
