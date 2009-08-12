/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test_3.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "generictask_test_3.hpp"

#include <iostream>
#include <Ports.hpp>
#include <interface/DataObjectInterfaces.hpp>
#include <interface/BufferLocked.hpp>

#include <extras/SlaveActivity.hpp>
#include <extras/SequentialActivity.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>

#include <boost/function_types/function_type_signature.hpp>
#include <Method.hpp>

using namespace std;
using namespace RTT;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


// Test TaskContext states.
class StatesTC
    : public RTT::TaskContext
{
public:
    StatesTC()
        : TaskContext("TC", PreOperational)
    {
        this->resetFlags();
        validconfig = true;
        validstart = true;
        validreset = true;
        validactivate = true;
        do_error = false;
    }

    void resetFlags()
    {
        didconfig = false;
        didstart = false;
        didstop = false;
        didcleanup = false;
        didupdate = false;
        didreset = false;
        didactivate = false;
    }

    bool configureHook() {
        BOOST_CHECK( mTaskState <= Stopped );
        didconfig = true;
        return validconfig;
    }

    bool startHook() {
        BOOST_CHECK( mTaskState == Stopped || mTaskState == Active);
        didstart = true;
        return validstart;
    }

    void stopHook() {
        BOOST_CHECK( mTaskState >= Running || mTaskState == Active || mTaskState == FatalError);
        didstop = true;
    }

    void cleanupHook() {
        BOOST_CHECK( mTaskState == Stopped );
        didcleanup = true;
    }

    void updateHook() {
        BOOST_CHECK( mTaskState == Running );
        didupdate = true;
        if (do_error)
            this->fatal();
    }

    bool resetHook() {
        didreset = true;
        return validreset;
    }

    bool activateHook() {
        didactivate = true;
        return  validactivate;
    }

    bool validconfig, didconfig;
    bool validstart, didstart;
    bool didstop;
    bool didcleanup;
    bool didupdate;
    bool validreset, didreset, didactivate, validactivate;
    bool do_error;
};
class EventPortsTC : public TaskContext
{
public:
    bool had_event;
    int  nb_events;
    EventPortsTC(): TaskContext("eptc") { resetStats(); }
    void updateHook(std::vector<PortInterface*> const& updated_ports)
    {
        nb_events += updated_ports.size();
        had_event = true;
    }
    void resetStats() {
        nb_events = 0;
        had_event = false;
    }
};

void
Generic_TaskTest_3::setUp()
{
    tc =  new TaskContext( "root", TaskContext::Stopped );
    tce = new EventPortsTC();
    tc2 = new EventPortsTC();
    tce->setActivity( new SequentialActivity() );
    tc2->setActivity( new SequentialActivity() );
    stc = new StatesTC();
    tsim = new SimulationActivity(0.001, tc->engine() );
    stsim = new SimulationActivity(0.001, stc->engine() );
    SimulationThread::Instance()->stop();
}


void
Generic_TaskTest_3::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tsim->stop();
    stsim->stop();
    delete tc;
    delete tce;
    delete tc2;
    delete tsim;
    delete stc;
    delete stsim;
}


void Generic_TaskTest_3::new_data_listener(PortInterface* port)
{
    signalled_port = port;
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  Generic_TaskTest3Suite,  Generic_TaskTest_3 )


BOOST_AUTO_TEST_CASE( testPeriod)
{
    // check unconfigured TC
    TaskContext pertc("PerTC");
    BOOST_CHECK( pertc.getPeriod() == 0.0 );

    // check periodic TC
    SlaveActivity sa(1.0, pertc.engine());
    BOOST_CHECK( sa.getPeriod() == 1.0 );
    BOOST_CHECK( pertc.getPeriod() == 1.0 );

    // check non periodic TC
    SlaveActivity sb(0.0, pertc.engine());
    BOOST_CHECK( sb.getPeriod() == 0.0 );
    BOOST_CHECK( pertc.getPeriod() == 0.0 );

}
BOOST_AUTO_TEST_CASE( testTCStates)
{
    // Test the states of a Default TC, no user methods.
    BOOST_CHECK( tc->getTaskState() == TaskContext::Stopped );

    // Configure in Stop
    BOOST_CHECK( tc->isConfigured() == true );
    BOOST_CHECK( tc->isRunning() == false );
    BOOST_CHECK( tc->isActive() == false );
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->resetError() == false );

    // Stop to Running
    BOOST_CHECK( tc->start() == true );
    BOOST_CHECK( tc->isRunning() == true );
    BOOST_CHECK( tc->isActive() == true );
    BOOST_CHECK( tc->configure() == false );
    BOOST_CHECK( tc->start() == false );
    BOOST_CHECK( tc->cleanup() == false );
    BOOST_CHECK( tc->resetError() == false );

    // Running to Stop
    BOOST_CHECK( tc->stop() == true );
    BOOST_CHECK( tc->isRunning() == false);
    BOOST_CHECK( tc->isActive() == false );
    BOOST_CHECK( tc->stop() == false );
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->isConfigured() == true );

    // Stop to Active to Stop
    BOOST_CHECK( tc->activate() == true );
    BOOST_CHECK( tc->isActive() == true);
    BOOST_CHECK( tc->isRunning() == false );
    BOOST_CHECK( tc->configure() == false );
    BOOST_CHECK( tc->resetError() == false );
    BOOST_CHECK( tc->stop() == true );
    BOOST_CHECK( tc->isConfigured() == true );

    // Stop to Active to Running to Stop
    BOOST_CHECK( tc->activate() == true );
    BOOST_CHECK( tc->start() == true );
    BOOST_CHECK( tc->isRunning() == true );
    BOOST_CHECK( tc->isActive() == true );
    BOOST_CHECK( tc->configure() == false );
    BOOST_CHECK( tc->start() == false );
    BOOST_CHECK( tc->cleanup() == false );
    BOOST_CHECK( tc->resetError() == false );
    BOOST_CHECK( tc->stop() == true );
    BOOST_CHECK( tc->isConfigured() == true );

    // Stop to PreOp
    BOOST_CHECK( tc->cleanup() == true );
    BOOST_CHECK( tc->isConfigured() == false);
    BOOST_CHECK( tc->isRunning() == false);

    // PreOp to stop
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->isConfigured() == true);
    BOOST_CHECK( tc->isRunning() == false);
    BOOST_CHECK( tc->isActive() == false );

}

BOOST_AUTO_TEST_CASE( testSpecialTCStates)
{
    // Test the states of a Specially crafted TC, requiring configure etc.
    BOOST_CHECK( stc->getTaskState() == TaskContext::PreOperational );

    // PreOperational state:
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == true );
    // test flags
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();

    // Stopped state:
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    stc->resetFlags();
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->start() == false );
    BOOST_CHECK( stc->cleanup() == false );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();


    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == true );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();

    // Back to stopped
    BOOST_CHECK( stc->stop() == true );
    BOOST_CHECK( stc->didstop == true );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == true );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();

    BOOST_CHECK( stc->isRunning() == false);
    BOOST_CHECK( stc->stop() == false );
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->isConfigured() == true );
    // test flags
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();


    // Active state:
    BOOST_CHECK( stc->activate() == true );
    BOOST_CHECK( stc->didactivate == true );
    stc->resetFlags();
    BOOST_CHECK( stc->isActive() == true );
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->activate() == false );
    BOOST_CHECK( stc->cleanup() == false );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();
    // continue to start
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    stc->resetFlags();
    BOOST_CHECK( stc->isActive() == true );
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->activate() == false );
    BOOST_CHECK( stc->cleanup() == false );
    BOOST_CHECK( stc->start() == false );
    // running to stop to active to stop
    BOOST_CHECK( stc->stop() == true );
    BOOST_CHECK( stc->activate() == true );
    stc->resetFlags();
    BOOST_CHECK( stc->stop() == true );
    BOOST_CHECK( stc->didstop == true );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();

    // Stopped to PreOp state:
    BOOST_CHECK( stc->cleanup() == true );
    BOOST_CHECK( stc->isConfigured() == false);
    BOOST_CHECK( stc->isRunning() == false);
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == true );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();
    BOOST_CHECK( stc->start() == false );
    BOOST_CHECK( stc->cleanup() == false );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();


    // PreOperational to Stopped state:
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->isConfigured() == true);
    BOOST_CHECK( stc->isRunning() == false);
    // test flags
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    BOOST_CHECK( stc->didactivate == false );
    stc->resetFlags();

}

BOOST_AUTO_TEST_CASE( testFailingTCStates)
{
    // Test the states of a TC failing in transitions
    stc->validconfig = false;
    stc->validstart = false;
    stc->validactivate = false;
    stc->validreset = false;

    // PreOperational state:
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == false );
    stc->resetFlags();

    // Retry:
    stc->validconfig = true;
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == false );
    stc->resetFlags();

    // Stopped state:
    BOOST_CHECK( stc->start() == false );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == false );
    BOOST_CHECK( stc->isConfigured() == true );
    stc->resetFlags();

    // Retry:
    stc->validstart = true;
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->isActive() == true );
    stc->resetFlags();

    // Stopped to Active
    BOOST_CHECK( stc->stop() == true );
    BOOST_CHECK( stc->activate() == false );
    BOOST_CHECK( stc->didactivate == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == false );
    BOOST_CHECK( stc->isConfigured() == true );
    stc->resetFlags();
    // Retry:
    stc->validactivate = true;
    BOOST_CHECK( stc->activate() == true );
    BOOST_CHECK( stc->didactivate == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isActive() == true );
    BOOST_CHECK( stc->isRunning() == false );
    stc->resetFlags();

    // Active to Running
    stc->validstart = false;
    BOOST_CHECK( stc->start() == false );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == true );
    BOOST_CHECK( stc->isConfigured() == true );
    stc->resetFlags();

    // Retry:
    stc->validstart = true;
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->isActive() == true );
    stc->resetFlags();

    // Error state.
    stc->do_error = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->inFatalError() == true );
    BOOST_CHECK( stc->resetError() == true ); // brings us to PreOp.
    BOOST_CHECK( stc->didreset == true );
    BOOST_CHECK( stc->isConfigured() == false);
    // Retry:
    BOOST_CHECK( stc->configure() == true);
    BOOST_CHECK( stc->start() == true );
    stc->validreset = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->inFatalError() == true );
    BOOST_CHECK( stc->resetError() == true ); // brings us to Stopped.
    BOOST_CHECK( stc->didreset == true );
    BOOST_CHECK( stc->isConfigured() == true);

}

BOOST_AUTO_TEST_CASE( testExecutionEngine)
{
    // no owner:
    ExecutionEngine ee1(0);
    ExecutionEngine ee2(0);

    // test setActivity:
    BOOST_CHECK( tsim->run(&ee1) );
    BOOST_CHECK( SimulationThread::Instance()->run(5) );

    // this also tests setActivity:
    BOOST_CHECK( tsim->run(&ee2) );
    BOOST_CHECK( SimulationThread::Instance()->run(5) );

    {
        TaskCore tc1("tc1", &ee2);
        TaskCore tc2("tc2", &ee2);

        // run with two children.
        BOOST_CHECK( SimulationThread::Instance()->run(5) );
    }
    // children removed again:
    BOOST_CHECK( SimulationThread::Instance()->run(5) );
    tsim->run(0);
}

BOOST_AUTO_TEST_CASE( testProperties)
{
    Property<double> d1("d1", "desc1", 1.234);
    tc->properties()->addProperty( &d1);

    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( double(1.234), tc->properties()->getProperty<double>("d1")->get() );

    // test setup of mirror:
    Property<string> s1;
    BOOST_CHECK( !s1.ready() );
    Property<string> s2("hello","description", "world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc->properties()->addProperty( &s1 ) == false);
    BOOST_CHECK(tc->properties()->addProperty( &s2 ) );
    s1 = tc->properties()->getProperty<string>("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("description"), s1.getDescription() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

#if 0
    BOOST_CHECK(tc->writeProperties("Generic_TaskTest_3_Properties.cpf"));
    BOOST_CHECK( tc->readProperties("Generic_TaskTest_3_Properties.cpf"));
#endif
}

BOOST_AUTO_TEST_CASE( testAttributes)
{
    // test attribute repository:
    Attribute<int> i1("i1");
    Attribute<double> d1("d1", 1.234);
    BOOST_CHECK( i1.ready() );
    BOOST_CHECK( d1.ready() );
    BOOST_CHECK(tc->attributes()->addAttribute( &d1 ));
    BOOST_CHECK(tc->attributes()->addAttribute( &i1 ));

    i1.set( 3 );
    BOOST_CHECK_EQUAL( double(1.234), d1.get() );
    BOOST_CHECK_EQUAL( int(3), i1.get() );

    BOOST_CHECK_EQUAL( double(1.234), tc->attributes()->getAttribute<double>("d1")->get() );
    BOOST_CHECK_EQUAL( int(3),        tc->attributes()->getAttribute<int>("i1")->get() );

    // test setup of mirror:
    Attribute<string> s1;
    BOOST_CHECK( !s1.ready() );
    Attribute<string> s2("hello","world");
    BOOST_CHECK( s2.ready() );

    BOOST_CHECK(tc->attributes()->addAttribute( &s1 ) == false);
    BOOST_CHECK(tc->attributes()->addAttribute( &s2 ) );
    s1 = tc->attributes()->getAttribute<string>("hello");
    BOOST_CHECK( s1.ready() );

    BOOST_CHECK_EQUAL(std::string("hello"), s1.getName() );
    BOOST_CHECK_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    BOOST_CHECK_EQUAL(std::string("universe"), s2.get() );

}

BOOST_AUTO_TEST_CASE( testPortTaskInterface )
{
    InputPort<int> rp1("Port1");
    OutputPort<int> wp1("Port1");
    InputPort<int> rp2("Port2");
    OutputPort<int> wp2("Port2");

    BOOST_CHECK( tc->ports()->addPort( &wp1 ));
    BOOST_CHECK( tc->ports()->addPort( &rp2 ));

    // check adding same port twice.
    BOOST_CHECK( tc->ports()->addPort( &wp1 ) == false);
    {
        // also check adding different port with same name.
        InputPort<double> other_rp("Port1");
        BOOST_CHECK( tc->ports()->addPort( &other_rp ) == false);
    }

    {
        auto_ptr<TaskContext> tc1(new TaskContext( "tc", TaskContext::Stopped ));
        auto_ptr<TaskContext> tc2(new TaskContext( "tc2", TaskContext::Stopped ));

        tc1->ports()->addPort(&rp1);
        tc1->ports()->addPort(&wp2);
        tc2->ports()->addPort(&rp2);
        tc2->ports()->addPort(&wp1);

        BOOST_CHECK( tc1->connectPorts(tc2.get()) );
        BOOST_CHECK( wp1.connected() );
        BOOST_CHECK( rp1.connected() );
        wp1.write(2);
        int value = 0;
        BOOST_CHECK( rp1.read(value) );
        BOOST_CHECK_EQUAL(2, value);

        BOOST_CHECK( wp2.connected() );
        BOOST_CHECK( rp2.connected() );
        wp2.write(3);
        value = 0;
        BOOST_CHECK( rp2.read(value) );
        BOOST_CHECK_EQUAL(3, value);
    }

    // Tasks have been destroyed, but the ports not. Automatic disconnection
    // is done when port objects are disconnected
    BOOST_CHECK( rp1.connected() );
    BOOST_CHECK( rp2.connected() );
    BOOST_CHECK( wp1.connected() );
    BOOST_CHECK( wp2.connected() );
}

BOOST_AUTO_TEST_CASE(testPortConnectionInitialization)
{
    OutputPort<int> wp("WriterName", true);
    InputPort<int> rp("ReaderName", ConnPolicy::data(true));

    wp.createConnection(rp);
    int value = 0;
    BOOST_CHECK( !rp.read(value) );
    BOOST_CHECK( !wp.getLastWrittenValue(value) );
    wp.write(10);
    BOOST_CHECK( rp.read(value) );

    wp.disconnect(rp);
    BOOST_CHECK( !wp.connected() );
    BOOST_CHECK( !rp.connected() );

    value = 0;
    BOOST_CHECK( wp.getLastWrittenValue(value) );
    BOOST_CHECK_EQUAL( 10, value );
    BOOST_CHECK_EQUAL( 10, wp.getLastWrittenValue() );

    value = 0;
    wp.createConnection(rp);
    BOOST_CHECK( rp.read(value) );
    BOOST_CHECK_EQUAL( 10, value );
}

BOOST_AUTO_TEST_CASE(testPortSimpleConnections)
{
    OutputPort<int> wp("WriterName");
    InputPort<int> rp("ReaderName", ConnPolicy::data());

    BOOST_CHECK( !wp.connected() );
    BOOST_CHECK( !rp.connected() );
    {
        int value;
        BOOST_CHECK( !rp.read(value) );
        wp.write(value); // just checking if is works or if it crashes
    }

    wp.createConnection(rp);
    BOOST_CHECK( wp.connected() );
    BOOST_CHECK( rp.connected() );

    { 
        int value = 0;
        BOOST_CHECK( !rp.read(value) );
        wp.write(1);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK( 1 == value );
    }

    rp.clear();
    { 
        int value = 0;
        BOOST_CHECK( !rp.read(value) );
        wp.write(1);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK( 1 == value );
    }

    // Try disconnecting starting at the writer. Disconnecting from the reader
    // will be done later
    wp.disconnect();
    BOOST_CHECK( !wp.connected() );
    BOOST_CHECK( !rp.connected() );
    {
        int value;
        wp.write(value);
        BOOST_CHECK( !rp.read(value) );
    }
    wp.disconnect(); // calling it when not connected should be fine as well

    { 
        int value = 0;
        BOOST_CHECK( !rp.read(value) );
        wp.createBufferConnection(rp, 4);
        BOOST_CHECK( !rp.read(value) );
        wp.write(1);
        wp.write(2);
        wp.write(3);
        wp.write(4);
        wp.write(5);
        BOOST_CHECK_EQUAL(0, value);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK_EQUAL(1, value);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK_EQUAL(2, value);

        rp.clear();
        BOOST_CHECK( !rp.read(value) );
        wp.write(10);
        wp.write(20);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK_EQUAL(10, value);
        BOOST_CHECK( rp.read(value) );
        BOOST_CHECK_EQUAL(20, value);
        BOOST_CHECK( !rp.read(value) );
    }

    // Try disconnecting from the reader this time
    rp.disconnect();
    BOOST_CHECK( !wp.connected() );
    BOOST_CHECK( !rp.connected() );
    {
        int value;
        wp.write(value);
        BOOST_CHECK( !rp.read(value) );
    }
    rp.disconnect(); // calling it when not connected should be fine as well

    // Test automatic disconnection because of port destruction
    {
        InputPort<int> rp("ReaderName", ConnPolicy::data());
        BOOST_CHECK(wp.createConnection(rp));
        BOOST_CHECK( wp.connected() );
        BOOST_CHECK( rp.connected() );
    }
    BOOST_CHECK( !wp.connected() );
}

BOOST_AUTO_TEST_CASE(testPortForkedConnections)
{
    OutputPort<int> wp("W");
    InputPort<int> rp1("R1", ConnPolicy::data());
    InputPort<int> rp2("R2", ConnPolicy::buffer(4));

    wp.createConnection(rp1);
    BOOST_CHECK( wp.connected() );
    BOOST_CHECK( rp1.connected() );
    BOOST_CHECK( !rp2.connected() );
    wp.createConnection(rp2);
    BOOST_CHECK( wp.connected() );
    BOOST_CHECK( rp1.connected() );
    BOOST_CHECK( rp2.connected() );

    wp.write(10);
    wp.write(15);
    wp.write(20);
    wp.write(25);
    wp.write(30);

    int value = 0;
    BOOST_CHECK( rp1.read(value));
    BOOST_CHECK_EQUAL(30, value);

    BOOST_CHECK( rp2.read(value));
    BOOST_CHECK_EQUAL(10, value);
    BOOST_CHECK( rp2.read(value));
    BOOST_CHECK_EQUAL(15, value);
    BOOST_CHECK( rp2.read(value));
    BOOST_CHECK_EQUAL(20, value);
    BOOST_CHECK( rp2.read(value));
    BOOST_CHECK_EQUAL(25, value);
    BOOST_CHECK( !rp2.read(value));

    // Now removes only the buffer port
    wp.disconnect(rp2);
    BOOST_CHECK( wp.connected() );
    BOOST_CHECK( rp1.connected() );
    BOOST_CHECK( !rp2.connected() );

    wp.write(10);
    BOOST_CHECK( rp1.read(value));
    BOOST_CHECK_EQUAL(10, value);

    // And finally the other port as well
    wp.disconnect(rp1);
    BOOST_CHECK( !wp.connected() );
    BOOST_CHECK( !rp1.connected() );
    BOOST_CHECK( !rp2.connected() );
}

BOOST_AUTO_TEST_CASE( testPortObjects)
{
    OutputPort<double> wp1("Write");
    InputPort<double>  rp1("Read");

    tc->ports()->addPort( &wp1 );
    tc->ports()->addPort( &rp1 );

    // Check if ports were added as objects as well
    BOOST_CHECK( tc->getObject("Write") != 0 );
    BOOST_CHECK( tc->getObject("Read") != 0 );

    // Set initial value
    wp1.write( 1.0 );

    // Connect ports.
    wp1.createConnection( rp1 );

    // Test Methods set/get
    Method<void(double)> mset;
    Method<bool(double&)> mget;

    mset = tc->getObject("Write")->methods()->getMethod<void(double)>("write");
    BOOST_CHECK( mset.ready() );

    mget = tc->getObject("Read")->methods()->getMethod<bool(double&)>("read");
    BOOST_CHECK( mget.ready() );

    mset( 3.991 );

    double get_value;
    BOOST_CHECK( mget(get_value) );
    BOOST_CHECK_EQUAL( 3.991, get_value );

    //// Finally, check cleanup. Ports and port objects must be gone:
    tc->ports()->removePort("Reader");
    BOOST_CHECK( tc->getObject("Reader") == 0 );
    BOOST_CHECK( tc->ports()->getPort("Reader") == 0 );

    tc->ports()->removePort("Writer");
    BOOST_CHECK( tc->getObject("Writer") == 0 );
    BOOST_CHECK( tc->ports()->getPort("Writer") == 0 );
}

BOOST_AUTO_TEST_CASE(testPortSignalling)
{
    OutputPort<double> wp1("Write");
    InputPort<double>  rp1("Read");

    Handle hl( rp1.getNewDataOnPortEvent()->setup(
                boost::bind(&Generic_TaskTest_3::new_data_listener, this, _1) ) );
    hl.connect();

    wp1.createConnection(rp1, ConnPolicy::data());
    signalled_port = 0;
    wp1.write(0.1);
    BOOST_CHECK(&rp1 == signalled_port);

    wp1.disconnect();
    wp1.createConnection(rp1, ConnPolicy::buffer(2));
    signalled_port = 0;
    wp1.write(0.1);
    BOOST_CHECK(&rp1 == signalled_port);
    signalled_port = 0;
    wp1.write(0.1);
    BOOST_CHECK(&rp1 == signalled_port);
    signalled_port = 0;
    wp1.write(0.1);
    BOOST_CHECK(0 == signalled_port);
}

BOOST_AUTO_TEST_CASE(testPortDataSource)
{
    OutputPort<int> wp1("Write");
    auto_ptr<InputPortInterface> reader(dynamic_cast<InputPortInterface*>(wp1.antiClone()));
    BOOST_CHECK(wp1.connectTo(*reader, ConnPolicy::buffer(2)));

    DataSource<int>::shared_ptr source = static_cast< DataSource<int>* >(reader->getDataSource());
    BOOST_CHECK(source);

    BOOST_CHECK(!source->evaluate());
    wp1.write(10);
    wp1.write(20);
    BOOST_CHECK_EQUAL(10, source->value());
    BOOST_CHECK_EQUAL(20, source->value());
    BOOST_CHECK_EQUAL(0, source->get());
    BOOST_CHECK(!source->evaluate());
}

BOOST_AUTO_TEST_SUITE_END()

