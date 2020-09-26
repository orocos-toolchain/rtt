/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  corba_mqueue_test.cpp

                        corba_mqueue_test.cpp -  description
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

using namespace std;
using corba::TaskContextProxy;

void
CorbaMQueueTest::setUp()
{
    // connect DataPorts
    mr1 = new InputPort<double>("mr");
    mw1 = new OutputPort<double>("mw");

    mr2 = new InputPort<double>("mr");
    mw2 = new OutputPort<double>("mw");

    tc =  new TaskContext( "localroot" );
    tc->ports()->addEventPort( *mr1, boost::bind(&CorbaMQueueTest::new_data_listener, this, _1) );
    tc->ports()->addPort( *mw1 );

    t2 = new TaskContext("localother");
    t2->ports()->addEventPort( *mr2,boost::bind(&CorbaMQueueTest::new_data_listener, this, _1) );
    t2->ports()->addPort( *mw2 );

    ts2 = ts = 0;
    tp2 = tp = 0;
}


void
CorbaMQueueTest::tearDown()
{
    delete tp;
    delete ts;
    delete tp2;
    delete ts2;
    delete tc;
    delete t2;

    delete mr1;
    delete mw1;
    delete mr2;
    delete mw2;
}

void CorbaMQueueTest::new_data_listener(base::PortInterface* port)
{
    signalled_port = port;
}


#define ASSERT_PORT_SIGNALLING(code, read_port) do { \
    signalled_port = 0; \
    code; \
    usleep(100000); \
    BOOST_CHECK( read_port == signalled_port ); \
} while(0)

void CorbaMQueueTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mw1 => mr2
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( !mr2->read(value) );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
}

void CorbaMQueueTest::testPortBufferConnection()
{
    // This test assumes that there is a buffer connection mw1 => mr2 of size 3
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( !mr2->read(value) );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(3.0), mr2);
    // it will be emptied too fast by mqueue.
    //ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK_EQUAL( mr2->read(value), OldData );
}

void CorbaMQueueTest::testPortDisconnected()
{
    BOOST_CHECK( !mw1->connected() );
    BOOST_CHECK( !mr2->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaMQueueTestSuite,  CorbaMQueueTest )


BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the differen port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    ts2 = corba::TaskContextServer::Create( t2, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA( RTT::ConnPolicy() );
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.data_size = 0;

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = ts2->server()->ports();

    // must be running to catch event port signalling.
    BOOST_CHECK( t2->start() );
    // WARNING: in the following, there is four configuration tested. There is
    // also three different ways to disconnect. We need to test those three
    // "disconnection methods", so beware when you change something ...

    policy.type = RTT::corba::CData;
    policy.pull = false;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    usleep(100000); // gives dispatcher time to catch up.
    testPortDataConnection();
    ports->disconnectPort("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CData;
    policy.pull = true;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    usleep(100000);
    testPortDataConnection();
    ports2->disconnectPort("mr");
    testPortDisconnected();

#if 1

    policy.type = RTT::corba::CBuffer;
    policy.pull = false;
    policy.size = 3;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = true;
    policy.size = 3;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mw");
    testPortDisconnected();
#endif
}

BOOST_AUTO_TEST_SUITE_END()

