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

#include <transports/corba/corba.h>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/OperationCaller.hpp>
#include <rtt/TaskContext.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
#include <rtt/Service.hpp>
#include <rtt/transports/corba/DataFlowI.h>
#include <rtt/transports/corba/RemotePorts.hpp>
#include <transports/corba/ServiceC.h>
#include <transports/corba/CorbaLib.hpp>

#include "operations_fixture.hpp"

using namespace std;
using corba::TaskContextProxy;

class CorbaTest : public OperationsFixture
{
public:
    CorbaTest() :
        pint1("pint1", "", 3), pdouble1("pdouble1", "", -3.0),
        aint1(3), adouble1(-3.0), wait(0)
    {
    // connect DataPorts
        mi1 = new InputPort<double> ("mi");
        mo1 = new OutputPort<double> ("mo");

        mi2 = new InputPort<double> ("mi");
        mo2 = new OutputPort<double> ("mo");

        tc->ports()->addPort(*mi1);
        tc->ports()->addPort(*mo1);

        t2 = new TaskContext("other");
        t2->ports()->addPort(*mi2);
        t2->ports()->addPort(*mo2);

        ts2 = ts = 0;
        tp2 = tp = 0;

        // store nested properties:
        tc->provides()->addProperty(pint1);
        storeProperty(*tc->provides()->properties(), "s1.s2", pdouble1.clone() );

        tc->addAttribute("aint1", aint1);
        tc->addAttribute("adouble1", adouble1);
    }
    ~CorbaTest()
    {
        delete tp;
        delete ts;
        delete tp2;
        delete ts2;
        delete t2;

        delete mi1;
        delete mo1;
        delete mi2;
        delete mo2;
    }

    TaskContext* t2;
    TaskContext* tp;
    corba::TaskContextServer* ts;
    TaskContext* tp2;
    corba::TaskContextServer* ts2;

    base::PortInterface* signalled_port;
    void new_data_listener(base::PortInterface* port);

    // Ports
    InputPort<double>*  mi1;
    OutputPort<double>* mo1;
    InputPort<double>*  mi2;
    OutputPort<double>* mo2;

    Property<int> pint1;
    Property<double> pdouble1;

    int aint1;
    double adouble1;
    int wait;

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

void CorbaTest::new_data_listener(base::PortInterface* port)
{
    signalled_port = port;
}


#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; wait = 0;\
    code; \
    while (read_port != signalled_port && wait++ != 5) \
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

void CorbaTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mo1 => mi2
    // Check if connection succeeded both ways:
    BOOST_CHECK( mo1->connected() );
    BOOST_CHECK( mi2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK_EQUAL( mi2->read(value), NoData );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mo1->write(1.0), mi2)
    BOOST_CHECK( mi2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    ASSERT_PORT_SIGNALLING(mo1->write(2.0), mi2);
    BOOST_CHECK( mi2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
}

void CorbaTest::testPortBufferConnection()
{
    // This test assumes that there is a buffer connection mo1 => mi2 of size 3
    // Check if connection succeeded both ways:
    BOOST_CHECK( mo1->connected() );
    BOOST_CHECK( mi2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK_EQUAL( mi2->read(value), NoData );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mo1->write(1.0), mi2);
    ASSERT_PORT_SIGNALLING(mo1->write(2.0), mi2);
    ASSERT_PORT_SIGNALLING(mo1->write(3.0), mi2);
    BOOST_CHECK( mi2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mi2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mi2->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK_EQUAL( mi2->read(value), OldData );
}

void CorbaTest::testPortDisconnected()
{
    BOOST_CHECK( !mo1->connected() );
    BOOST_CHECK( !mi2->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaTestSuite,  CorbaTest )

BOOST_AUTO_TEST_CASE( testAttributes )
{
    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = corba::TaskContextProxy::Create( ts->server(), true );
    BOOST_CHECK( tp );

    BOOST_CHECK( tp->provides()->hasAttribute("aint1") );
    Attribute<int> proxy_int = tp->provides()->getAttribute("aint1");
    BOOST_REQUIRE( proxy_int.ready() );
    BOOST_CHECK_EQUAL( proxy_int.get(), 3);

    BOOST_CHECK( tp->provides()->hasAttribute("adouble1") );
    Attribute<double> proxy_double = tp->provides()->getAttribute("adouble1");
    BOOST_REQUIRE( proxy_double.ready() );
    BOOST_CHECK_EQUAL( proxy_double.get(), -3.0);
}

BOOST_AUTO_TEST_CASE( testProperties )
{
    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = corba::TaskContextProxy::Create( ts->server(), true );
    BOOST_CHECK( tp );

    BOOST_CHECK( findProperty( *tp->provides()->properties(), "pint1") );
    Property<int> proxy_int = findProperty( *tp->provides()->properties(), "pint1");
    BOOST_REQUIRE( proxy_int.ready() );
    BOOST_CHECK_EQUAL( proxy_int.value(), 3);

    BOOST_CHECK( findProperty( *tp->provides()->properties(), "s1.s2.pdouble1") );
    Property<double> proxy_d = findProperty( *tp->provides()->properties(), "s1.s2.pdouble1");
    BOOST_REQUIRE( proxy_d.ready() );
    BOOST_CHECK_EQUAL( proxy_d.value(), -3.0);
}

BOOST_AUTO_TEST_CASE( testOperationCallerC_Call )
{

    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = corba::TaskContextProxy::Create( ts->server(), true );
    BOOST_CHECK( tp );

    // This test tests 'transparant' remote invocation of Orocos internal::OperationCallerC objects.
    internal::OperationCallerC mc;
    double r = 0.0;
    mc = tp->provides("methods")->create("vm0", tc->engine() );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == 0.0 );

    mc = tp->provides("methods")->create("m0", tc->engine() ).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -1.0 );

    mc = tp->provides("methods")->create("m2", tc->engine() ).argC(1).argC(2.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -3.0 );

    mc = tp->provides("methods")->create("m3", tc->engine() ).ret( r ).argC(1).argC(2.0).argC(true);
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -4.0 );

}

BOOST_AUTO_TEST_CASE( testOperationCallerC_Send )
{

    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = corba::TaskContextProxy::Create( ts->server(), true );
    BOOST_CHECK( tp );

    OperationCallerC mc;
    SendHandleC shc;
    double r = 0.0;
    double cr = 0.0;
    mc = tp->provides("methods")->create("m0", caller->engine()).ret( r );
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -1.0 );

    mc = tp->provides("methods")->create("m2", caller->engine()).argC(1).argC(2.0).ret( r );
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -3.0 );

    mc = tp->provides("methods")->create("m3", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true);
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -4.0 );

    mc = tp->provides("methods")->create("m4", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello"));
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -5.0 );
}

BOOST_AUTO_TEST_CASE( testRemoteOperationCallerCall )
{

    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    tp = corba::TaskContextProxy::Create( ts->server(), true );

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

BOOST_AUTO_TEST_CASE( testAnyOperationCaller )
{
    double d;

    ts = corba::TaskContextServer::Create( tc, false ); //no-naming
    tp = corba::TaskContextProxy::Create( ts->server() , true);

    // This test tests the callOperation() function of the server.
    corba::CService_var co = ts->server()->getProvider("methods");
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
    ts = corba::TaskContextServer::Create( tc, false ); //no-naming

    corba::CDataFlowInterface_var ports = ts->server()->ports();

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
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    ts2 = corba::TaskContextServer::Create( t2, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi2->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    hl.connect();

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = ts2->server()->ports();

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

    policy.type = RTT::corba::CData;
    policy.pull = false;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    testPortDataConnection();
    ports->disconnectPort("mo");
    testPortDisconnected();

    policy.type = RTT::corba::CData;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    testPortDataConnection();
    ports2->disconnectPort("mi");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = false;
    policy.size = 3;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    testPortBufferConnection();
    ports->disconnectPort("mo");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    testPortBufferConnection();
    // Here, check removal of specific connections. So first add another
    // connection ...
    mo1->createConnection(*mi1);
    // Remove the remote connection
    ports->removeConnection("mo", ports2, "mi");
    // Check it is removed
    BOOST_CHECK(mo1->connected());
    BOOST_CHECK(mi1->connected());
    BOOST_CHECK(!mi2->connected());
}

BOOST_AUTO_TEST_CASE( testPortProxying )
{
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    tp  = corba::TaskContextProxy::Create( ts->server(), true );
    ts2  = corba::TaskContextServer::Create( t2, false ); //no-naming
    tp2  = corba::TaskContextProxy::Create( ts2->server(), true );

    base::PortInterface* untyped_port;

    untyped_port = tp->ports()->getPort("mi");
    BOOST_CHECK(untyped_port);
    base::InputPortInterface* read_port = dynamic_cast<base::InputPortInterface*>(tp->ports()->getPort("mi"));
    BOOST_CHECK(read_port);

    untyped_port = tp->ports()->getPort("mi");
    BOOST_CHECK(untyped_port);
    base::OutputPortInterface* write_port = dynamic_cast<base::OutputPortInterface*>(tp2->ports()->getPort("mo"));
    BOOST_CHECK(write_port);

    // Just make sure 'read_port' and 'write_port' are actually proxies and not
    // the real thing
    BOOST_CHECK(dynamic_cast<corba::RemoteInputPort*>(read_port));
    BOOST_CHECK(dynamic_cast<corba::RemoteOutputPort*>(write_port));

    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(read_port->getTypeInfo() == mi1->getTypeInfo());
    BOOST_CHECK(!write_port->connected());
    BOOST_CHECK(write_port->getTypeInfo() == mo2->getTypeInfo());

    mo2->createConnection(*read_port);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    read_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    mo2->createConnection(*read_port);
    BOOST_CHECK(read_port->connected());
    BOOST_CHECK(write_port->connected());
    write_port->disconnect();
    BOOST_CHECK(!read_port->connected());
    BOOST_CHECK(!write_port->connected());

    // Test cloning
    auto_ptr<base::InputPortInterface> read_clone(dynamic_cast<base::InputPortInterface*>(read_port->clone()));
    BOOST_CHECK(mo2->createConnection(*read_clone));
    BOOST_CHECK(read_clone->connected());
    BOOST_CHECK(!read_port->connected());
    mo2->disconnect();
}

BOOST_AUTO_TEST_CASE( testDataHalfs )
{
    double result;
    // This test tests the differen port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi1->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    BOOST_CHECK( hl.connect() );

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNoData );
    // Check read of new data
    mo1->write( 3.33 );
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out() ), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();

    // test unbuffered Corba write --> C++ read
    cce = ports->buildChannelOutput("mi", policy);
    ports->channelReady("mi", cce);
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 4.44;
    cce->write( sample.in() );
    BOOST_CHECK_EQUAL( mi1->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi1->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}

BOOST_AUTO_TEST_CASE( testBufferHalfs )
{
    double result;
    // This test tests the differen port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CBuffer;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 10;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mi1->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    BOOST_CHECK( hl.connect() );

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    BOOST_CHECK_EQUAL( cce->read( sample.out() ), CNoData );
    // Check read of new data
    mo1->write( 6.33 );
    mo1->write( 3.33 );
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

    // test unbuffered Corba write --> C++ read
    cce = ports->buildChannelOutput("mi", policy);
    ports->channelReady("mi", cce);
    sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    // Check read of new data
    result = 0.0;
    sample <<= 6.44;
    cce->write( sample.in() );
    sample <<= 4.44;
    cce->write( sample.in() );
    BOOST_CHECK_EQUAL( mi1->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 6.44 );
    BOOST_CHECK_EQUAL( mi1->read( result ), NewData );
    BOOST_CHECK_EQUAL( result, 4.44 );

    // Check re-read of old data.
    result = 0.0;
    BOOST_CHECK_EQUAL( mi1->read( result ), OldData );
    BOOST_CHECK_EQUAL( result, 4.44);
}

BOOST_AUTO_TEST_SUITE_END()

