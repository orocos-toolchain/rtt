/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test.cpp -  description
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



#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <rtt/Method.hpp>
#include <rtt/interface/ServiceProvider.hpp>
#include <transports/corba/DataFlowI.h>
#include <rtt/transports/corba/RemotePorts.hpp>
#include <transports/corba/ServiceProviderC.h>
#include <transports/corba/corba.h>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/TaskContext.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <string>
#include <stdlib.h>

using namespace RTT;
using namespace RTT::detail;

class CorbaTest
{
public:
    CorbaTest() { this->setUp(); }
    ~CorbaTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;
    TaskContextProxy* tp;
    corba::TaskContextServer* ts;
    TaskContext* tp2;
    corba::TaskContextServer* ts2;
    CTaskContext_ptr s;
    CTaskContext_ptr s2;

    base::PortInterface* signalled_port;
    void new_data_listener(base::PortInterface* port);

    // Ports
    InputPort<double>*  mi;
    OutputPort<double>* mo;

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
CorbaTest::setUp()
{
    // connect DataPorts
    mi = new InputPort<double>("mi");
    mo = new OutputPort<double>("mo");

    tc =  new TaskContext( "root" );
    tc->ports()->addPort( *mi );
    tc->ports()->addPort( *mo );

    t2 = 0;
    ts2 = ts = 0;
    tp2 = tp = 0;
}


void
CorbaTest::tearDown()
{
    delete tp;
    delete ts;
    delete tp2;
    delete ts2;
    delete tc;
    delete t2;

    delete mi;
    delete mo;
}

void CorbaTest::new_data_listener(base::PortInterface* port)
{
    signalled_port = port;
}


#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; \
    code; \
    usleep(100000); \
    BOOST_CHECK( read_port == signalled_port );

void CorbaTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mo => mi
    // Check if connection succeeded both ways:
    BOOST_CHECK( mo->connected() );
    BOOST_CHECK( mi->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK_EQUAL( mi->read(value), NoData );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mo->write(1.0), mi)
    BOOST_CHECK( mi->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mo->write(2.0), mi);
    BOOST_CHECK( mi->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
}

void CorbaTest::testPortBufferConnection()
{
    // This test assumes that there is a buffer connection mo => mi of size 3
    // Check if connection succeeded both ways:
    BOOST_CHECK( mo->connected() );
    BOOST_CHECK( mi->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK_EQUAL( mi->read(value), NoData );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mo->write(1.0), mi);
    ASSERT_PORT_SIGNALLING(mo->write(2.0), mi);
    ASSERT_PORT_SIGNALLING(mo->write(3.0), mi);
    ASSERT_PORT_SIGNALLING(mo->write(4.0), 0);
    BOOST_CHECK( mi->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mi->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mi->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK_EQUAL( mi->read(value), OldData );
}

void CorbaTest::testPortDisconnected()
{
    BOOST_CHECK( !mo->connected() );
    BOOST_CHECK( !mi->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaIPCTestSuite,  CorbaTest )


BOOST_AUTO_TEST_CASE( setupServer )
{
    system("./corba-ipc-server &");
    usleep(100000);
}

BOOST_AUTO_TEST_CASE( testRemoteMethodC )
{
    tp = corba::TaskContextProxy::Create( "peerRMC", false ); // no-ior
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerRMC.ior");
    BOOST_REQUIRE( tp );

    // This test tests 'transparant' remote invocation of Orocos internal::MethodC objects.
    internal::MethodC mc;
    double r = 0.0;
    mc = tp->provides("methods")->create("vm0", tc->engine() );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == 0.0 );

    mc = tp->provides("methods")->create("m0", tc->engine() ).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -1.0 );

    mc = tp->provides("methods")->create("m2", tc->engine()).argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -3.0 );

    mc = tp->provides("methods")->create("m3", tc->engine()).ret( r ).argC(1).argC(1.0).argC(true);
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -4.0 );

}

BOOST_AUTO_TEST_CASE( testRemoteMethod )
{
    tp = corba::TaskContextProxy::Create( "peerRM" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerRM.ior");
    BOOST_REQUIRE(tp);
    // This test tests 'transparant' remote invocation of Orocos methods.
    // This requires the internal::RemoteMethod class, which does not work yet.
    RTT::Method<double(void)> m0 = tp->provides("methods")->getOperation("m0");
    RTT::Method<double(int)> m1 = tp->provides("methods")->getOperation("m1");
    RTT::Method<double(int,double)> m2 = tp->provides("methods")->getOperation("m2");
    RTT::Method<double(int,double,bool)> m3 = tp->provides("methods")->getOperation("m3");
    RTT::Method<double(int,double,bool,std::string)> m4 = tp->provides("methods")->getOperation("m4");

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

BOOST_AUTO_TEST_CASE( testAnyMethod )
{
    tp = corba::TaskContextProxy::Create( "peerAM" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerAM.ior");

    BOOST_REQUIRE(tp);
    s = tp->server();
    BOOST_REQUIRE( s );

    // This test tests the createMethodAny() function of the server.
    corba::CServiceProvider_var co = s->provides("methods");
    BOOST_REQUIRE( co.in() );

    corba::CAnyArguments_var any_args = new corba::CAnyArguments(0);
    corba::CMethod_var vm0 = co->createMethodAny("vm0", any_args.in());
    BOOST_REQUIRE( vm0.in() );

    BOOST_CHECK( vm0->callOperation( any_args.in() ) );

    corba::CMethod_var m0 = co->createMethodAny("m0", any_args.in());
    BOOST_REQUIRE( m0.in() );

    BOOST_CHECK( m0->callOperation( any_args.in() ) );

    any_args = new corba::CAnyArguments(1);
    any_args->length(1);
    unsigned int index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    corba::CMethod_var m1;
    BOOST_CHECK_NO_THROW( m1 = co->createMethodAny("m1", any_args.in()));
    BOOST_CHECK( m1.in() );

    BOOST_CHECK(m1->callOperation( any_args.in() ));

    any_args = new corba::CAnyArguments(2);
    any_args->length(2);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    corba::CMethod_var m2;
    BOOST_CHECK_NO_THROW( m2 = co->createMethodAny("m2", any_args.in()));
    BOOST_CHECK( m2.in() );

    BOOST_CHECK(m2->callOperation( any_args.in() ));

    any_args = new corba::CAnyArguments(3);
    any_args->length(3);
     index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    corba::CMethod_var m3;
    BOOST_CHECK_NO_THROW( m3= co->createMethodAny("m3", any_args.in()) );
    BOOST_CHECK( m3.in() );

    BOOST_CHECK(m3->callOperation( any_args.in() ));

    any_args = new corba::CAnyArguments(4);
    any_args->length(4);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    ++index;
    any_args[index] <<= "hello";
    corba::CMethod_var m4;
    BOOST_CHECK_NO_THROW ( m4 = co->createMethodAny("m4", any_args.in()) );
    BOOST_CHECK( m4.in() );

    BOOST_CHECK(m4->callOperation( any_args.in() ));
}

BOOST_AUTO_TEST_CASE(testDataFlowInterface)
{
    tp = corba::TaskContextProxy::Create( "peerDFI" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerDFI.ior");

    corba::CDataFlowInterface_var ports = tp->server()->ports();

    corba::CDataFlowInterface::CPortNames_var names =
	ports->getPorts();

    BOOST_CHECK_EQUAL(CORBA::ULong(2), names->length());
    BOOST_CHECK_EQUAL(string("mi"), string(names[CORBA::ULong(0)]));
    BOOST_CHECK_EQUAL(string("mo"), string(names[CORBA::ULong(1)]));

    // Now check directions
    BOOST_CHECK_EQUAL(RTT::corba::COutput,
	    ports->getPortType("mo"));
    BOOST_CHECK_EQUAL(RTT::corba::CInput,
	    ports->getPortType("mi"));

    // And check type names
    BOOST_CHECK_EQUAL(string("double"),
	    string(ports->getDataType("mo")));
}

BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerPC" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerPC.ior");

    s = tp->server();
    // server to our own tc.
    ts2  = corba::TaskContextServer::Create( tc, false ); //no-naming
    s2 = ts2->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    hl.connect();

    corba::CDataFlowInterface_var ports  = s->ports();
    corba::CDataFlowInterface_var ports2 = s2->ports();

    // Test cases that should not connect
    BOOST_CHECK_THROW( !ports->createConnection("mo", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( !ports->createConnection("does_not_exist", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( !ports->createConnection("does_not_exist", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( !ports->createConnection("mo", ports2, "mo", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( !ports->createConnection("mi", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( !ports->createConnection("mi", ports2, "mo", policy), CNoSuchPortException );

    // WARNING: in the following, there is four configuration tested. There is
    // also three different ways to disconnect. We need to test those three
    // "disconnection methods", so beware when you change something ...

    // All these tests require the server app to round-trip the data to us.

    policy.type = RTT::corba::CData;
    policy.pull = false;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports, "mi", policy) );
    testPortDataConnection();
    ports->disconnectPort("mo");
    ports->disconnectPort("mi");
    testPortDisconnected();

    return;

    policy.type = RTT::corba::CData;
    policy.pull = true;
    BOOST_CHECK( ports2->createConnection("mo", ports, "mi", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports, "mi", policy) );
    testPortDataConnection();
    ports2->disconnectPort("mi");
    ports2->disconnectPort("mo");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = false;
    policy.size = 3;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports, "mi", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mo");
    ports->disconnectPort("mi");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports, "mi", policy) );
    testPortBufferConnection();
    ports2->disconnectPort("mo");
    ports2->disconnectPort("mi");
    testPortDisconnected();

#if 0
    // Here, check removal of specific connections. So first add another
    // connection ...
    mo->createConnection(*mi);
    // Remove the remote connection
    ports->removeConnection("mo", ports2, "mi");
    ports->removeConnection("mi", ports2, "mo");
    // Check it is removed
    BOOST_CHECK(mo->connected());
    BOOST_CHECK(mi->connected());
    BOOST_CHECK(!mi->connected());
#endif
}

BOOST_AUTO_TEST_CASE( testPortProxying )
{
    // This test creates connections between local and remote ports.
    tp = corba::TaskContextProxy::Create( "peerPP" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerPP.ior");

    base::PortInterface* untyped_port;

    untyped_port = tp->ports()->getPort("mi");
    BOOST_CHECK(untyped_port);
    base::InputPortInterface* read_port = dynamic_cast<base::InputPortInterface*>(tp->ports()->getPort("mi"));
    BOOST_CHECK(read_port);

    untyped_port = tp->ports()->getPort("mo");
    BOOST_CHECK(untyped_port);
    base::OutputPortInterface* write_port = dynamic_cast<base::OutputPortInterface*>(tp->ports()->getPort("mo"));
    BOOST_CHECK(write_port);

    // Just make sure 'read_port' and 'write_port' are actually proxies and not
    // the real thing
    BOOST_CHECK(dynamic_cast<corba::RemoteInputPort*>(read_port));
    BOOST_CHECK(dynamic_cast<corba::RemoteOutputPort*>(write_port));

    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(read_port->getTypeInfo() == mi->getTypeInfo());
    BOOST_CHECK(!write_port->connected());
    BOOST_CHECK(write_port->getTypeInfo() == mo->getTypeInfo());

    mo->createConnection(*read_port);
    write_port->createConnection(*mi);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    // XXX This currently does not work:
    //read_port->disconnect(*mo);
    //write_port->disconnect(*mi);
    read_port->disconnect();
    write_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    mo->createConnection(*read_port);
    write_port->createConnection(*mi);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    write_port->disconnect();
    read_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    // Test cloning
    auto_ptr<base::InputPortInterface> read_clone(dynamic_cast<base::InputPortInterface*>(read_port->clone()));
    BOOST_CHECK(mo->createConnection(*read_clone));
    BOOST_CHECK(read_clone->connected());
    BOOST_CHECK(!read_port->connected());
    mo->disconnect();
}

BOOST_AUTO_TEST_CASE( testDataHalfs )
{
    double result;
    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerDH" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerDH.ior");

    s = tp->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    BOOST_CHECK( hl.connect() );

    corba::CDataFlowInterface_var ports  = s->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    mo->connectTo( tp->ports()->getPort("mi"), toRTT(policy)  );
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample;
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    mo->write( 3.33 );
    usleep(100000);
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();
    mo->disconnect();

    // test unbuffered Corba write --> C++ read
    cce = ports->buildChannelOutput("mi", policy);
    mi->connectTo( tp->ports()->getPort("mo"), toRTT(policy)  );
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 4.44;
    cce->write( sample.in() );
    usleep(100000);
    BOOST_CHECK_EQUAL( mi->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}

BOOST_AUTO_TEST_CASE( testBufferHalfs )
{
    double result;

    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerBH" , false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerBH.ior");

    s = tp->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CBuffer;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 10;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    BOOST_CHECK( hl.connect() );

    corba::CDataFlowInterface_var ports  = s->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    mo->connectTo( tp->ports()->getPort("mi"), toRTT(policy) );
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample;
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    mo->write( 6.33 );
    mo->write( 3.33 );
    usleep(100000);
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 6.33);
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();
    mo->disconnect();

    // test unbuffered Corba write --> C++ read
    mi->connectTo( tp->ports()->getPort("mo"), toRTT(policy)  );
    cce = ports->buildChannelOutput("mi", policy);
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 6.44;
    cce->write( sample.in() );
    sample <<= 4.44;
    cce->write( sample.in() );
    usleep(100000);
    BOOST_CHECK_EQUAL( mi->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 6.44 );
    BOOST_CHECK_EQUAL( mi->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}


BOOST_AUTO_TEST_CASE( cleanupServer )
{
    system("kill -9 $(pidof corba-ipc-server)");
}

BOOST_AUTO_TEST_SUITE_END()

