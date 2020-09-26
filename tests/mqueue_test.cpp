/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  mqueue_test.cpp

                        mqueue_test.cpp -  description
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

#include <iostream>

#include "rtt/Service.hpp"
#include "rtt/transports/mqueue/MQLib.hpp"
#include "rtt/transports/mqueue/MQChannelElement.hpp"
#include "rtt/transports/mqueue/MQTemplateProtocol.hpp"
#include "rtt/os/fosi.h"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

#include "rtt/InputPort.hpp"
#include "rtt/OutputPort.hpp"
#include "rtt/TaskContext.hpp"
#include <string>

using namespace RTT;
using namespace RTT::detail;

class MQueueTest
{
public:
    MQueueTest()
    {
        // connect DataPorts
        mr1 = new InputPort<double>("mr");
        mw1 = new OutputPort<double>("mw");

        mr2 = new InputPort<double>("mr");
        mw2 = new OutputPort<double>("mw");

        // both tc's are non periodic
        tc =  new TaskContext( "root" );
        tc->ports()->addEventPort( *mr1 );
        tc->ports()->addPort( *mw1 );

        t2 = new TaskContext("other");
        t2->ports()->addEventPort( *mr2, boost::bind(&MQueueTest::new_data_listener, this, _1) );
        t2->ports()->addPort( *mw2 );

        tc->start();
        t2->start();
    }

    ~MQueueTest()
    {
        delete tc;
        delete t2;

        delete mr1;
        delete mw1;
        delete mr2;
        delete mw2;
    }

    TaskContext* tc;
    TaskContext* t2;

    PortInterface* signalled_port;
    void new_data_listener(PortInterface* port)
    {
        signalled_port = port;
    }

    // Ports
    InputPort<double>*  mr1;
    OutputPort<double>* mw1;
    InputPort<double>*  mr2;
    OutputPort<double>* mw2;

    ConnPolicy policy;

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

class MQueueFixture : public MQueueTest
{
public:
    MQueueFixture() {
        // Create a default policy specification
        policy.type = ConnPolicy::DATA;
        policy.init = false;
        policy.lock_policy = ConnPolicy::LOCK_FREE;
        policy.size = 0;
        policy.pull = true;
        policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    }
};

#define ASSERT_PORT_SIGNALLING(code, read_port) do { \
    signalled_port = 0; \
    code; \
    rtos_disable_rt_warning(); \
    usleep(100000); \
    rtos_enable_rt_warning(); \
    BOOST_CHECK( read_port == signalled_port ); \
} while(0)

void MQueueTest::testPortDataConnection()
{
    rtos_enable_rt_warning();
    // This test assumes that there is a data connection mw1 => mr2
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( NoData == mr2->read(value) );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( OldData == mr2->read(value) );

    rtos_disable_rt_warning();
}

void MQueueTest::testPortBufferConnection()
{
    rtos_enable_rt_warning();
    // This test assumes that there is a buffer connection mw1 => mr2 of size 3
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( NoData == mr2->read(value) );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(3.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);  // because size == 3
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK( OldData == mr2->read(value) );

    rtos_disable_rt_warning();
}

void MQueueTest::testPortDisconnected()
{
    BOOST_CHECK( !mw1->connected() );
    BOOST_CHECK( !mr2->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MQueueTestSuite,  MQueueFixture )

/**
 * This unit test checks a manual setup of mqueue data flow,
 * without any use of CORBA to mediate the connection.
 */
BOOST_AUTO_TEST_CASE( testPortConnections )
{
#if 1
    // WARNING: in the following, there is four configuration tested.
    // We need to manually disconnect both sides since mqueue are connection-less.
    policy.type = ConnPolicy::DATA;
    policy.pull = true;
    // test user supplied connection.
    policy.name_id = "/data1";
    BOOST_REQUIRE( mw1->createConnection(*mr2, policy) );
    BOOST_CHECK( policy.name_id == "/data1" );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();

    policy.type = ConnPolicy::DATA;
    policy.pull = true;
    policy.name_id = "";
    BOOST_REQUIRE( mw1->createConnection(*mr2, policy) );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
#if 1
    policy.type = ConnPolicy::BUFFER;
    policy.pull = false;
    policy.size = 3;
    policy.name_id = "";
    //policy.name_id = "buffer1";
    BOOST_REQUIRE( mw1->createConnection(*mr2, policy) );
    testPortBufferConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
#if 1
    policy.type = ConnPolicy::BUFFER;
    policy.pull = true;
    policy.size = 3;
    policy.name_id = "";
    //policy.name_id = "buffer2";
    BOOST_REQUIRE( mw1->createConnection(*mr2, policy) );
    testPortBufferConnection();
    //while(1) sleep(1);
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
    }

BOOST_AUTO_TEST_CASE( testPortStreams )
{
    // Test all four configurations of Data/Buffer & push/pull
    policy.type = ConnPolicy::DATA;
    policy.pull = false;
    policy.name_id = "/data1";
    BOOST_REQUIRE( mw1->createStream( policy ) );
    BOOST_REQUIRE( mr2->createStream( policy ) );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();

    policy.type = ConnPolicy::DATA;
    policy.pull = true;
    policy.name_id = "";
    BOOST_REQUIRE( mw1->createStream( policy ) );
    BOOST_REQUIRE( mr2->createStream( policy ) );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();

    policy.type = ConnPolicy::BUFFER;
    policy.pull = false;
    policy.size = 3;
    policy.name_id = "/buffer1";
    BOOST_REQUIRE( mw1->createStream( policy ) );
    BOOST_REQUIRE( mr2->createStream( policy ) );
    testPortBufferConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();

    policy.type = ConnPolicy::BUFFER;
    policy.pull = true;
    policy.size = 3;
    policy.name_id = "";
    BOOST_REQUIRE( mw1->createStream( policy ) );
    BOOST_REQUIRE( mr2->createStream( policy ) );
    testPortBufferConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
}

BOOST_AUTO_TEST_CASE( testPortStreamsTimeout )
{
    // Test creating an input stream without an output stream available.
    policy.type = ConnPolicy::DATA;
    policy.pull = false;
    policy.name_id = "/data1";
    BOOST_REQUIRE( mr2->createStream( policy ) == false );
    BOOST_CHECK( mr2->connected() == false );
    mr2->disconnect();

    policy.type = ConnPolicy::BUFFER;
    policy.pull = false;
    policy.size = 10;
    policy.name_id = "/buffer1";
    BOOST_REQUIRE( mr2->createStream( policy ) == false );
    BOOST_CHECK( mr2->connected() == false );
    mr2->disconnect();
}


BOOST_AUTO_TEST_CASE( testPortStreamsWrongName )
{
    // Test creating an input/output stream with a wrong name
    policy.type = ConnPolicy::DATA;
    policy.pull = false;
    policy.name_id = "data1"; // name must start with '/'
    BOOST_REQUIRE( mr2->createStream( policy ) == false );
    BOOST_CHECK( mr2->connected() == false );
    mr2->disconnect();

    policy.type = ConnPolicy::BUFFER;
    policy.pull = false;
    policy.size = 10;
    policy.name_id = "buffer1";
    BOOST_REQUIRE( mw2->createStream( policy ) == false );
    BOOST_CHECK( mw2->connected() == false );
    mw2->disconnect();
}

// copied from testPortStreams
BOOST_AUTO_TEST_CASE( testVectorTransport )
{
    DataFlowInterface* ports  = tc->ports();
    DataFlowInterface* ports2 = t2->ports();

    std::vector<double> data(20, 3.33);
    InputPort< std::vector<double> > vin("VIn");
    OutputPort< std::vector<double> > vout("Vout");
    ports->addPort(vin).doc("input port");
    ports2->addPort(vout).doc("output port");

    // init the output port with a vector of size 20, values 3.33
    vout.setDataSample( data );
    data = vout.getLastWrittenValue();
    for(int i=0; i != 20; ++i)
        BOOST_CHECK_CLOSE( data[i], 3.33, 0.01);

    policy.type = ConnPolicy::DATA;
    policy.pull = false;
    policy.name_id = "/vdata1";
    BOOST_REQUIRE( vout.createStream( policy ) );
    BOOST_REQUIRE( vin.createStream( policy ) );

    // check that the receiver did not get any data
    BOOST_CHECK_EQUAL( vin.read(data), NoData);

    // prepare a new data sample, size 10, values 6.66
    data.clear();
    data.resize(10, 6.66);
    for(unsigned int i=0; i != data.size(); ++i)
        BOOST_CHECK_CLOSE( data[i], 6.66, 0.01);

    rtos_enable_rt_warning();
    vout.write( data );
    rtos_disable_rt_warning();

    // prepare data buffer for reception:
    data.clear();
    data.resize(20, 0.0);
    usleep(200000);

    rtos_enable_rt_warning();
    BOOST_CHECK_EQUAL( vin.read(data), NewData);
    rtos_disable_rt_warning();

    // check if both size and capacity and values are as expected.
    BOOST_CHECK_EQUAL( data.size(), 10);
    BOOST_CHECK_EQUAL( data.capacity(), 20);
    for(unsigned int i=0; i != data.size(); ++i)
        BOOST_CHECK_CLOSE( data[i], 6.66, 0.01);

    rtos_enable_rt_warning();
    BOOST_CHECK_EQUAL( vin.read(data), OldData);
    rtos_disable_rt_warning();
}

BOOST_AUTO_TEST_SUITE_END()

