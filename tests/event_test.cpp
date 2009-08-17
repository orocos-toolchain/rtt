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
#include <base/RunnableInterface.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <Activity.hpp>
#include <internal/CompletionProcessor.hpp>
#include <os/Atomic.hpp>

#include "event_test.hpp"
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace RTT;
using namespace boost;


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
        // do not emit within handle!
        // first disconnect self, then reconnect
        h1.disconnect();
        h2.disconnect();
        h1.connect();
        h2.connect();
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

static AtomicInt testConcurrentEmitHandlerCount;

void testConcurrentEmitHandler(void)
{
    testConcurrentEmitHandlerCount.inc();
}

class EmitAndcount
    :public RunnableInterface
{
public:
    EmitAndcount(Event<void(void)> &ev)
        : mev(ev), count(0) {}
    Event<void(void)> &mev;
    int count;
    bool initialize() { return true;}
    void step() {}
    void finalize() {}
    bool breakLoop() { return true; }
    void loop()
    {
        mev();
        ++count;
        this->getActivity()->trigger();
    }

};

BOOST_FIXTURE_TEST_SUITE( EventTestSuite, EventTest )

BOOST_AUTO_TEST_CASE( testEmpty )
{
    t_event();
}

BOOST_AUTO_TEST_CASE( testTask )
{
    Event<void(int)> event("Event");
    Runner runobj(event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK( SimulationThread::Instance()->stop() );
    BOOST_CHECK(task.start());
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( runobj.result );
}

BOOST_AUTO_TEST_CASE( testSelfRemoval )
{
    SelfRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK( SimulationThread::Instance()->stop() );
    BOOST_CHECK( task.start() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( task.stop() );
}

BOOST_AUTO_TEST_CASE( testCrossRemoval )
{
    CrossRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK( SimulationThread::Instance()->stop() );
    BOOST_CHECK( task.start() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( task.stop() );
}

#ifdef OROCOS_TARGET_GNULINUX
BOOST_AUTO_TEST_CASE( testConcurrentEmit )
{
    testConcurrentEmitHandlerCount.set(0);
    Event<void(void)> event("Event");
    BOOST_CHECK( event.ready() );
    EmitAndcount arunobj(event);
    EmitAndcount brunobj(event);
    EmitAndcount crunobj(event);
    EmitAndcount drunobj(event);
    Activity atask(ORO_SCHED_OTHER, 0, &arunobj);
    Activity btask(ORO_SCHED_OTHER, 0, &brunobj);
    Activity ctask(ORO_SCHED_OTHER, 0, &crunobj);
    Activity dtask(ORO_SCHED_OTHER, 0, &drunobj);
    Handle h = event.connect( &testConcurrentEmitHandler );
    BOOST_CHECK( h.connected() );
    BOOST_CHECK( atask.start() );
    BOOST_CHECK( btask.start() );
    BOOST_CHECK( ctask.start() );
    BOOST_CHECK( dtask.start() );
    sleep(1);
    BOOST_CHECK( atask.stop() );
    BOOST_CHECK( btask.stop() );
    BOOST_CHECK( ctask.stop() );
    BOOST_CHECK( dtask.stop() );
    // Verify that all emits also caused the handler to be called.
    BOOST_CHECK_EQUAL( arunobj.count + brunobj.count + crunobj.count + drunobj.count, testConcurrentEmitHandlerCount.read() );
}
#endif

BOOST_AUTO_TEST_CASE( testBlockingTask )
{
    Event<void(int)> event("Event");
    Runner runobj(event);
    Activity task(15, &runobj);
    BOOST_CHECK(task.start());
    usleep(100000);
    BOOST_CHECK(task.stop());

    BOOST_CHECK( runobj.result );
}

BOOST_AUTO_TEST_CASE( testEventArgs )
{
    float_sum = 0;
    float_sub = 0;
    // use CompletionProcessor for completer
    BOOST_CHECK(CompletionProcessor::Instance()->stop());
    Handle h1 = t_event_float.connect( boost::bind(&EventTest::float_listener, this,_1,_2) );

    Handle h2 = t_event_float.connect(boost::bind(&EventTest::float_completer, this, _1, _2),
                                      CompletionProcessor::Instance() );

    t_event_float(1.0, 4.0);
    BOOST_CHECK_EQUAL( float(5.0), float_sum );

    float a = 10.0, b = 5.0;
    t_event_float(a, b);
    BOOST_CHECK_EQUAL( float(20.0), float_sum );
    BOOST_CHECK_EQUAL( float(0.0),  float_sub );

    BOOST_CHECK(CompletionProcessor::Instance()->start());

    h1.disconnect();
    h2.disconnect();
    float_sum = 0;
    float_sub = 0;

    // use event processor
    BOOST_CHECK(act.start());

    h1 = t_event_float.connect(boost::bind(&EventTest::float_listener, this,_1,_2));
    h2 = t_event_float.connect(boost::bind(&EventTest::float_completer, this, _1, _2),
                               event_proc, EventProcessor::OnlyLast);

    // simulate overrun :
    t_event_float(1.0, 4.0);
    BOOST_CHECK_EQUAL( float(5.0), float_sum );

    t_event_float(a, b);
    BOOST_CHECK_EQUAL( float(20.0), float_sum );

    event_proc->step();
    act.stop();
    // asyn handlers should reach only last total.
    BOOST_CHECK_EQUAL( float(-15.0), float_sub );
    h1.disconnect();
    h2.disconnect();
}

BOOST_AUTO_TEST_CASE( testSyncListener )
{
    // No completer:
    reset();
    Handle h = t_event.connect( boost::bind(&EventTest::listener, this) );
    BOOST_CHECK( h.connected() );
    t_event();
    h.disconnect();
    BOOST_CHECK( !h.connected() );

    BOOST_CHECK( t_listener_value );
    BOOST_CHECK( !t_completer_value );
}

BOOST_AUTO_TEST_CASE( testSyncListenerThreadCompleter )
{
    // in thread completer:
    reset();
    BOOST_CHECK(act.start());
    // Manually call step
    event_proc->step();
    Handle h1 = t_event.connect( boost::bind(&EventTest::listener,this));
    Handle h2 = t_event.connect( boost::bind(&EventTest::completer,this),
                                 event_proc );
    BOOST_CHECK( h1.connected() );
    BOOST_CHECK( h2.connected() );

    // Manually call step
    event_proc->step();

    t_event();

    // Manually call step
    event_proc->step();

    // This will block until all completers are processed.
    h1.disconnect();
    h2.disconnect();
    BOOST_CHECK( !h1.connected() );
    BOOST_CHECK( !h2.connected() );

    // Manually call step
    event_proc->step();
    event_proc->finalize();

    // now, both must be called.
    BOOST_CHECK( t_listener_value );
    BOOST_CHECK( t_completer_value );
}

BOOST_AUTO_TEST_CASE( testSyncListenerString )
{
    // No completer:
    reset();
    Handle h = t_event_string.connect( boost::bind(&EventTest::listenerString,this,_1) );
    BOOST_CHECK( h.connected() );
    t_event_string( std::string("What") );
    h.disconnect();
    BOOST_CHECK( !h.connected() );

    BOOST_CHECK_EQUAL( t_listener_what, std::string("What") );
    BOOST_CHECK( !t_listener_value );
    BOOST_CHECK( !t_completer_value );
}

BOOST_AUTO_TEST_CASE( testCompletionProcessor )
{
    // in thread completer:
    reset();

    BOOST_CHECK( CompletionProcessor::Instance()->isActive() );

    Handle h2 = t_event.connect(boost::bind(&EventTest::completer,this),
                                CompletionProcessor::Instance()
                                 );
    BOOST_CHECK( h2.connected() );

    t_event();

    // This will block until all completers are processed.
    usleep(100000);
    h2.disconnect();
    BOOST_CHECK( !h2.connected() );
    // CP must be called.
    BOOST_CHECK( t_completer_value );
}

BOOST_AUTO_TEST_CASE( testRTEvent )
{
    reset();

    BOOST_CHECK(act.start());

    Handle hl( t_event.setup( boost::bind(&EventTest::listener,this) ) );
    Handle hc( t_event.setup( boost::bind(&EventTest::completer,this), event_proc ) );

    BOOST_CHECK( !hl.connected() );
    BOOST_CHECK( !hc.connected() );

    hl.connect();
    hc.connect();
    BOOST_CHECK( hl.connected() );
    BOOST_CHECK( hc.connected() );

    t_event();


    hl.disconnect();
    BOOST_CHECK( !hl.connected() );
    BOOST_CHECK( t_listener_value );
    BOOST_CHECK( !t_completer_value );

    event_proc->step();
    BOOST_CHECK( t_completer_value );
    hc.disconnect();
    BOOST_CHECK( !hc.connected() );

    reset();
    t_event();
    event_proc->step();

    BOOST_CHECK( t_completer_value == false );
    BOOST_CHECK( t_listener_value == false );

    hl.connect();
    hc.connect();
    t_event();

    hl.disconnect();
    BOOST_CHECK( !hl.connected() );
    BOOST_CHECK( t_listener_value == true );
    BOOST_CHECK( t_completer_value == false );

    event_proc->step();
    hc.disconnect();
    BOOST_CHECK( !hc.connected() );
    BOOST_CHECK( t_completer_value == true );

}

BOOST_AUTO_TEST_SUITE_END()

