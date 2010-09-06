
#include "corba_mqueue_test.hpp"

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <transports/corba/DataFlowI.h>
#include <rtt/transports/corba/RemotePorts.hpp>
#include <rtt/transports/mqueue/MQLib.hpp>
#include <rtt/transports/corba/CorbaConnPolicy.hpp>
#include <transports/corba/corba.h>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/TaskContext.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
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

    void setupCorba();
    void cleanupCorba();

    void setUp();
    void tearDown();

    void testPortConnections();
    void testPortProxying();

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
    tc->ports()->addPort( *mr1 );
    tc->ports()->addPort( *mw1 );

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


#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; \
    code; \
    usleep(100000); \
    BOOST_CHECK( read_port == signalled_port );

bool wait_for_helper;
#define wait_for( cond, times ) \
    wait = 0; \
    while( (wait_for_helper = !(cond)) && wait++ != times ) \
      usleep(100000); \
    if (wait_for_helper) BOOST_CHECK( cond );

#define wait_for_equal( a, b, times ) \
    wait = 0; \
    while( (wait_for_helper = ((a) != (b))) && wait++ != times ) \
      usleep(100000); \
    if (wait_for_helper) BOOST_CHECK_EQUAL( a, b );

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
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr1)
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


BOOST_AUTO_TEST_CASE( setupServer )
{
    system("./corba-mqueue-ipc-server &");
    usleep(500000);
}

BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the different port-to-port connections.
    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    //tp = corba::TaskContextProxy::Create("other");
    //if (!tp )
    tp = corba::TaskContextProxy::CreateFromFile( "other.ior");

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA( RTT::ConnPolicy() );
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.data_size = 0;

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mr1->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaMQueueIPCTest::new_data_listener, this, _1) ) );
    hl.connect();

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
    usleep(100000); // gives dispatcher time to catch up.
    testPortDataConnection();
    ports->disconnectPort("mw");
    ports2->disconnectPort("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CData;
    policy.pull = true;
    policy.transport = ORO_MQUEUE_PROTOCOL_ID;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    BOOST_CHECK( ports2->createConnection("mw", ports, "mr", policy) );
    usleep(100000);
    testPortDataConnection();
    ports2->disconnectPort("mr");
    ports->disconnectPort("mr");
    testPortDisconnected();

#if 1

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
#endif
}

BOOST_AUTO_TEST_CASE( cleanupServer )
{
    system("killall corba-mqueue-ipc-server");
}

BOOST_AUTO_TEST_SUITE_END()

