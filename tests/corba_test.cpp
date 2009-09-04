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



#include "corba_test.hpp"

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <Method.hpp>
#include <interface/OperationInterface.hpp>
#include <transports/corba/DataFlowI.h>
#include <transports/corba/RemotePorts.hpp>
#include <transports/corba/OperationsC.h>
#include <transports/corba/OperationInterfaceC.h>

using namespace std;
using corba::ControlTaskProxy;

void
CorbaTest::setUp()
{
    // connect DataPorts
    mr1 = new InputPort<double>("mr");
    mw1 = new OutputPort<double>("mw");

    mr2 = new InputPort<double>("mr");
    mw2 = new OutputPort<double>("mw");

    tc =  new TaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
    tc->ports()->addPort( mr1 );
    tc->ports()->addPort( mw1 );

    t2 = new TaskContext("other");
    t2->ports()->addPort( mr2 );
    t2->ports()->addPort( mw2 );

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

    delete mr1;
    delete mw1;
    delete mr2;
    delete mw2;
}

bool CorbaTest::assertBool( bool b) {
    return b;
}


TaskObject* CorbaTest::createMethodFactory()
{
    TaskObject* to = new TaskObject("methods");

    to->methods()->addMethod( method("assert",  &CorbaTest::assertBool, this), "assert","b","bd");

    to->methods()->addMethod( method("vm0",  &CorbaTest::vm0, this), "VM0");
    to->methods()->addMethod( method("m0",  &CorbaTest::m0, this), "M0");
    to->methods()->addMethod( method("m1",  &CorbaTest::m1, this), "M1","a","ad");
    to->methods()->addMethod( method("m2",  &CorbaTest::m2, this), "M2","a","ad","a","ad");
    to->methods()->addMethod( method("m3",  &CorbaTest::m3, this), "M3","a","ad","a","ad","a","ad");
    to->methods()->addMethod( method("m4",  &CorbaTest::m4, this), "M4","a","ad","a","ad","a","ad","a","ad");
    return to;
}

void CorbaTest::new_data_listener(PortInterface* port)
{
    signalled_port = port;
}


#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; \
    code; \
    BOOST_CHECK( read_port == signalled_port );

void CorbaTest::testPortDataConnection()
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

void CorbaTest::testPortBufferConnection()
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
    ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 1.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 2.0, value );
    BOOST_CHECK( mr2->read(value) );
    BOOST_CHECK_EQUAL( 3.0, value );
    BOOST_CHECK( !mr2->read(value) );
}

void CorbaTest::testPortDisconnected()
{
    BOOST_CHECK( !mw1->connected() );
    BOOST_CHECK( !mr2->connected() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaTestSuite,  CorbaTest )


BOOST_AUTO_TEST_CASE( setupCorba )
{
    corba::ControlTaskProxy::InitOrb(0,0);
}

BOOST_AUTO_TEST_CASE( testRemoteMethodC )
{

    ts = corba::ControlTaskServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = corba::ControlTaskProxy::Create( ts->server(), true );
    BOOST_CHECK( tp );

    // This test tests 'transparant' remote invocation of Orocos MethodC objects.
    MethodC mc;
    double r = 0.0;
    mc = tp->getObject("methods")->methods()->create("vm0");
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == 0.0 );

    mc = tp->getObject("methods")->methods()->create("m0").ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -1.0 );

    mc = tp->getObject("methods")->methods()->create("m2").argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -3.0 );

    mc = tp->getObject("methods")->methods()->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -4.0 );

}

BOOST_AUTO_TEST_CASE( testRemoteMethod )
{

    ts = corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = corba::ControlTaskProxy::Create( ts->server(), true );

    // This test tests 'transparant' remote invocation of Orocos methods.
    // This requires the RemoteMethod class, which does not work yet.
    RTT::Method<double(void)> m0 = tp->getObject("methods")->methods()->getMethod<double(void)>("m0");
    RTT::Method<double(int)> m1 = tp->getObject("methods")->methods()->getMethod<double(int)>("m1");
    RTT::Method<double(int,double)> m2 = tp->getObject("methods")->methods()->getMethod<double(int,double)>("m2");
    RTT::Method<double(int,double,bool)> m3 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool)>("m3");
    RTT::Method<double(int,double,bool,std::string)> m4 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool,std::string)>("m4");

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

BOOST_AUTO_TEST_CASE( testAnyMethod )
{

    ts = corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = corba::ControlTaskProxy::Create( ts->server() , true);

    // This test tests the createMethodAny() function of the server.
    corba::CControlObject_var co = ts->server()->getObject("methods");
    BOOST_CHECK( co.in() );

    corba::CMethodInterface_var methods = co->methods();
    BOOST_CHECK( methods.in() );

    corba::CAnyArguments_var any_args = new corba::CAnyArguments(0);
    corba::CMethod_var vm0 = methods->createMethodAny("vm0", any_args.in());
    BOOST_CHECK( vm0.in() );

    BOOST_CHECK( vm0->executeAny( any_args.in() ) );

    corba::CMethod_var m0 = methods->createMethodAny("m0", any_args.in());
    BOOST_CHECK( m0.in() );

    BOOST_CHECK( m0->executeAny( any_args.in() ) );

    any_args = new corba::CAnyArguments(1);
    any_args->length(1);
    unsigned int index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    corba::CMethod_var m1;
    BOOST_CHECK_NO_THROW( m1 = methods->createMethodAny("m1", any_args.in()));
    BOOST_CHECK( m1.in() );

    BOOST_CHECK(m1->executeAny( any_args.in() ));

    any_args = new corba::CAnyArguments(2);
    any_args->length(2);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    corba::CMethod_var m2;
    BOOST_CHECK_NO_THROW( m2 = methods->createMethodAny("m2", any_args.in()));
    BOOST_CHECK( m2.in() );

    BOOST_CHECK(m2->executeAny( any_args.in() ));

    any_args = new corba::CAnyArguments(3);
    any_args->length(3);
     index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    corba::CMethod_var m3;
    BOOST_CHECK_NO_THROW( m3= methods->createMethodAny("m3", any_args.in()) );
    BOOST_CHECK( m3.in() );

    BOOST_CHECK(m3->executeAny( any_args.in() ));

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
    BOOST_CHECK_NO_THROW ( m4 = methods->createMethodAny("m4", any_args.in()) );
    BOOST_CHECK( m4.in() );

    BOOST_CHECK(m4->executeAny( any_args.in() ));
}

BOOST_AUTO_TEST_CASE(testDataFlowInterface)
{
    ts = corba::ControlTaskServer::Create( tc, false ); //no-naming

    corba::CDataFlowInterface_var ports = ts->server()->ports();

    corba::CDataFlowInterface::CPortNames_var names =
	ports->getPorts();

    BOOST_CHECK_EQUAL(CORBA::ULong(2), names->length());
    BOOST_CHECK_EQUAL(string("mr"), string(names[CORBA::ULong(0)]));
    BOOST_CHECK_EQUAL(string("mw"), string(names[CORBA::ULong(1)]));

    // Now check directions
    BOOST_CHECK_EQUAL(RTT::corba::COutput,
	    ports->getPortType("mw"));
    BOOST_CHECK_EQUAL(RTT::corba::CInput,
	    ports->getPortType("mr"));

    // And check type names
    BOOST_CHECK_EQUAL(string("double"),
	    string(ports->getDataType("mw")));
}

BOOST_AUTO_TEST_CASE( testPortConnections )
{
    // This test tests the differen port-to-port connections.
    ts  = corba::ControlTaskServer::Create( tc, false ); //no-naming
    ts2 = corba::ControlTaskServer::Create( t2, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy;
    policy.type = RTT::corba::CData;
    policy.init = false;
    policy.lock_policy = RTT::corba::CLockFree;
    policy.size = 0;
    policy.transport = 0;

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mr2->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    hl.connect();

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = ts2->server()->ports();

    // Test cases that should not connect
    BOOST_CHECK( !ports->createConnection("mw", ports2, "does_not_exist", policy) );
    BOOST_CHECK( !ports->createConnection("does_not_exist", ports2, "mr", policy) );
    BOOST_CHECK( !ports->createConnection("does_not_exist", ports2, "does_not_exist", policy) );
    BOOST_CHECK( !ports->createConnection("mw", ports2, "mw", policy) );
    BOOST_CHECK( !ports->createConnection("mr", ports2, "mr", policy) );
    BOOST_CHECK( !ports->createConnection("mr", ports2, "mw", policy) );

    // WARNING: in the following, there is four configuration tested. There is
    // also three different ways to disconnect. We need to test those three
    // "disconnection methods", so beware when you change something ...

    policy.type = RTT::corba::CData;
    policy.pull = false;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortDataConnection();
    ports->disconnect("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CData;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortDataConnection();
    ports2->disconnect("mr");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = false;
    policy.size = 3;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    ports->disconnect("mw");
    testPortDisconnected();

    policy.type = RTT::corba::CBuffer;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    // Here, check removal of specific connections. So first add another
    // connection ...
    mw1->createConnection(*mr1);
    // Remove the remote connection
    ports->disconnectPort("mw", ports2, "mr");
    // Check it is removed
    BOOST_CHECK(mw1->connected());
    BOOST_CHECK(mr1->connected());
    BOOST_CHECK(!mr2->connected());
}

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

BOOST_AUTO_TEST_CASE( cleanupCorba )
{
    corba::ControlTaskProxy::DestroyOrb();
}

BOOST_AUTO_TEST_SUITE_END()

