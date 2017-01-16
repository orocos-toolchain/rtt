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

#include "unit.hpp"

#include <iostream>
#include <memory>

#include <rtt/OperationCaller.hpp>
#include <rtt/Service.hpp>
#include <transports/corba/DataFlowI.h>
#include <rtt/transports/corba/RemotePorts.hpp>
#include <transports/corba/ServiceC.h>
#include <transports/corba/corba.h>
#include <transports/corba/CorbaConnPolicy.hpp>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/TaskContext.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <transports/corba/CorbaLib.hpp>
#include <rtt/internal/DataSourceTypeInfo.hpp>

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
    enum { INITIAL, CALL, SEND, FINAL } callBackPeer_step;
    int callBackPeer_count;
    SendHandle<void(TaskContext*, string const&)> handle;

    void setUp();
    void tearDown();

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();

    void callBackPeer(TaskContext* peer, string const& opname) {
        OperationCaller<void(TaskContext*, string const&)> op1( peer->getOperation(opname), tc->engine() );
        OperationCaller<void()> resetCallBackPeer( peer->getOperation("resetCallBackPeer"), tc->engine() );
        int count = ++callBackPeer_count;

        if (callBackPeer_step == INITIAL) {
            log(Info) << "Test resets server." <<endlog();
            resetCallBackPeer();
            callBackPeer_step = CALL;
        }

        log(Info) << "Test executes callBackPeer():"<< count <<endlog();
        if (callBackPeer_step == CALL) {
            callBackPeer_step = SEND;
            log(Info) << "Test calls server:" << count <<endlog();
            op1(tc, "callBackPeer");
            log(Info) << "Test finishes server call:"<<count <<endlog();
        }
        else if (callBackPeer_step == SEND) {
            callBackPeer_step = FINAL;
            log(Info) << "Test sends server:"<<count <<endlog();
            handle = op1.send(tc, "callBackPeerOwn");
            log(Info) << "Test finishes server send:"<< count <<endlog();
        }
        log(Info) << "Test finishes callBackPeer():"<< count <<endlog();
    }

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
    tc->ports()->addEventPort( *mi,boost::bind(&CorbaTest::new_data_listener, this, _1) );
    tc->ports()->addPort( *mo );

    t2 = 0;
    ts2 = ts = 0;
    tp2 = tp = 0;
    callBackPeer_count = 0;
    callBackPeer_step = INITIAL;

    tc->addOperation("callBackPeer", &CorbaTest::callBackPeer, this,ClientThread);
    tc->addOperation("callBackPeerOwn", &CorbaTest::callBackPeer, this,OwnThread);
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
    ASSERT_PORT_SIGNALLING(mo->write(1.0), mi);
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


BOOST_AUTO_TEST_CASE( testRemoteOperationCallerC )
{
    tp = corba::TaskContextProxy::Create( "peerRMC", /* is_ior = */ false ); // no-ior
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerRMC.ior");
    BOOST_REQUIRE( tp );

    // This test tests 'transparant' remote invocation of Orocos internal::OperationCallerC objects.
    internal::OperationCallerC mc;
    double r = 0.0;
    mc = tp->provides("methods")->create("vm0", tc->engine() );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == 0.0 );

    mc = tp->provides("methods")->create("m0", tc->engine() ).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -1.0 );

    mc = tp->provides("methods")->create("m2", tc->engine()).argC(1).argC(2.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -3.0 );

    mc = tp->provides("methods")->create("m3", tc->engine()).ret( r ).argC(1).argC(2.0).argC(true);
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -4.0 );

}

BOOST_AUTO_TEST_CASE( testRemoteOperationCaller )
{
    tp = corba::TaskContextProxy::Create( "peerRM" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerRM.ior");
    BOOST_REQUIRE(tp);
    // This test tests 'transparant' remote invocation of Orocos methods.
    // This requires the internal::RemoteOperationCaller class, which does not work yet.
    RTT::OperationCaller<double(void)> m0 = tp->provides("methods")->getOperation("m0");
    RTT::OperationCaller<double(int)> m1 = tp->provides("methods")->getOperation("m1");
    RTT::OperationCaller<double(int,double)> m2 = tp->provides("methods")->getOperation("m2");
    RTT::OperationCaller<double(int,double,bool)> m3 = tp->provides("methods")->getOperation("m3");
    RTT::OperationCaller<double(int,double,bool,std::string)> m4 = tp->provides("methods")->getOperation("m4");

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
}

/**
 * Tests synchronous/asynchronous callbacks to self from a remote peer.
 * For example A->B->A or even A->B->A->B
 */
BOOST_AUTO_TEST_CASE( testRemoteOperationCallerCallback )
{
    tp = corba::TaskContextProxy::Create( "peerRMCb" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerRMC.ior");
    BOOST_REQUIRE(tp);

    BOOST_REQUIRE( RTT::internal::DataSourceTypeInfo<TaskContext*>::getTypeInfo() != 0 );
    BOOST_REQUIRE( RTT::internal::DataSourceTypeInfo<TaskContext*>::getTypeInfo() !=  RTT::internal::DataSourceTypeInfo<UnknownType>::getTypeInfo());
    BOOST_REQUIRE( RTT::internal::DataSourceTypeInfo<TaskContext*>::getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID) != 0 );

    BOOST_REQUIRE_EQUAL( (int) callBackPeer_step, (int) INITIAL );
    this->callBackPeer(tp, "callBackPeer");
    sleep(1); //asyncronous processing...
    BOOST_CHECK_EQUAL( (int) callBackPeer_step, (int) FINAL );
    BOOST_CHECK_EQUAL( callBackPeer_count, 3 );
    BOOST_CHECK( handle.ready() );
    BOOST_CHECK_EQUAL( handle.collectIfDone(), SendSuccess );
}

BOOST_AUTO_TEST_CASE( testAnyOperationCaller )
{
    double d;
    tp = corba::TaskContextProxy::Create( "peerAM" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerAM.ior");

    BOOST_REQUIRE(tp);
    s = tp->server();
    BOOST_REQUIRE( s );
    // This test tests the callOperation() function of the server.
    corba::CService_var co = s->getProvider("methods");
    BOOST_CHECK( co.in() );

    corba::CAnyArguments_var any_args = new corba::CAnyArguments(0);
    CORBA::Any_var vm0 = co->callOperation("vm0", any_args.inout() );
    //BOOST_CHECK( vm0.in() );

    CORBA::Any_var m0 = co->callOperation("m0", any_args.inout());
    BOOST_CHECK( m0 >>= d );
    BOOST_CHECK_EQUAL(d, -1.0 );

    any_args = new corba::CAnyArguments(1);
    any_args->length(1);
    unsigned int index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    CORBA::Any_var m1;
    BOOST_CHECK_NO_THROW( m1 = co->callOperation("m1", any_args.inout()));
    BOOST_CHECK( m1 >>= d );
    BOOST_CHECK_EQUAL(d, -2.0 );


    any_args = new corba::CAnyArguments(2);
    any_args->length(2);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    CORBA::Any_var m2;
    BOOST_CHECK_NO_THROW( m2 = co->callOperation("m2", any_args.inout()));
    BOOST_CHECK( m2 >>= d );
    BOOST_CHECK_EQUAL(d, -3.0 );

    any_args = new corba::CAnyArguments(3);
    any_args->length(3);
     index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( true );
    CORBA::Any_var m3;
    BOOST_CHECK_NO_THROW( m3= co->callOperation("m3", any_args.inout()) );
    BOOST_CHECK( m3 >>= d );
    BOOST_CHECK_EQUAL(d, -4.0 );

    any_args = new corba::CAnyArguments(4);
    any_args->length(4);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( true );
    ++index;
    any_args[index] <<= "hello";
    CORBA::Any_var m4;
    BOOST_CHECK_NO_THROW ( m4 = co->callOperation("m4", any_args.inout()) );
    BOOST_CHECK( m4 >>= d );
    BOOST_CHECK_EQUAL(d, -5.0 );
}

BOOST_AUTO_TEST_CASE(testDataFlowInterface)
{
    tp = corba::TaskContextProxy::Create( "peerDFI" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerDFI.ior");

    BOOST_REQUIRE(tp);
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
    CORBA::String_var cstr = ports->getDataType("mo");
    BOOST_CHECK_EQUAL(string("double"),
	    string(cstr.in()));
}

BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerPC", /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerPC.ior");

    BOOST_REQUIRE(tp);

    s = tp->server();
    // server to our own tc.
    ts2  = corba::TaskContextServer::Create( tc, /* use_naming = */ false );
    s2 = ts2->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::data());
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    corba::CDataFlowInterface_var ports  = s->ports();
    corba::CDataFlowInterface_var ports2 = s2->ports();

    // Test cases that should not connect
    BOOST_CHECK_THROW( ports->createConnection("mo", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("does_not_exist", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("does_not_exist", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mo", ports2, "mo", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mi", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mi", ports2, "mo", policy), CNoSuchPortException );

    // must be running to catch event port signalling.
    BOOST_CHECK( tc->start() );
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
    tp = corba::TaskContextProxy::Create( "peerPP" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerPP.ior");

    BOOST_REQUIRE(tp);

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
#if __cplusplus > 199711L
    unique_ptr<base::InputPortInterface>
#else
    auto_ptr<base::InputPortInterface>
#endif
            read_clone(dynamic_cast<base::InputPortInterface*>(read_port->clone()));
    BOOST_CHECK(mo->createConnection(*read_clone));
    BOOST_CHECK(read_clone->connected());
    BOOST_CHECK(!read_port->connected());
    mo->disconnect();
}

BOOST_AUTO_TEST_CASE( testDataHalfs )
{
    if(std::getenv("CI") != NULL) {
      BOOST_TEST_MESSAGE("Skipping testAffinity because it can fail on integration servers.");
      return;
    }

    double result;
    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerDH" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerDH.ior");

    BOOST_REQUIRE(tp);

    s = tp->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::data());
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

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
    wait_for_equal( cce->read( sample.out(), true), CNewData, 5 );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();
    mo->disconnect();

    // test unbuffered Corba write --> C++ read
    cce = ports->buildChannelOutput("mi", policy);
    cce->channelReady(policy);

    mi->connectTo( tp->ports()->getPort("mo"), toRTT(policy) );
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 4.44;
    cce->write( sample.in() );
    wait_for_equal( mi->read( result ), NewData, 5 );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}

BOOST_AUTO_TEST_CASE( testBufferHalfs )
{
    if(std::getenv("CI") != NULL) {
      BOOST_TEST_MESSAGE("Skipping testAffinity because it can fail on integration servers.");
      return;
    }

    double result;

    // This test tests the differen port-to-port connections.
    tp = corba::TaskContextProxy::Create( "peerBH" , /* is_ior = */ false);
    if (!tp )
        tp = corba::TaskContextProxy::CreateFromFile( "peerBH.ior");

    BOOST_REQUIRE(tp);

    s = tp->server();

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::buffer(10));
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

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
    wait_for_equal( cce->read( sample.out(), true), CNewData, 10 );
    sample >>= result;
#ifndef RTT_CORBA_PORTS_WRITE_ONEWAY
    BOOST_CHECK_EQUAL( result, 6.33);
#else
    BOOST_CHECK( (result == 6.33) || (result == 3.33) );
#endif
    wait_for_equal( cce->read( sample.out(), true ), CNewData, 10 );
    sample >>= result;
#ifndef RTT_CORBA_PORTS_WRITE_ONEWAY
    BOOST_CHECK_EQUAL( result, 3.33);
#else
    BOOST_CHECK( (result == 6.33) || (result == 3.33) );
#endif

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), COldData );
    sample >>= result;
#ifndef RTT_CORBA_PORTS_WRITE_ONEWAY
    BOOST_CHECK_EQUAL( result, 3.33);
#else
    BOOST_CHECK( (result == 6.33) || (result == 3.33) );
#endif

    cce->disconnect();
    mo->disconnect();

    // test buffered Corba write --> C++ read
    mi->connectTo( tp->ports()->getPort("mo"), toRTT(policy)  );
    cce = ports->buildChannelOutput("mi", policy);
    cce->channelReady(policy);
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 6.44;
    cce->write( sample.in() );
    sample <<= 4.44;
    cce->write( sample.in() );
    wait_for_equal( mi->read( result ), NewData, 5 );
    BOOST_CHECK_EQUAL( result, 6.44 );
    wait_for_equal( mi->read( result ), NewData, 5 );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}

BOOST_AUTO_TEST_SUITE_END()

