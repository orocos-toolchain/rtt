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
#include <Ports.hpp>
#include <DataObjectInterfaces.hpp>
#include <BufferLocked.hpp>
#include <DataPort.hpp>

#include <SlaveActivity.hpp>
#include <SimulationActivity.hpp>
#include <SimulationThread.hpp>

#include <boost/function_types/function_type_signature.hpp>

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

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  Generic_TaskTest3Suite,  Generic_TaskTest_3 )


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

BOOST_AUTO_TEST_CASE( testPeriod)
{
    // check unconfigured TC
    TaskContext pertc("PerTC");
    BOOST_CHECK( pertc.getPeriod() < 0.0 );

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

BOOST_AUTO_TEST_CASE( testPorts)
{
    WriteDataPort<double> wdp("WDName");
    ReadDataPort<double> rdp("RDName");
    DataPort<double> dp("DName");
    DataPort<double> dp2("D2Name");

    BOOST_CHECK( wdp.getPortType() == PortInterface::WritePort );
    BOOST_CHECK( rdp.getPortType() == PortInterface::ReadPort );
    BOOST_CHECK( dp.getPortType() == PortInterface::ReadWritePort );

    // Test initial value
    wdp.Set( 1.0 );
    dp.Set( 2.0 );

    WriteBufferPort<double> wbp("WBName", 10);
    ReadBufferPort<double> rbp("RBName");
    BufferPort<double> bp("BName", 10);
    BufferPort<double> bp2("B2Name", 10);

    BOOST_CHECK( wbp.getPortType() == PortInterface::WritePort );
    BOOST_CHECK( rbp.getPortType() == PortInterface::ReadPort );
    BOOST_CHECK( bp.getPortType() == PortInterface::ReadWritePort );

    BOOST_CHECK( tc->ports()->addPort( &wdp ));
    BOOST_CHECK( tc->ports()->addPort( &rdp ));
    BOOST_CHECK( tc->ports()->addPort( &dp ));
    BOOST_CHECK( tc->ports()->addPort( &dp2 ));

    // check adding same port twice.
    BOOST_CHECK( tc->ports()->addPort( &wdp ) == false);
    {
        // also check adding different port with same name.
        DataPort<double> tdp("WDName");
        BOOST_CHECK( tc->ports()->addPort( &tdp ) == false);
    }

    BOOST_CHECK( tc->ports()->addPort( &wbp ));
    BOOST_CHECK( tc->ports()->addPort( &rbp ));
    BOOST_CHECK( tc->ports()->addPort( &bp ));
    BOOST_CHECK( tc->ports()->addPort( &bp2 ));

    // Test connection creation.
    BOOST_CHECK(wdp.connectTo( &rdp ) );
    BOOST_CHECK(dp.connectTo( rdp.connection() ));

    BOOST_CHECK(wbp.connectTo( &rbp ) );
    BOOST_CHECK(bp.connectTo( rbp.connection() ));

    BOOST_CHECK( wdp.connected() );
    BOOST_CHECK( rdp.connected() );
    BOOST_CHECK( dp.connected() );

    BOOST_CHECK( wbp.connected() );
    BOOST_CHECK( rbp.connected() );
    BOOST_CHECK( bp.connected() );

    // Test data transfer
    BOOST_CHECK( rdp.Get() == 1.0 );
    wdp.Set( 3.0 );
    BOOST_CHECK( rdp.Get() == 3.0 );
    BOOST_CHECK( dp.Get() == 3.0 );
    double dat = 0.0;
    dp.Get( dat );
    BOOST_CHECK( dat == 3.0 );
    dat = 0.0;
    rdp.Get( dat );
    BOOST_CHECK( dat == 3.0 );

    // Test Data-to-Data:
    dp.disconnect();
    BOOST_CHECK( dp.connectTo( &dp2 ) );
    BOOST_CHECK( dp.connected() );
    BOOST_CHECK( dp2.connected() );

    dp.Set( 5.0 );
    dp2.Get( dat );
    BOOST_CHECK( dat == 5.0 );

    dp2.Set( 6.0 );
    BOOST_CHECK( dp.Get() == 6.0 );

    dp.disconnect();
    dp2.disconnect();
#ifndef OROPKG_OS_MACOSX
    *((DataPortBase<double>*)&dp) = new DataObject<double>("Data",10.0);
    //dp = new DataObject<double>("Data",10.0); // TODO the operator= is inaccessible on MinGW 3.4.2
    BOOST_CHECK( dp.connected() );
    BOOST_CHECK( dp.Get() == 10.0 );
#endif
    // Test buffer transfer
    double val;
    BOOST_CHECK( wbp.Push( 5.0 ) );
    BOOST_CHECK( rbp.Pop( val ) );
    BOOST_CHECK( val == 5.0 );

    BOOST_CHECK( wbp.Push( 6.0 ) );
    BOOST_CHECK( bp.Pop( val ) );
    BOOST_CHECK( val == 6.0 );

    BOOST_CHECK( bp.Push( 5.0 ) );
    BOOST_CHECK( bp.Pop( val ) );
    BOOST_CHECK( val == 5.0 );
    BOOST_CHECK( bp.Pop( val ) == false );

    // Test Buffer-to-Buffer:
    bp.disconnect();
    BOOST_CHECK( bp.connectTo( &bp2 ) );
    BOOST_CHECK( bp.connected() );
    BOOST_CHECK( bp2.connected() );

    BOOST_CHECK( bp.Push( 5.0 ) );
    BOOST_CHECK( bp2.Pop( val ) );
    BOOST_CHECK( val == 5.0 );
    BOOST_CHECK( bp2.Pop( val ) == false );

    BOOST_CHECK( bp2.Push( 5.0 ) );
    BOOST_CHECK( bp.Pop( val ) );
    BOOST_CHECK( val == 5.0 );
    BOOST_CHECK( bp2.Pop( val ) == false );

    bp.disconnect();
    bp2.disconnect();
#ifndef OROPKG_OS_MACOSX
    bp = new BufferLockFree<double>(10);
    BOOST_CHECK( bp.connected() );
    BOOST_CHECK( bp.buffer()->capacity() == 10 );
#endif

}

BOOST_AUTO_TEST_CASE( testConnections)
{
    WriteDataPort<double> wdp("WDName");
    ReadDataPort<double> rdp("RDName");
    DataPort<double> dp("DName");
    DataPort<double> dp2("D2Name");

    BOOST_CHECK( wdp.getPortType() == PortInterface::WritePort );
    BOOST_CHECK( rdp.getPortType() == PortInterface::ReadPort );
    BOOST_CHECK( dp.getPortType() == PortInterface::ReadWritePort );

    // Test initial value
    wdp.Set( 1.0 );
    dp.Set( 2.0 );

    WriteBufferPort<double> wbp("WBName", 10);
    ReadBufferPort<double> rbp("RBName");
    BufferPort<double> bp("BName", 10);
    BufferPort<double> bp2("B2Name", 10);

    BOOST_CHECK( wbp.getPortType() == PortInterface::WritePort );
    BOOST_CHECK( rbp.getPortType() == PortInterface::ReadPort );
    BOOST_CHECK( bp.getPortType() == PortInterface::ReadWritePort );

    BOOST_CHECK( tc->ports()->addPort( &wdp ));
    BOOST_CHECK( tc->ports()->addPort( &rdp ));
    BOOST_CHECK( tc->ports()->addPort( &dp ));
    BOOST_CHECK( tc->ports()->addPort( &dp2 ));

    // test setting the connection object
    *((DataPortBase<double>*)&wdp) = new DataObject<double>("");
    *((DataPortBase<double>*)&rdp) = new DataObject<double>("");
    *((DataPortBase<double>*)&dp) = new DataObject<double>("");
    *((DataPortBase<double>*)&dp2) = new DataObject<double>("");
    // TODO the operator= is inaccessible on MinGW 3.4.2
    //wdp = new DataObject<double>("");
    //rdp = new DataObject<double>("");
    //dp = new DataObject<double>("");
    //dp2 = new DataObject<double>("");


    BOOST_CHECK( wdp.connected() );
    BOOST_CHECK( rdp.connected() );
    BOOST_CHECK( dp.connected() );
    BOOST_CHECK( dp2.connected() );

    BOOST_CHECK( dynamic_cast<DataObject<double>* >( wdp.connection()->getDataSource().get() ) );
    BOOST_CHECK( dynamic_cast<DataObject<double>* >( rdp.connection()->getDataSource().get() ) );
    BOOST_CHECK( dynamic_cast<DataObject<double>* >( dp.connection()->getDataSource().get() ) );
    BOOST_CHECK( dynamic_cast<DataObject<double>* >( dp2.connection()->getDataSource().get() ) );

    // test setting the connection object
    wbp = new BufferLocked<double>(10);
    rbp = new BufferLocked<double>(11);
    bp = new BufferLocked<double>(12);
    bp2 = new BufferLocked<double>(13);

    BOOST_CHECK( wbp.connected() );
    BOOST_CHECK( rbp.connected() );
    BOOST_CHECK( bp.connected() );
    BOOST_CHECK( bp2.connected() );

    BOOST_CHECK( dynamic_cast<BufferLocked<double>* >( wbp.connection()->getBuffer().get() ) );
    BOOST_CHECK( dynamic_cast<BufferLocked<double>* >( rbp.connection()->getBuffer().get() ) );
    BOOST_CHECK( dynamic_cast<BufferLocked<double>* >( bp.connection()->getBuffer().get() ) );
    BOOST_CHECK( dynamic_cast<BufferLocked<double>* >( bp2.connection()->getBuffer().get() ) );

}

BOOST_AUTO_TEST_CASE( testPortObjects)
{
    WriteDataPort<double> wdp("WDName");
    ReadDataPort<double> rdp("RDName");
    DataPort<double> dp("DName");
    DataPort<double> dp2("D2Name");

    tc->ports()->addPort( &wdp );
    tc->ports()->addPort( &rdp );
    tc->ports()->addPort( &dp );
    tc->ports()->addPort( &dp2 );

    // Check if ports were added as objects as well
    BOOST_CHECK( tc->getObject("WDName") != 0 );
    BOOST_CHECK( tc->getObject("RDName") != 0 );
    BOOST_CHECK( tc->getObject("DName") != 0 );
    BOOST_CHECK( tc->getObject("D2Name") != 0 );

    // Set initial value
    wdp.Set( 1.0 );
    dp.Set( 2.0 );

    // Connect ports.
    wdp.connectTo( &rdp );
    dp.connectTo( &dp2 );

    WriteBufferPort<double> wbp("WBName", 10);
    ReadBufferPort<double> rbp("RBName");
    BufferPort<double> bp("BName", 10);
    BufferPort<double> bp2("B2Name", 10);

    tc->ports()->addPort( &wbp );
    tc->ports()->addPort( &rbp );
    tc->ports()->addPort( &bp );
    tc->ports()->addPort( &bp2 );

    // Check if ports were added as objects as well
    BOOST_CHECK( tc->getObject("WBName") != 0 );
    BOOST_CHECK( tc->getObject("RBName") != 0 );
    BOOST_CHECK( tc->getObject("BName") != 0 );
    BOOST_CHECK( tc->getObject("B2Name") != 0 );

    // Connect ports.
    wbp.connectTo( &rbp );
    bp.connectTo( &bp2 );

    // Test Methods set/get
    Method<void(const double&)> mset;
    Method<double(void)> mget;

    mset = tc->getObject("WDName")->methods()->getMethod<void(const double&)>("Set");
    BOOST_CHECK( mset.ready() );

    mget = tc->getObject("RDName")->methods()->getMethod<double(void)>("Get");
    BOOST_CHECK( mget.ready() );

    mset( 3.991 );

    BOOST_CHECK( mget() == 3.991 );

    // Test Methods for DataPort
    mset = tc->getObject("DName")->methods()->getMethod<void(const double&)>("Set");
    BOOST_CHECK( mset.ready() );

    mget = tc->getObject("D2Name")->methods()->getMethod<double(void)>("Get");
    BOOST_CHECK( mget.ready() );

    mset( 3.992 );

    BOOST_CHECK( mget() == 3.992 );

    // Test Methods push/pull
    Method<bool(const double&)> mpush;
    Method<bool(double&)> mpop;

    mpush = tc->getObject("WBName")->methods()->getMethod<bool(const double&)>("Push");
    BOOST_CHECK( mpush.ready() );

    mpop = tc->getObject("RBName")->methods()->getMethod<bool(double&)>("Pop");
    BOOST_CHECK( mpop.ready() );

    BOOST_CHECK( mpush( 3.991 ) );

    double ret;
    BOOST_CHECK( mpop(ret) );

    BOOST_CHECK( ret == 3.991 );

    // Test Methods push/pop for DataPort
    mpush = tc->getObject("BName")->methods()->getMethod<bool(const double&)>("Push");
    BOOST_CHECK( mpush.ready() );

    mpop = tc->getObject("B2Name")->methods()->getMethod<bool(double&)>("Pop");
    BOOST_CHECK( mpop.ready() );

    BOOST_CHECK( mpush( 3.993 ) );

    BOOST_CHECK( mpop(ret) );

    BOOST_CHECK( ret == 3.993 );

    // Finally, check cleanup. Ports and port objects must be gone:
    tc->ports()->removePort("RDName");
    BOOST_CHECK( tc->getObject("RDName") == 0 );
    BOOST_CHECK( tc->ports()->getPort("RDName") == 0 );

    tc->ports()->removePort("BName");
    BOOST_CHECK( tc->getObject("BName") == 0 );
    BOOST_CHECK( tc->ports()->getPort("BName") == 0 );
}

BOOST_AUTO_TEST_SUITE_END()
