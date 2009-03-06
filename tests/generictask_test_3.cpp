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
#include <unistd.h>
#include <iostream>
#include "Ports.hpp"
#include "DataObjectInterfaces.hpp"
#include "BufferLocked.hpp"

#include "SlaveActivity.hpp"
#include "SimulationActivity.hpp"
#include "SimulationThread.hpp"

#include <boost/function_types/function_type_signature.hpp>
#include "Method.hpp"

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( Generic_TaskTest_3 );

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
        CPPUNIT_ASSERT( mTaskState <= Stopped );
        didconfig = true;
        return validconfig;
    }

    bool startHook() {
        CPPUNIT_ASSERT( mTaskState == Stopped || mTaskState == Active);
        didstart = true;
        return validstart;
    }

    void stopHook() {
        CPPUNIT_ASSERT( mTaskState >= Running || mTaskState == Active || mTaskState == FatalError);
        didstop = true;
    }

    void cleanupHook() {
        CPPUNIT_ASSERT( mTaskState == Stopped );
        didcleanup = true;
    }

    void updateHook() {
        CPPUNIT_ASSERT( mTaskState == Running );
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


void
Generic_TaskTest_3::setUp()
{
    tc =  new TaskContext( "root", TaskContext::Stopped );
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
    delete tsim;
    delete stc;
    delete stsim;
}

void Generic_TaskTest_3::testPeriod()
{
    // check unconfigured TC
    TaskContext pertc("PerTC");
    CPPUNIT_ASSERT( pertc.getPeriod() == 0.0 );

    // check periodic TC
    SlaveActivity sa(1.0, pertc.engine());
    CPPUNIT_ASSERT( sa.getPeriod() == 1.0 );
    CPPUNIT_ASSERT( pertc.getPeriod() == 1.0 );

    // check non periodic TC
    SlaveActivity sb(0.0, pertc.engine());
    CPPUNIT_ASSERT( sb.getPeriod() == 0.0 );
    CPPUNIT_ASSERT( pertc.getPeriod() == 0.0 );

}
void Generic_TaskTest_3::testTCStates()
{
    // Test the states of a Default TC, no user methods.
    CPPUNIT_ASSERT( tc->getTaskState() == TaskContext::Stopped );

    // Configure in Stop
    CPPUNIT_ASSERT( tc->isConfigured() == true );
    CPPUNIT_ASSERT( tc->isRunning() == false );
    CPPUNIT_ASSERT( tc->isActive() == false );
    CPPUNIT_ASSERT( tc->configure() == true );
    CPPUNIT_ASSERT( tc->resetError() == false );

    // Stop to Running
    CPPUNIT_ASSERT( tc->start() == true );
    CPPUNIT_ASSERT( tc->isRunning() == true );
    CPPUNIT_ASSERT( tc->isActive() == true );
    CPPUNIT_ASSERT( tc->configure() == false );
    CPPUNIT_ASSERT( tc->start() == false );
    CPPUNIT_ASSERT( tc->cleanup() == false );
    CPPUNIT_ASSERT( tc->resetError() == false );

    // Running to Stop
    CPPUNIT_ASSERT( tc->stop() == true );
    CPPUNIT_ASSERT( tc->isRunning() == false);
    CPPUNIT_ASSERT( tc->isActive() == false );
    CPPUNIT_ASSERT( tc->stop() == false );
    CPPUNIT_ASSERT( tc->configure() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true );

    // Stop to Active to Stop
    CPPUNIT_ASSERT( tc->activate() == true );
    CPPUNIT_ASSERT( tc->isActive() == true);
    CPPUNIT_ASSERT( tc->isRunning() == false );
    CPPUNIT_ASSERT( tc->configure() == false );
    CPPUNIT_ASSERT( tc->resetError() == false );
    CPPUNIT_ASSERT( tc->stop() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true );

    // Stop to Active to Running to Stop
    CPPUNIT_ASSERT( tc->activate() == true );
    CPPUNIT_ASSERT( tc->start() == true );
    CPPUNIT_ASSERT( tc->isRunning() == true );
    CPPUNIT_ASSERT( tc->isActive() == true );
    CPPUNIT_ASSERT( tc->configure() == false );
    CPPUNIT_ASSERT( tc->start() == false );
    CPPUNIT_ASSERT( tc->cleanup() == false );
    CPPUNIT_ASSERT( tc->resetError() == false );
    CPPUNIT_ASSERT( tc->stop() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true );

    // Stop to PreOp
    CPPUNIT_ASSERT( tc->cleanup() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == false);
    CPPUNIT_ASSERT( tc->isRunning() == false);

    // PreOp to stop
    CPPUNIT_ASSERT( tc->configure() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true);
    CPPUNIT_ASSERT( tc->isRunning() == false);
    CPPUNIT_ASSERT( tc->isActive() == false );

}

void Generic_TaskTest_3::testSpecialTCStates()
{
    // Test the states of a Specially crafted TC, requiring configure etc.
    CPPUNIT_ASSERT( stc->getTaskState() == TaskContext::PreOperational );

    // PreOperational state:
    CPPUNIT_ASSERT( stc->isConfigured() == false );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    // test flags
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();

    // Stopped state:
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    stc->resetFlags();
    CPPUNIT_ASSERT( stc->isRunning() == true );
    CPPUNIT_ASSERT( stc->configure() == false );
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->cleanup() == false );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();


    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == true );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();

    // Back to stopped
    CPPUNIT_ASSERT( stc->stop() == true );
    CPPUNIT_ASSERT( stc->didstop == true );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == true );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();

    CPPUNIT_ASSERT( stc->isRunning() == false);
    CPPUNIT_ASSERT( stc->stop() == false );
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();


    // Active state:
    CPPUNIT_ASSERT( stc->activate() == true );
    CPPUNIT_ASSERT( stc->didactivate == true );
    stc->resetFlags();
    CPPUNIT_ASSERT( stc->isActive() == true );
    CPPUNIT_ASSERT( stc->configure() == false );
    CPPUNIT_ASSERT( stc->activate() == false );
    CPPUNIT_ASSERT( stc->cleanup() == false );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();
    // continue to start
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    stc->resetFlags();
    CPPUNIT_ASSERT( stc->isActive() == true );
    CPPUNIT_ASSERT( stc->isRunning() == true );
    CPPUNIT_ASSERT( stc->configure() == false );
    CPPUNIT_ASSERT( stc->activate() == false );
    CPPUNIT_ASSERT( stc->cleanup() == false );
    CPPUNIT_ASSERT( stc->start() == false );
    // running to stop to active to stop
    CPPUNIT_ASSERT( stc->stop() == true );
    CPPUNIT_ASSERT( stc->activate() == true );
    stc->resetFlags();
    CPPUNIT_ASSERT( stc->stop() == true );
    CPPUNIT_ASSERT( stc->didstop == true );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();

    // Stopped to PreOp state:
    CPPUNIT_ASSERT( stc->cleanup() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == false);
    CPPUNIT_ASSERT( stc->isRunning() == false);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == true );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->cleanup() == false );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();


    // PreOperational to Stopped state:
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true);
    CPPUNIT_ASSERT( stc->isRunning() == false);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    CPPUNIT_ASSERT( stc->didactivate == false );
    stc->resetFlags();

}

void Generic_TaskTest_3::testFailingTCStates()
{
    // Test the states of a TC failing in transitions
    stc->validconfig = false;
    stc->validstart = false;
    stc->validactivate = false;
    stc->validreset = false;

    // PreOperational state:
    CPPUNIT_ASSERT( stc->isConfigured() == false );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->configure() == false );
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->isConfigured() == false );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isActive() == false );
    stc->resetFlags();

    // Retry:
    stc->validconfig = true;
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isActive() == false );
    stc->resetFlags();

    // Stopped state:
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isActive() == false );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    stc->resetFlags();

    // Retry:
    stc->validstart = true;
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isRunning() == true );
    CPPUNIT_ASSERT( stc->isActive() == true );
    stc->resetFlags();

    // Stopped to Active
    CPPUNIT_ASSERT( stc->stop() == true );
    CPPUNIT_ASSERT( stc->activate() == false );
    CPPUNIT_ASSERT( stc->didactivate == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isActive() == false );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    stc->resetFlags();
    // Retry:
    stc->validactivate = true;
    CPPUNIT_ASSERT( stc->activate() == true );
    CPPUNIT_ASSERT( stc->didactivate == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isActive() == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    stc->resetFlags();

    // Active to Running
    stc->validstart = false;
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isActive() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    stc->resetFlags();

    // Retry:
    stc->validstart = true;
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isRunning() == true );
    CPPUNIT_ASSERT( stc->isActive() == true );
    stc->resetFlags();

    // Error state.
    stc->do_error = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    CPPUNIT_ASSERT( stc->inFatalError() == true );
    CPPUNIT_ASSERT( stc->resetError() == true ); // brings us to PreOp.
    CPPUNIT_ASSERT( stc->didreset == true );
    CPPUNIT_ASSERT( stc->isConfigured() == false);
    // Retry:
    CPPUNIT_ASSERT( stc->configure() == true);
    CPPUNIT_ASSERT( stc->start() == true );
    stc->validreset = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    CPPUNIT_ASSERT( stc->inFatalError() == true );
    CPPUNIT_ASSERT( stc->resetError() == true ); // brings us to Stopped.
    CPPUNIT_ASSERT( stc->didreset == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true);

}

void Generic_TaskTest_3::testExecutionEngine()
{
    // no owner:
    ExecutionEngine ee1(0);
    ExecutionEngine ee2(0);

    // test setActivity:
    CPPUNIT_ASSERT( tsim->run(&ee1) );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(5) );

    // this also tests setActivity:
    CPPUNIT_ASSERT( tsim->run(&ee2) );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(5) );

    {
        TaskCore tc1("tc1", &ee2);
        TaskCore tc2("tc2", &ee2);

        // run with two children.
        CPPUNIT_ASSERT( SimulationThread::Instance()->run(5) );
    }
    // children removed again:
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(5) );
    tsim->run(0);
}

void Generic_TaskTest_3::testProperties()
{
    Property<double> d1("d1", "desc1", 1.234);
    tc->properties()->addProperty( &d1);

    CPPUNIT_ASSERT_EQUAL( double(1.234), d1.get() );
    CPPUNIT_ASSERT_EQUAL( double(1.234), tc->properties()->getProperty<double>("d1")->get() );

    // test setup of mirror:
    Property<string> s1;
    CPPUNIT_ASSERT( !s1.ready() );
    Property<string> s2("hello","description", "world");
    CPPUNIT_ASSERT( s2.ready() );

    CPPUNIT_ASSERT(tc->properties()->addProperty( &s1 ) == false);
    CPPUNIT_ASSERT(tc->properties()->addProperty( &s2 ) );
    s1 = tc->properties()->getProperty<string>("hello");
    CPPUNIT_ASSERT( s1.ready() );

    CPPUNIT_ASSERT_EQUAL(std::string("hello"), s1.getName() );
    CPPUNIT_ASSERT_EQUAL(std::string("description"), s1.getDescription() );
    CPPUNIT_ASSERT_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    CPPUNIT_ASSERT_EQUAL(std::string("universe"), s2.get() );

#if 0
    CPPUNIT_ASSERT(tc->writeProperties("Generic_TaskTest_3_Properties.cpf"));
    CPPUNIT_ASSERT( tc->readProperties("Generic_TaskTest_3_Properties.cpf"));
#endif
}

void Generic_TaskTest_3::testAttributes()
{
    // test attribute repository:
    Attribute<int> i1("i1");
    Attribute<double> d1("d1", 1.234);
    CPPUNIT_ASSERT( i1.ready() );
    CPPUNIT_ASSERT( d1.ready() );
    CPPUNIT_ASSERT(tc->attributes()->addAttribute( &d1 ));
    CPPUNIT_ASSERT(tc->attributes()->addAttribute( &i1 ));

    i1.set( 3 );
    CPPUNIT_ASSERT_EQUAL( double(1.234), d1.get() );
    CPPUNIT_ASSERT_EQUAL( int(3), i1.get() );

    CPPUNIT_ASSERT_EQUAL( double(1.234), tc->attributes()->getAttribute<double>("d1")->get() );
    CPPUNIT_ASSERT_EQUAL( int(3),        tc->attributes()->getAttribute<int>("i1")->get() );

    // test setup of mirror:
    Attribute<string> s1;
    CPPUNIT_ASSERT( !s1.ready() );
    Attribute<string> s2("hello","world");
    CPPUNIT_ASSERT( s2.ready() );

    CPPUNIT_ASSERT(tc->attributes()->addAttribute( &s1 ) == false);
    CPPUNIT_ASSERT(tc->attributes()->addAttribute( &s2 ) );
    s1 = tc->attributes()->getAttribute<string>("hello");
    CPPUNIT_ASSERT( s1.ready() );

    CPPUNIT_ASSERT_EQUAL(std::string("hello"), s1.getName() );
    CPPUNIT_ASSERT_EQUAL(std::string("world"), s1.get() );

    // Test mirroring of 'set' and 'get':
    s1.set("universe");
    CPPUNIT_ASSERT_EQUAL(std::string("universe"), s2.get() );

}

void Generic_TaskTest_3::testPortTaskInterface()
{
    ReadPort<int> rp1("Port1");
    WritePort<int> wp1("Port1");
    ReadPort<int> rp2("Port2");
    WritePort<int> wp2("Port2");

    CPPUNIT_ASSERT( tc->ports()->addPort( &wp1 ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &rp2 ));

    // check adding same port twice.
    CPPUNIT_ASSERT( tc->ports()->addPort( &wp1 ) == false);
    {
        // also check adding different port with same name.
        ReadPort<double> other_rp("Port1");
        CPPUNIT_ASSERT( tc->ports()->addPort( &other_rp ) == false);
    }

    {
        auto_ptr<TaskContext> tc1(new TaskContext( "tc", TaskContext::Stopped ));
        auto_ptr<TaskContext> tc2(new TaskContext( "tc2", TaskContext::Stopped ));

        tc1->ports()->addPort(&rp1);
        tc1->ports()->addPort(&wp2);
        tc2->ports()->addPort(&rp2);
        tc2->ports()->addPort(&wp1);

        CPPUNIT_ASSERT( tc1->connectPorts(tc2.get()) );
        CPPUNIT_ASSERT( wp1.connected() );
        CPPUNIT_ASSERT( rp1.connected() );
        wp1.write(2);
        int value = 0;
        CPPUNIT_ASSERT( rp1.read(value) );
        CPPUNIT_ASSERT_EQUAL(2, value);

        CPPUNIT_ASSERT( wp2.connected() );
        CPPUNIT_ASSERT( rp2.connected() );
        wp2.write(3);
        value = 0;
        CPPUNIT_ASSERT( rp2.read(value) );
        CPPUNIT_ASSERT_EQUAL(3, value);
    }

    // Tasks have been destroyed, but the ports not. Automatic disconnection
    // is done when port objects are disconnected
    CPPUNIT_ASSERT( rp1.connected() );
    CPPUNIT_ASSERT( rp2.connected() );
    CPPUNIT_ASSERT( wp1.connected() );
    CPPUNIT_ASSERT( wp2.connected() );
}

void Generic_TaskTest_3::testPortConnectionInitialization()
{
    WritePort<int> wp("WriterName", true);
    ReadPort<int> rp("ReaderName", ConnPolicy::data(true));

    wp.createConnection(rp);
    int value = 0;
    CPPUNIT_ASSERT( !rp.read(value) );
    CPPUNIT_ASSERT( !wp.getLastWrittenValue(value) );
    wp.write(10);
    CPPUNIT_ASSERT( rp.read(value) );

    wp.disconnect(rp);
    CPPUNIT_ASSERT( !wp.connected() );
    CPPUNIT_ASSERT( !rp.connected() );

    value = 0;
    CPPUNIT_ASSERT( wp.getLastWrittenValue(value) );
    CPPUNIT_ASSERT_EQUAL( 10, value );
    CPPUNIT_ASSERT_EQUAL( 10, wp.getLastWrittenValue() );

    value = 0;
    wp.createConnection(rp);
    CPPUNIT_ASSERT( rp.read(value) );
    CPPUNIT_ASSERT_EQUAL( 10, value );
}

void Generic_TaskTest_3::testPortSimpleConnections()
{
    WritePort<int> wp("WriterName");
    ReadPort<int> rp("ReaderName", ConnPolicy::data());

    CPPUNIT_ASSERT( !wp.connected() );
    CPPUNIT_ASSERT( !rp.connected() );
    {
        int value;
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.write(value); // just checking if is works or if it crashes
    }

    wp.createConnection(rp);
    CPPUNIT_ASSERT( wp.connected() );
    CPPUNIT_ASSERT( rp.connected() );

    { 
        int value = 0;
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.write(1);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT( 1 == value );
    }

    rp.clear();
    { 
        int value = 0;
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.write(1);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT( 1 == value );
    }

    // Try disconnecting starting at the writer. Disconnecting from the reader
    // will be done later
    wp.disconnect();
    CPPUNIT_ASSERT( !wp.connected() );
    CPPUNIT_ASSERT( !rp.connected() );
    {
        int value;
        wp.write(value);
        CPPUNIT_ASSERT( !rp.read(value) );
    }
    wp.disconnect(); // calling it when not connected should be fine as well

    { 
        int value = 0;
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.createBufferConnection(rp, 4);
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.write(1);
        wp.write(2);
        wp.write(3);
        wp.write(4);
        wp.write(5);
        CPPUNIT_ASSERT_EQUAL(0, value);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT_EQUAL(1, value);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT_EQUAL(2, value);

        rp.clear();
        CPPUNIT_ASSERT( !rp.read(value) );
        wp.write(10);
        wp.write(20);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT_EQUAL(10, value);
        CPPUNIT_ASSERT( rp.read(value) );
        CPPUNIT_ASSERT_EQUAL(20, value);
        CPPUNIT_ASSERT( !rp.read(value) );
    }

    // Try disconnecting from the reader this time
    rp.disconnect();
    CPPUNIT_ASSERT( !wp.connected() );
    CPPUNIT_ASSERT( !rp.connected() );
    {
        int value;
        wp.write(value);
        CPPUNIT_ASSERT( !rp.read(value) );
    }
    rp.disconnect(); // calling it when not connected should be fine as well

    // Test automatic disconnection because of port destruction
    {
        ReadPort<int> rp("ReaderName", ConnPolicy::data());
        CPPUNIT_ASSERT(wp.createConnection(rp));
        CPPUNIT_ASSERT( wp.connected() );
        CPPUNIT_ASSERT( rp.connected() );
    }
    CPPUNIT_ASSERT( !wp.connected() );
}

void Generic_TaskTest_3::testPortForkedConnections()
{
    WritePort<int> wp("W");
    ReadPort<int> rp1("R1", ConnPolicy::data());
    ReadPort<int> rp2("R2", ConnPolicy::buffer(4));

    wp.createConnection(rp1);
    CPPUNIT_ASSERT( wp.connected() );
    CPPUNIT_ASSERT( rp1.connected() );
    CPPUNIT_ASSERT( !rp2.connected() );
    wp.createConnection(rp2);
    CPPUNIT_ASSERT( wp.connected() );
    CPPUNIT_ASSERT( rp1.connected() );
    CPPUNIT_ASSERT( rp2.connected() );

    wp.write(10);
    wp.write(15);
    wp.write(20);
    wp.write(25);
    wp.write(30);

    int value = 0;
    CPPUNIT_ASSERT( rp1.read(value));
    CPPUNIT_ASSERT_EQUAL(30, value);

    CPPUNIT_ASSERT( rp2.read(value));
    CPPUNIT_ASSERT_EQUAL(10, value);
    CPPUNIT_ASSERT( rp2.read(value));
    CPPUNIT_ASSERT_EQUAL(15, value);
    CPPUNIT_ASSERT( rp2.read(value));
    CPPUNIT_ASSERT_EQUAL(20, value);
    CPPUNIT_ASSERT( rp2.read(value));
    CPPUNIT_ASSERT_EQUAL(25, value);
    CPPUNIT_ASSERT( !rp2.read(value));

    // Now removes only the buffer port
    wp.disconnect(rp2);
    CPPUNIT_ASSERT( wp.connected() );
    CPPUNIT_ASSERT( rp1.connected() );
    CPPUNIT_ASSERT( !rp2.connected() );

    wp.write(10);
    CPPUNIT_ASSERT( rp1.read(value));
    CPPUNIT_ASSERT_EQUAL(10, value);

    // And finally the other port as well
    wp.disconnect(rp1);
    CPPUNIT_ASSERT( !wp.connected() );
    CPPUNIT_ASSERT( !rp1.connected() );
    CPPUNIT_ASSERT( !rp2.connected() );
}

void Generic_TaskTest_3::testPortObjects()
{
    WritePort<double> wp1("Write");
    ReadPort<double>  rp1("Read");

    tc->ports()->addPort( &wp1 );
    tc->ports()->addPort( &rp1 );

    // Check if ports were added as objects as well
    CPPUNIT_ASSERT( tc->getObject("Write") != 0 );
    CPPUNIT_ASSERT( tc->getObject("Read") != 0 );

    // Set initial value
    wp1.write( 1.0 );

    // Connect ports.
    wp1.createConnection( rp1 );

    // Test Methods set/get
    Method<void(double)> mset;
    Method<bool(double&)> mget;

    mset = tc->getObject("Write")->methods()->getMethod<void(double)>("write");
    CPPUNIT_ASSERT( mset.ready() );

    mget = tc->getObject("Read")->methods()->getMethod<bool(double&)>("read");
    CPPUNIT_ASSERT( mget.ready() );

    mset( 3.991 );

    double get_value;
    CPPUNIT_ASSERT( mget(get_value) );
    CPPUNIT_ASSERT_EQUAL( 3.991, get_value );

    //// Finally, check cleanup. Ports and port objects must be gone:
    tc->ports()->removePort("Reader");
    CPPUNIT_ASSERT( tc->getObject("Reader") == 0 );
    CPPUNIT_ASSERT( tc->ports()->getPort("Reader") == 0 );

    tc->ports()->removePort("Writer");
    CPPUNIT_ASSERT( tc->getObject("Writer") == 0 );
    CPPUNIT_ASSERT( tc->ports()->getPort("Writer") == 0 );
}

