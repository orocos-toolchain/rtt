
#include "mqueue_test.hpp"

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <interface/OperationInterface.hpp>
#include <transports/mqueue/MQLib.hpp>
#include <transports/mqueue/MQChannelElement.hpp>
#include <transports/mqueue/MQTemplateProtocol.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

void
MQueueTest::setUp()
{
    // connect DataPorts
    mr1 = new InputPort<double>("mr");
    mw1 = new OutputPort<double>("mw");

    mr2 = new InputPort<double>("mr");
    mw2 = new OutputPort<double>("mw");

    tc =  new TaskContext( "root" );
    tc->ports()->addPort( mr1 );
    tc->ports()->addPort( mw1 );

    t2 = new TaskContext("other");
    t2->ports()->addPort( mr2 );
    t2->ports()->addPort( mw2 );
}


void
MQueueTest::tearDown()
{
    delete tc;
    delete t2;

    delete mr1;
    delete mw1;
    delete mr2;
    delete mw2;
}

void MQueueTest::new_data_listener(PortInterface* port)
{
    signalled_port = port;
}


#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; \
    code; \
    usleep(100000); \
    BOOST_CHECK( read_port == signalled_port );

void MQueueTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mw1 => mr2
    // Check if connection succeeded both ways:
    BOOST_CHECK( mw1->connected() );
    BOOST_CHECK( mr2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK( !mr2->read(value) );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2)
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
}

void MQueueTest::testPortBufferConnection()
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
    ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);  // because size == 3
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK( !mr2->read(value) );
}

void MQueueTest::testPortDisconnected()
{
    BOOST_CHECK( !mw1->connected() );
    BOOST_CHECK( !mr2->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MQueueTestSuite,  MQueueTest )

/**
 * This unit test checks a manual setup of mqueue data flow,
 * without any use of CORBA to mediate the connection.
 */
BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // Create a default policy specification
    ConnPolicy policy;
    policy.type = ConnPolicy::DATA;
    policy.init = false;
    policy.lock_policy = ConnPolicy::LOCK_FREE;
    policy.size = 0;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mr2->getNewDataOnPortEvent()->setup(
                boost::bind(&MQueueTest::new_data_listener, this, _1) ) );
    hl.connect();

    DataFlowInterface* ports  = tc->ports();
    DataFlowInterface* ports2 = t2->ports();

#if 1
    // WARNING: in the following, there is four configuration tested.
    // We need to manually disconnect both sides since mqueue are connection-less.
    policy.type = ConnPolicy::DATA;
    policy.pull = false;
    //policy.name_id = "data1";
    BOOST_CHECK( mw1->createConnection(*mr2, policy) );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();

    policy.type = ConnPolicy::DATA;
    policy.pull = true;
    //policy.name_id = "data2";
    BOOST_CHECK( mw1->createConnection(*mr2, policy) );
    testPortDataConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
#if 1
    policy.type = ConnPolicy::BUFFER;
    policy.pull = false;
    policy.size = 3;
    //policy.name_id = "buffer1";
    BOOST_CHECK( mw1->createConnection(*mr2, policy) );
    testPortBufferConnection();
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
#if 1
    policy.type = ConnPolicy::BUFFER;
    policy.pull = true;
    policy.size = 3;
    //policy.name_id = "buffer2";
    BOOST_CHECK( mw1->createConnection(*mr2, policy) );
    testPortBufferConnection();
    //while(1) sleep(1);
    mw1->disconnect();
    mr2->disconnect();
    testPortDisconnected();
#endif
    }

#if 0
BOOST_AUTO_TEST_CASE( testPortProxying )
{
    ts  = corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp  = corba::ControlTaskProxy::Create( ts->server(), true );
    ts2  = corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2  = corba::ControlTaskProxy::Create( ts2->server(), true );

    PortInterface* untyped_port;
     
    untyped_port = tp->ports()->getPort("mr");
    BOOST_CHECK(untyped_port);
    InputPortInterface* read_port = dynamic_cast<InputPortInterface*>(tp->ports()->getPort("mr"));
    BOOST_CHECK(read_port);
     
    untyped_port = tp->ports()->getPort("mr");
    BOOST_CHECK(untyped_port);
    OutputPortInterface* write_port = dynamic_cast<OutputPortInterface*>(tp2->ports()->getPort("mw"));
    BOOST_CHECK(write_port);

    // Just make sure 'read_port' and 'write_port' are actually proxies and not
    // the real thing
    BOOST_CHECK(dynamic_cast<corba::RemoteInputPort*>(read_port));
    BOOST_CHECK(dynamic_cast<corba::RemoteOutputPort*>(write_port));

    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(read_port->getTypeInfo() == mr1->getTypeInfo());
    BOOST_CHECK(!write_port->connected());
    BOOST_CHECK(write_port->getTypeInfo() == mw2->getTypeInfo());

    mw2->createConnection(*read_port);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    read_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    mw2->createConnection(*read_port);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    write_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    // Test cloning
    auto_ptr<InputPortInterface> read_clone(dynamic_cast<InputPortInterface*>(read_port->clone()));
    BOOST_CHECK(mw2->createConnection(*read_clone));
    BOOST_CHECK(read_clone->connected());
    BOOST_CHECK(!read_port->connected());
    mw2->disconnect();
}
#endif

BOOST_AUTO_TEST_SUITE_END()

