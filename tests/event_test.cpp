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
 
 

#include <Event.hpp>
#include <Logger.hpp>
#include <RunnableInterface.hpp>
#include <SimulationActivity.hpp>
#include <SimulationThread.hpp>
#include <NonPeriodicActivity.hpp>
#include <CompletionProcessor.hpp>

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
    t_event = Event<void(void)>("t_event");
    t_event_string = Event<void(std::string)>("t_event_string");
    t_event_float = Event<int(float, float)>("t_event_float");
    event_proc = new EventProcessor();
    act.run( event_proc );
    reset();
    // to be sure
    CompletionProcessor::Instance()->start();
}


void 
EventTest::tearDown()
{
    act.stop();
    delete event_proc;
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
    t_event();
}

struct Runner : public RunnableInterface
{
    bool result;
    int data;
    Event<void(int)>& e;
    Handle h1;
    Handle h2;
    Runner( Event<void(int)>& e_ ) : e(e_) {
        act.run(&ep);
    }

    SlaveActivity act;
    BlockingEventProcessor ep;

    bool initialize() {
        act.start();
        result = false;
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h1 = e.connect( bind(&Runner::handle,this, _1) );
        h2 = e.connect( bind(&Runner::complete,this,_1), &ep );
        return true;
    }
    void step() {
        e( 123456 );
        ep.loop();
    }

    // blocking implementation
    void loop() {
        e( 123456 );
        ep.loop(); // wait for our own event.
    }

    bool breakLoop() {
        return ep.breakLoop();
    }

    void finalize() {
        act.stop();
        h1.disconnect();
        h2.disconnect();
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
    Handle h1, h2;
    SelfRemover( Event<void(void)>& e_ ) : e(e_) {
        act.run(&ep);
    }
    
    SlaveActivity act;
    EventProcessor ep;

    bool initialize() {
        act.start();
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h1 = e.setup( bind(&SelfRemover::handle,this));
        h2 = e.setup( bind(&SelfRemover::complete,this), &ep );
        return true;
    }
    void step() {
        h1.connect();
        h2.connect();
        e();
        // repeat for complete :
        h1.connect();
        h2.connect();

        ep.step();
    }

    void finalize() {
        act.stop();
    }

    void handle(void) {
        // this is dirty, user !
        // first disconnect self, then emit() within emit() !
        h1.disconnect();
        h2.disconnect();
        e();
        h1.connect();
        h2.connect();
        // emit within emit should not come through (possible recursive stack blowup).
        e();
        h1.disconnect();
        h2.disconnect();
    }
    void complete(void) {
        h1.disconnect();
        h2.disconnect();
    }
};
        
struct CrossRemover : public RunnableInterface
{
    Event<void(void)>& e;
    Handle h1,h2;
    CrossRemover( Event<void(void)>& e_ ) : e(e_), count(0) {
        act.run(&ep);
    }
    int count;

    SlaveActivity act;
    EventProcessor ep;

    bool initialize() {
        act.start();
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        e.connect( bind(&CrossRemover::handle,this));
        e.connect( bind(&CrossRemover::complete,this), &ep );
        h1 = e.connect( bind(&CrossRemover::handle,this));
        h2 = e.connect( bind(&CrossRemover::complete,this), &ep );
        e.connect( bind(&CrossRemover::handle,this));
        e.connect( bind(&CrossRemover::complete,this), &ep );
        return true;
    }
    void step() {
        // for syn :
        count = 0;
        e();
        h1.disconnect(); // disconnect !
        h2.disconnect(); // disconnect !
        // for asyn :
        count = 0;
        ep.step();
    }

    void finalize() {
        act.stop();
    }

    void handle(void) {
        if ( count == 0 ) {
            h1.disconnect(); // remove next handler
            h2.disconnect(); // remove next handler
        }
        if ( count == 1 ){
            h1.connect();  // insert again
            h2.connect();  // insert again
        }
    }
    void complete(void) {
        // these connect/disconnect have no effect on times the complete
        // function is called, since it is already queued in the CP.
        if ( count == 0 ) {
            h1.connect();  // insert a handler
            h2.connect();  // insert a handler
        }
        if ( count == 1 ) {// remove it again
            h1.disconnect();
            h2.disconnect();
        }
        if ( count == 2 ) { // add it again
            h1.connect();
            h2.connect();
        }
    }
};
        

void EventTest::testTask()
{
    Event<void(int)> event("Event");
    Runner runobj(event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT( SimulationThread::Instance()->stop() );
    CPPUNIT_ASSERT(task.start());
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( runobj.result );
}

void EventTest::testSelfRemoval()
{
    SelfRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT( SimulationThread::Instance()->stop() );
    CPPUNIT_ASSERT( task.start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( task.stop() );
}

void EventTest::testCrossRemoval()
{
    CrossRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    CPPUNIT_ASSERT( SimulationThread::Instance()->stop() );
    CPPUNIT_ASSERT( task.start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( task.stop() );
}

void EventTest::testBlockingTask()
{
    Event<void(int)> event("Event");
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
    Handle h1 = t_event_float.connect( boost::bind(&EventTest::float_listener, this,_1,_2) );

    Handle h2 = t_event_float.connect(boost::bind(&EventTest::float_completer, this, _1, _2),
                                      CompletionProcessor::Instance() );

    t_event_float(1.0, 4.0);
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );

    float a = 10.0, b = 5.0;
    t_event_float(a, b);
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );
    CPPUNIT_ASSERT_EQUAL( float(0.0),  float_sub );

    CPPUNIT_ASSERT(CompletionProcessor::Instance()->start());
    
    h1.disconnect();
    h2.disconnect();
    float_sum = 0;
    float_sub = 0;

    // use event processor
    CPPUNIT_ASSERT(act.start());

    h1 = t_event_float.connect(boost::bind(&EventTest::float_listener, this,_1,_2));
    h2 = t_event_float.connect(boost::bind(&EventTest::float_completer, this, _1, _2),
                               event_proc, EventProcessor::OnlyLast);

    // simulate overrun :
    t_event_float(1.0, 4.0);
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );

    t_event_float(a, b);
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );

    event_proc->step();
    act.stop();
    // asyn handlers should reach only last total.
    CPPUNIT_ASSERT_EQUAL( float(-15.0), float_sub );
    h1.disconnect();
    h2.disconnect();
}

void EventTest::testSyncListener()
{
    // No completer:
    reset();
    Handle h = t_event.connect( boost::bind(&EventTest::listener, this) );
    CPPUNIT_ASSERT( h.connected() );
    t_event();
    h.disconnect();
    CPPUNIT_ASSERT( !h.connected() );

    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );
}

void EventTest::testSyncListenerThreadCompleter()
{
    // in thread completer:
    reset();
    CPPUNIT_ASSERT(act.start());
    // Manually call step
    event_proc->step();
    Handle h1 = t_event.connect( boost::bind(&EventTest::listener,this));
    Handle h2 = t_event.connect( boost::bind(&EventTest::completer,this),
                                 event_proc );
    CPPUNIT_ASSERT( h1.connected() );
    CPPUNIT_ASSERT( h2.connected() );

    // Manually call step
    event_proc->step();

    t_event();

    // Manually call step
    event_proc->step();

    // This will block until all completers are processed.
    h1.disconnect();
    h2.disconnect();
    CPPUNIT_ASSERT( !h1.connected() );
    CPPUNIT_ASSERT( !h2.connected() );

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
    Handle h = t_event_string.connect( boost::bind(&EventTest::listenerString,this,_1) );
    CPPUNIT_ASSERT( h.connected() );
    t_event_string( std::string("What") );
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

    CPPUNIT_ASSERT( CompletionProcessor::Instance()->isActive() );

    Handle h2 = t_event.connect(boost::bind(&EventTest::completer,this),
                                CompletionProcessor::Instance()
                                 );
    CPPUNIT_ASSERT( h2.connected() );

    t_event();

    // This will block until all completers are processed.
    sleep(1);
    h2.disconnect();
    CPPUNIT_ASSERT( !h2.connected() );
    // CP must be called.
    CPPUNIT_ASSERT( t_completer_value );
}

void EventTest::testRTEvent()
{
    reset();

    CPPUNIT_ASSERT(act.start());

    Handle hl( t_event.setup( boost::bind(&EventTest::listener,this) ) );
    Handle hc( t_event.setup( boost::bind(&EventTest::completer,this), event_proc ) );

    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( !hc.connected() );

    hl.connect();
    hc.connect();
    CPPUNIT_ASSERT( hl.connected() );
    CPPUNIT_ASSERT( hc.connected() );

    t_event();
    

    hl.disconnect();
    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( t_listener_value );
    CPPUNIT_ASSERT( !t_completer_value );

    event_proc->step();
    CPPUNIT_ASSERT( t_completer_value );
    hc.disconnect();
    CPPUNIT_ASSERT( !hc.connected() );

    reset();
    t_event();
    event_proc->step();

    CPPUNIT_ASSERT( t_completer_value == false );
    CPPUNIT_ASSERT( t_listener_value == false );

    hl.connect();
    hc.connect();
    t_event();

    hl.disconnect();
    CPPUNIT_ASSERT( !hl.connected() );
    CPPUNIT_ASSERT( t_listener_value == true );
    CPPUNIT_ASSERT( t_completer_value == false );

    event_proc->step();
    hc.disconnect();
    CPPUNIT_ASSERT( !hc.connected() );
    CPPUNIT_ASSERT( t_completer_value == true );

}
