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

#include "SimulationActivity.hpp"
#include "SimulationThread.hpp"

#include <boost/function_types/function_type_signature.hpp>

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
        this->reset();
        validconfig = true;
        validstart = true;
    }

    void reset()
    {
        didconfig = false;
        didstart = false;
        didstop = false;
        didcleanup = false;
        didupdate = false;
    }

    bool configureHook() {
        CPPUNIT_ASSERT( mTaskState <= Stopped );
        didconfig = true;
        return validconfig;
    }

    bool startHook() {
        CPPUNIT_ASSERT( mTaskState == Stopped );
        didstart = true;
        return validstart;
    }

    void stopHook() {
        CPPUNIT_ASSERT( mTaskState == Running );
        didstop = true;
    }

    void cleanupHook() {
        CPPUNIT_ASSERT( mTaskState == Stopped );
        didcleanup = true;
    }

    void updateHook() {
        CPPUNIT_ASSERT( mTaskState == Running );
        didupdate = true;
    }

    bool validconfig, didconfig;
    bool validstart, didstart;
    bool didstop;
    bool didcleanup;
    bool didupdate;
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

void Generic_TaskTest_3::testTCStates()
{
    // Test the states of a Default TC, no user methods.
    CPPUNIT_ASSERT( tc->getTaskState() == TaskContext::Stopped );

    CPPUNIT_ASSERT( tc->isConfigured() == true );
    CPPUNIT_ASSERT( tc->isRunning() == false );
    CPPUNIT_ASSERT( tc->configure() == true );

    CPPUNIT_ASSERT( tc->start() == true );
    CPPUNIT_ASSERT( tc->isRunning() == true );
    CPPUNIT_ASSERT( tc->configure() == false );
    CPPUNIT_ASSERT( tc->start() == false );
    CPPUNIT_ASSERT( tc->cleanup() == false );

    CPPUNIT_ASSERT( tc->stop() == true );
    CPPUNIT_ASSERT( tc->isRunning() == false);
    CPPUNIT_ASSERT( tc->stop() == false );
    CPPUNIT_ASSERT( tc->configure() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true );

    CPPUNIT_ASSERT( tc->cleanup() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == false);
    CPPUNIT_ASSERT( tc->isRunning() == false);

    CPPUNIT_ASSERT( tc->configure() == true );
    CPPUNIT_ASSERT( tc->isConfigured() == true);
    CPPUNIT_ASSERT( tc->isRunning() == false);

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
    stc->reset();

    // Stopped state:
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    stc->reset();
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
    stc->reset();


    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == true );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    stc->reset();

    // Back to stopped
    CPPUNIT_ASSERT( stc->stop() == true );
    CPPUNIT_ASSERT( stc->didstop == true );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == true );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    stc->reset();

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
    stc->reset();


    // Stopped state:
    CPPUNIT_ASSERT( stc->cleanup() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == false);
    CPPUNIT_ASSERT( stc->isRunning() == false);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == true );
    stc->reset();
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->cleanup() == false );
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == false );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    stc->reset();


    // PreOperational state:
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true);
    CPPUNIT_ASSERT( stc->isRunning() == false);
    // test flags
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->didstart == false );
    CPPUNIT_ASSERT( stc->didupdate == false );
    CPPUNIT_ASSERT( stc->didstop == false );
    CPPUNIT_ASSERT( stc->didcleanup == false );
    stc->reset();

}

void Generic_TaskTest_3::testFailingTCStates()
{
    // Test the states of a TC failing in transitions
    stc->validconfig = false;
    stc->validstart = false;

    // PreOperational state:
    CPPUNIT_ASSERT( stc->isConfigured() == false );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->configure() == false );
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->isConfigured() == false );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    stc->reset();

    // Retry:
    stc->validconfig = true;
    CPPUNIT_ASSERT( stc->configure() == true );
    CPPUNIT_ASSERT( stc->didconfig == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    stc->reset();

    // Stopped state:
    CPPUNIT_ASSERT( stc->start() == false );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isRunning() == false );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    stc->reset();

    // Retry:
    stc->validstart = true;
    CPPUNIT_ASSERT( stc->start() == true );
    CPPUNIT_ASSERT( stc->didstart == true );
    CPPUNIT_ASSERT( stc->isConfigured() == true );
    CPPUNIT_ASSERT( stc->isRunning() == true );
    stc->reset();

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

void Generic_TaskTest_3::testPorts()
{
    WriteDataPort<double> wdp("WDName");
    ReadDataPort<double> rdp("RDName");
    DataPort<double> dp("DName");
    DataPort<double> dp2("D2Name");

    CPPUNIT_ASSERT( wdp.getPortType() == PortInterface::WritePort );
    CPPUNIT_ASSERT( rdp.getPortType() == PortInterface::ReadPort );
    CPPUNIT_ASSERT( dp.getPortType() == PortInterface::ReadWritePort );

    // Test initial value
    wdp.Set( 1.0 );
    dp.Set( 2.0 );

    WriteBufferPort<double> wbp("WBName", 10);
    ReadBufferPort<double> rbp("RBName");
    BufferPort<double> bp("BName", 10);
    BufferPort<double> bp2("B2Name", 10);

    CPPUNIT_ASSERT( wbp.getPortType() == PortInterface::WritePort );
    CPPUNIT_ASSERT( rbp.getPortType() == PortInterface::ReadPort );
    CPPUNIT_ASSERT( bp.getPortType() == PortInterface::ReadWritePort );

    CPPUNIT_ASSERT( tc->ports()->addPort( &wdp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &rdp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &dp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &dp2 ));

    // check adding same port twice.
    CPPUNIT_ASSERT( tc->ports()->addPort( &wdp ) == false);
    {
        // also check adding different port with same name.
        DataPort<double> tdp("WDName");
        CPPUNIT_ASSERT( tc->ports()->addPort( &tdp ) == false);
    }

    CPPUNIT_ASSERT( tc->ports()->addPort( &wbp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &rbp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &bp ));
    CPPUNIT_ASSERT( tc->ports()->addPort( &bp2 ));

    // Test connection creation.
    CPPUNIT_ASSERT(wdp.createConnection( &rdp )->connect());
    CPPUNIT_ASSERT(dp.connectTo( rdp.connection() ));

    CPPUNIT_ASSERT(wbp.createConnection( &rbp )->connect());
    CPPUNIT_ASSERT(bp.connectTo( rbp.connection() ));

    CPPUNIT_ASSERT( wdp.connected() );
    CPPUNIT_ASSERT( rdp.connected() );
    CPPUNIT_ASSERT( dp.connected() );

    CPPUNIT_ASSERT( wbp.connected() );
    CPPUNIT_ASSERT( rbp.connected() );
    CPPUNIT_ASSERT( bp.connected() );

    // Test data transfer
    CPPUNIT_ASSERT( rdp.Get() == 1.0 );
    wdp.Set( 3.0 );
    CPPUNIT_ASSERT( rdp.Get() == 3.0 );
    CPPUNIT_ASSERT( dp.Get() == 3.0 );
    double dat = 0.0;
    dp.Get( dat );
    CPPUNIT_ASSERT( dat == 3.0 );
    dat = 0.0;
    rdp.Get( dat );
    CPPUNIT_ASSERT( dat == 3.0 );

    // Test Data-to-Data:
    dp.disconnect();
    CPPUNIT_ASSERT( dp.createConnection( &dp2 )->connect() );
    CPPUNIT_ASSERT( dp.connected() );
    CPPUNIT_ASSERT( dp2.connected() );

    dp.Set( 5.0 );
    dp2.Get( dat );
    CPPUNIT_ASSERT( dat == 5.0 );
    
    dp2.Set( 6.0 );
    CPPUNIT_ASSERT( dp.Get() == 6.0 );

    dp.disconnect();
    dp2.disconnect();
    dp = new DataObject<double>("Data",10.0);
    CPPUNIT_ASSERT( dp.connected() );
    CPPUNIT_ASSERT( dp.Get() == 10.0 );

    // Test buffer transfer
    double val;
    CPPUNIT_ASSERT( wbp.Push( 5.0 ) );
    CPPUNIT_ASSERT( rbp.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );

    CPPUNIT_ASSERT( wbp.Push( 6.0 ) );
    CPPUNIT_ASSERT( bp.Pop( val ) );
    CPPUNIT_ASSERT( val == 6.0 );

    CPPUNIT_ASSERT( bp.Push( 5.0 ) );
    CPPUNIT_ASSERT( bp.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );
    CPPUNIT_ASSERT( bp.Pop( val ) == false );

    // Test Buffer-to-Buffer:
    bp.disconnect();
    CPPUNIT_ASSERT( bp.createConnection( &bp2 )->connect() );
    CPPUNIT_ASSERT( bp.connected() );
    CPPUNIT_ASSERT( bp2.connected() );

    CPPUNIT_ASSERT( bp.Push( 5.0 ) );
    CPPUNIT_ASSERT( bp2.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );
    CPPUNIT_ASSERT( bp2.Pop( val ) == false );
    
    CPPUNIT_ASSERT( bp2.Push( 5.0 ) );
    CPPUNIT_ASSERT( bp.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );
    CPPUNIT_ASSERT( bp2.Pop( val ) == false );

    bp.disconnect();
    bp2.disconnect();
    bp = new BufferLockFree<double>(10);
    CPPUNIT_ASSERT( bp.connected() );
    CPPUNIT_ASSERT( bp.buffer()->capacity() == 10 );

    
}

void Generic_TaskTest_3::testPortObjects()
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
    CPPUNIT_ASSERT( tc->getObject("WDName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("RDName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("DName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("D2Name") != 0 );

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
    CPPUNIT_ASSERT( tc->getObject("WBName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("RBName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("BName") != 0 );
    CPPUNIT_ASSERT( tc->getObject("B2Name") != 0 );

    // Connect ports.
    wbp.connectTo( &rbp );
    bp.connectTo( &bp2 );

    // Test Methods set/get
    Method<void(const double&)> mset;
    Method<double(void)> mget;

    mset = tc->getObject("WDName")->methods()->getMethod<void(const double&)>("Set");
    CPPUNIT_ASSERT( mset.ready() );

    mget = tc->getObject("RDName")->methods()->getMethod<double(void)>("Get");
    CPPUNIT_ASSERT( mget.ready() );

    mset( 3.991 );

    CPPUNIT_ASSERT( mget() == 3.991 );

    // Test Methods for DataPort
    mset = tc->getObject("DName")->methods()->getMethod<void(const double&)>("Set");
    CPPUNIT_ASSERT( mset.ready() );

    mget = tc->getObject("D2Name")->methods()->getMethod<double(void)>("Get");
    CPPUNIT_ASSERT( mget.ready() );

    mset( 3.992 );

    CPPUNIT_ASSERT( mget() == 3.992 );

    // Test Methods push/pull
    Method<bool(const double&)> mpush;
    Method<bool(double&)> mpop;

    mpush = tc->getObject("WBName")->methods()->getMethod<bool(const double&)>("Push");
    CPPUNIT_ASSERT( mpush.ready() );

    mpop = tc->getObject("RBName")->methods()->getMethod<bool(double&)>("Pop");
    CPPUNIT_ASSERT( mpop.ready() );

    CPPUNIT_ASSERT( mpush( 3.991 ) );

    double ret;
    CPPUNIT_ASSERT( mpop(ret) );

    CPPUNIT_ASSERT( ret == 3.991 );

    // Test Methods push/pop for DataPort
    mpush = tc->getObject("BName")->methods()->getMethod<bool(const double&)>("Push");
    CPPUNIT_ASSERT( mpush.ready() );

    mpop = tc->getObject("B2Name")->methods()->getMethod<bool(double&)>("Pop");
    CPPUNIT_ASSERT( mpop.ready() );

    CPPUNIT_ASSERT( mpush( 3.993 ) );

    CPPUNIT_ASSERT( mpop(ret) );

    CPPUNIT_ASSERT( ret == 3.993 );

    // Finally, check cleanup. Ports and port objects must be gone:
    tc->ports()->removePort("RDName");
    CPPUNIT_ASSERT( tc->getObject("RDName") == 0 );
    CPPUNIT_ASSERT( tc->ports()->getPort("RDName") == 0 );

    tc->ports()->removePort("BName");
    CPPUNIT_ASSERT( tc->getObject("BName") == 0 );
    CPPUNIT_ASSERT( tc->ports()->getPort("BName") == 0 );
}
