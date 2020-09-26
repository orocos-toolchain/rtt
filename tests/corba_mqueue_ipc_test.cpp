/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  corba_mqueue_ipc_test.cpp

                        corba_mqueue_ipc_test.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tests/unit.hpp"

#include "tests/corba_mqueue_test.hpp"

#include <iostream>

#include "rtt/transports/corba/DataFlowI.h"
#include "rtt/transports/corba/RemotePorts.hpp"
#include "rtt/transports/mqueue/MQLib.hpp"
#include "rtt/transports/corba/CorbaConnPolicy.hpp"
#include "rtt/transports/corba/corba.h"
#include "rtt/InputPort.hpp"
#include "rtt/OutputPort.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/transports/corba/TaskContextServer.hpp"
#include "rtt/transports/corba/TaskContextProxy.hpp"
#include <string>
#include <cstdlib>

using namespace RTT;
using namespace RTT::detail;

class CorbaMQueueIPCTest
{
public:
    CorbaMQueueIPCTest() { this->setUp(); }
    ~CorbaMQueueIPCTest() { this->tearDown(); }

    TaskContext* tc;
    corba::TaskContextProxy* tp, *tp2;
    corba::TaskContextServer* ts, *ts2;

    base::PortInterface* signalled_port;
    void new_data_listener(base::PortInterface* port);

    // Ports
    InputPort<double>*  mr1;
    OutputPort<double>* mw1;

    void setUp();
    void tearDown();

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

using namespace std;
using corba::TaskContextProxy;

void
CorbaMQueueIPCTest::setUp()
{
    // setup DataPorts: we write into mw1, the server roundtrips it to mr1
    mr1 = new InputPort<double>("mr");
    mw1 = new OutputPort<double>("mw");

    tc =  new TaskContext( "root" );
    tc->ports()->addEventPort( *mr1, boost::bind(&CorbaMQueueIPCTest::new_data_listener, this, _1) );
    tc->ports()->addPort( *mw1 );
    tc->start();

    ts2 = ts = 0;
    tp2 = tp = 0;
}


void
CorbaMQueueIPCTest::tearDown()
{
    delete tp;
    delete ts;
    delete tp2;
    delete tc;

    delete mr1;
    delete mw1;
}

void CorbaMQueueIPCTest::new_data_listener(base::PortInterface* port)
{
    signalled_port = port;
}

#define ASSERT_PORT_SIGNALLING(code, read_port) do { \
    signalled_port = 0; \
    int wait = 0; \
    code; \
    while (read_port != signalled_port && wait++ != 5) \
        usleep(100000); \
    BOOST_CHECK( read_port == signalled_port ); \
} while(0)

#define wait_for( cond, times ) do { \
    bool wait_for_helper; \
    int wait = 0; \
    while( (wait_for_helper = !(cond)) && wait++ != times ) \
        usleep(100000); \
    if (wait_for_helper) BOOST_CHECK( cond ); \
} while(0)

#define wait_for_equal( a, b, times ) do { \
    bool wait_for_helper; \
    int wait = 0; \
    while( (wait_for_helper = ((a) != (b))) && wait++ != times ) \
        usleep(100000); \
    if (wait_for_helper) BOOST_CHECK_EQUAL( a, b ); \
} while(0)

void CorbaMQueueIPCTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mw1 => server => mr1
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr1->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( !mr1->read(value) );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr1);
    BOOST_CHECK( mr1->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr1);
    BOOST_CHECK( mr1->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
}

void CorbaMQueueIPCTest::testPortBufferConnection()
{
    // This test assumes that there is a buffer connection mw1 => server => mr1 of size 3
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr1->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( !mr1->read(value) );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr1);
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr1);
    ASSERT_PORT_SIGNALLING(mw1->write(3.0), mr1);
    // it will be emptied too fast by mqueue.
    //ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);
    BOOST_CHECK( mr1->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mr1->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mr1->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK_EQUAL( mr1->read(value), OldData );
}

void CorbaMQueueIPCTest::testPortDisconnected()
{
    BOOST_CHECK( !mw1->connected() );
    BOOST_CHECK( !mr1->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaMQueueIPCTestSuite,  CorbaMQueueIPCTest )

BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the different port-to-port connections.
    ts = corba::TaskContextServer::Create( tc, /* use_naming = */ false );
    tp = corba::TaskContextProxy::Create( "other", /* is_ior = */ false );
    if (!tp)
        tp = corba::TaskContextProxy::CreateFromFile( "other.ior");

    BOOST_REQUIRE(tp);

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA( RTT::ConnPolicy() );
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.data_size = 0;

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = tp->server()->ports();

    // WARNING: in the following, there is four configuration tested. There is
    // also three different ways to disconnect. We need to test those three
    // "disconnection methods", so beware when you change something ...

    policy.type = RTT::corba::CData;
    policy.pull = false;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    BOOST_CHECK( ports2->createConnection("mw", ports, "mr", policy) );
    testPortDataConnection();
    ports->disconnectPort("mw");
    ports2->disconnectPort("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CData;
    policy.pull = true;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    BOOST_CHECK( ports2->createConnection("mw", ports, "mr", policy) );
    testPortDataConnection();
    ports2->disconnectPort("mr");
    ports->disconnectPort("mr");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = false;
    policy.size = 3;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    BOOST_CHECK( ports2->createConnection("mw", ports, "mr", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mw");
    ports2->disconnectPort("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = true;
    policy.size = 3;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    BOOST_CHECK( ports2->createConnection("mw", ports, "mr", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mw");
    ports2->disconnectPort("mw");
    testPortDisconnected();
}

BOOST_AUTO_TEST_SUITE_END()

