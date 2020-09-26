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

#include "tests/unit.hpp"

#include "rtt/internal/Signal.hpp"
#include "rtt/extras/SlaveActivity.hpp"
#include <string>
#include "rtt/rtt-detail-fwd.hpp"
#include "rtt/Logger.hpp"
#include "rtt/base/RunnableInterface.hpp"
#include "rtt/extras/SimulationActivity.hpp"
#include "rtt/extras/SimulationThread.hpp"
#include "rtt/Activity.hpp"
#include "rtt/os/Atomic.hpp"

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

using namespace RTT;
using namespace boost;
using namespace RTT::detail;



struct Runner : public RunnableInterface
{
    bool result;
    int data;
    Signal<void(int)>& e;
    Handle h1;
    Runner( Signal<void(int)>& e_ ) : e(e_) {
    }

    bool initialize() {
        result = false;
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h1 = e.connect( boost::bind(&Runner::handle,this, _1) );
        return true;
    }
    void step() {
        e( 123456 );
    }

    // blocking implementation
    void loop() {
        e( 123456 );
    }

    bool breakLoop() {
        return true;
    }

    void finalize() {
        h1.disconnect();
    }

    void handle(int i) {
        data = i;
        if (data == 123456)
            result = true;
        else
            result = false;
    }
};

struct SelfRemover : public RunnableInterface
{
    Signal<void(void)>& e;
    Handle h1;
    SelfRemover( Signal<void(void)>& e_ ) : e(e_) {
    }

    bool initialize() {
        // connect sync and async handler with event
        // and run async handler in thread of this task.
        h1 = e.setup( boost::bind(&SelfRemover::handle,this));
        return true;
    }
    void step() {
        h1.connect();
        e();
    }

    void finalize() {
    }

    void handle(void) {
        // do not emit within handle!
        // first disconnect self, then reconnect
        h1.disconnect();
        h1.connect();
        h1.disconnect();
    }
};

struct CrossRemover : public RunnableInterface
{
    Signal<void(void)>& e;
    Handle h1;
    CrossRemover( Signal<void(void)>& e_ ) : e(e_), count(0) {
    }
    int count;

    bool initialize() {
        // connect sync handler with event
        e.connect( boost::bind(&CrossRemover::handle,this));
        h1 = e.connect( boost::bind(&CrossRemover::handle,this));
        e.connect( boost::bind(&CrossRemover::handle,this));
        return true;
    }
    void step() {
        // for syn :
        count = 0;
        e();
        h1.disconnect(); // disconnect !
    }

    void finalize() {
    }

    void handle(void) {
        if ( count == 0 ) {
            h1.disconnect(); // remove next handler
        }
        if ( count == 1 ){
            h1.connect();  // insert again
        }
    }
};

class EventTest
{
public:

    Signal<void( void )> t_event;
    Signal<void( std::string )> t_event_string;
    Signal<int( float, float )> t_event_float;
    bool t_listener_value;
    std::string t_listener_what;
    float float_sum;
public:

    EventTest(): t_listener_value(false), float_sum(0.0) {
        reset();
    }

    ~EventTest(){
    }


    void listener(void)
    {
        t_listener_value = true;
    }

    void listenerString(const std::string& what)
    {
        t_listener_what = what;
    }

    int float_listener(float a, float b)
    {
        Logger::log() << Logger::Debug << "float_listener "<< a<<", "<<b<<Logger::endl;
        float_sum += a + b;
        return 1;
    }

    void reset()
    {
        t_listener_value = false;
        t_listener_what = "";
    }

    void float_reference(float& f) {
        log(Debug) << "Received f:"<<f<<endlog();
        f = 10.0;
        float_sum +=f ;
    }
};

static AtomicInt testConcurrentEmitHandlerCount;

void testConcurrentEmitHandler(void)
{
    testConcurrentEmitHandlerCount.inc();
}

class EmitAndcount
    :public RunnableInterface
{
public:
    EmitAndcount(Signal<void(void)> &ev)
        : mev(ev), count(0) {}
    Signal<void(void)> &mev;
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
    Signal<void(int)> event;
    Runner runobj(event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK(task.start());
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( runobj.result );
}

BOOST_AUTO_TEST_CASE( testSelfRemoval )
{
    SelfRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK( task.start() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( task.stop() );
}

/**
 * Tests if we can emit a signal that takes an argument by reference.
 */
BOOST_AUTO_TEST_CASE( testReference )
{
    Signal<void(float&)> event;

    Handle h = event.connect( boost::bind(&testReference::float_reference, this, _1) );

    float f = 5.0;
    event.emit( f );

    // Note: we actually don't guarantee that this will succeed.
    // The current implementation allows it though. This check may
    // be removed in the future.
    BOOST_CHECK( f == 10.0 );
    BOOST_CHECK(float_sum == 10.0 );
}

BOOST_AUTO_TEST_CASE( testCrossRemoval )
{
    CrossRemover runobj(t_event);
    SimulationActivity task(0.01, &runobj);
    BOOST_CHECK( task.start() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( task.stop() );
}

#ifdef OROCOS_TARGET_GNULINUX
BOOST_AUTO_TEST_CASE( testConcurrentEmit )
{
    testConcurrentEmitHandlerCount.set(0);
    Signal<void(void)> event;
    EmitAndcount arunobj(event);
    EmitAndcount brunobj(event);
    EmitAndcount crunobj(event);
    EmitAndcount drunobj(event);
    Activity atask(ORO_SCHED_OTHER, 0, 0, &arunobj);
    Activity btask(ORO_SCHED_OTHER, 0, 0, &brunobj);
    Activity ctask(ORO_SCHED_OTHER, 0, 0, &crunobj);
    Activity dtask(ORO_SCHED_OTHER, 0, 0, &drunobj);
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
    Signal<void(int)> event;
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
    Handle h1 = t_event_float.connect( boost::bind(&EventTest::float_listener, this,_1,_2) );

    t_event_float(1.0, 4.0);
    BOOST_CHECK_EQUAL( float(5.0), float_sum );

    float a = 10.0, b = 5.0;
    t_event_float(a, b);
    BOOST_CHECK_EQUAL( float(20.0), float_sum );

    h1.disconnect();
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
}

BOOST_AUTO_TEST_CASE( testSyncListenerString )
{
    reset();
    Handle h = t_event_string.connect( boost::bind(&EventTest::listenerString,this,_1) );
    BOOST_CHECK( h.connected() );
    t_event_string( std::string("What") );
    h.disconnect();
    BOOST_CHECK( !h.connected() );

    BOOST_CHECK_EQUAL( t_listener_what, std::string("What") );
    BOOST_CHECK( !t_listener_value );
}

BOOST_AUTO_TEST_CASE( testRTEvent )
{
    reset();

    Handle hl( t_event.setup( boost::bind(&EventTest::listener,this) ) );

    BOOST_CHECK( !hl.connected() );

    hl.connect();
    BOOST_CHECK( hl.connected() );

    t_event();


    hl.disconnect();
    BOOST_CHECK( !hl.connected() );
    BOOST_CHECK( t_listener_value );

    reset();
    t_event();
    BOOST_CHECK( t_listener_value == false );

    hl.connect();
    t_event();

    hl.disconnect();
    BOOST_CHECK( !hl.connected() );
    BOOST_CHECK( t_listener_value == true );
}

BOOST_AUTO_TEST_SUITE_END()

