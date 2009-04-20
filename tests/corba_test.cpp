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
#include <unistd.h>
#include <iostream>

#include <Method.hpp>
#include <OperationInterface.hpp>
#include <corba/DataFlowI.h>
#include <corba/RemotePorts.hpp>

using namespace std;
using Corba::ControlTaskProxy;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CorbaTest );

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


void CorbaTest::setupCorba()
{
    Corba::ControlTaskProxy::InitOrb(0,0);
}

void CorbaTest::cleanupCorba()
{
    Corba::ControlTaskProxy::DestroyOrb();
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

void CorbaTest::testRemoteMethodC()
{

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );

    // This test tests 'transparant' remote invocation of Orocos MethodC objects.
    MethodC mc;
    double r = 0.0;
    mc = tp->getObject("methods")->methods()->create("vm0");
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == 0.0 );

    mc = tp->getObject("methods")->methods()->create("m0").ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -1.0 );

    mc = tp->getObject("methods")->methods()->create("m2").argC(1).argC(1.0).ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -3.0 );

    mc = tp->getObject("methods")->methods()->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -4.0 );

}


void CorbaTest::testRemoteMethod()
{

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );

    // This test tests 'transparant' remote invocation of Orocos methods.
    // This requires the RemoteMethod class, which does not work yet.
    Method<double(void)> m0 = tp->getObject("methods")->methods()->getMethod<double(void)>("m0");
    Method<double(int)> m1 = tp->getObject("methods")->methods()->getMethod<double(int)>("m1");
    Method<double(int,double)> m2 = tp->getObject("methods")->methods()->getMethod<double(int,double)>("m2");
    Method<double(int,double,bool)> m3 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool)>("m3");
    Method<double(int,double,bool,std::string)> m4 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool,std::string)>("m4");

    CPPUNIT_ASSERT_EQUAL( -1.0, m0() );
    CPPUNIT_ASSERT_EQUAL( -2.0, m1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, m2(1, 2.0) );
    CPPUNIT_ASSERT_EQUAL( -4.0, m3(1, 2.0, false) );
    CPPUNIT_ASSERT_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

void CorbaTest::testAnyMethod()
{

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server() , true);

    // This test tests the createMethodAny() function of the server.
    Corba::ControlObject_var co = ts->server()->getObject("methods");
    CPPUNIT_ASSERT( co.in() );

    Corba::MethodInterface_var methods = co->methods();
    CPPUNIT_ASSERT( methods.in() );

    Corba::AnyArguments_var any_args = new Corba::AnyArguments(0);
    Corba::Method_var vm0 = methods->createMethodAny("vm0", any_args.in());
    CPPUNIT_ASSERT( vm0.in() );

    CPPUNIT_ASSERT( vm0->executeAny( any_args.in() ) );

    Corba::Method_var m0 = methods->createMethodAny("m0", any_args.in());
    CPPUNIT_ASSERT( m0.in() );

    CPPUNIT_ASSERT( m0->executeAny( any_args.in() ) );

    any_args = new Corba::AnyArguments(1);
    any_args->length(1);
    unsigned int index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    Corba::Method_var m1;
    CPPUNIT_ASSERT_NO_THROW( m1 = methods->createMethodAny("m1", any_args.in()));
    CPPUNIT_ASSERT( m1.in() );

    CPPUNIT_ASSERT(m1->executeAny( any_args.in() ));

    any_args = new Corba::AnyArguments(2);
    any_args->length(2);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    Corba::Method_var m2;
    CPPUNIT_ASSERT_NO_THROW( m2 = methods->createMethodAny("m2", any_args.in()));
    CPPUNIT_ASSERT( m2.in() );

    CPPUNIT_ASSERT(m2->executeAny( any_args.in() ));

    any_args = new Corba::AnyArguments(3);
    any_args->length(3);
     index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    Corba::Method_var m3;
    CPPUNIT_ASSERT_NO_THROW( m3= methods->createMethodAny("m3", any_args.in()) );
    CPPUNIT_ASSERT( m3.in() );

    CPPUNIT_ASSERT(m3->executeAny( any_args.in() ));

    any_args = new Corba::AnyArguments(4);
    any_args->length(4);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    ++index;
    any_args[index] <<= "hello";
    Corba::Method_var m4;
    CPPUNIT_ASSERT_NO_THROW ( m4 = methods->createMethodAny("m4", any_args.in()) );
    CPPUNIT_ASSERT( m4.in() );

    CPPUNIT_ASSERT(m4->executeAny( any_args.in() ));
}

// TODO: test connectPorts on task contexts

void CorbaTest::testDataFlowInterface()
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    Corba::DataFlowInterface_var ports = ts->server()->ports();

    Corba::DataFlowInterface::PortNames_var names =
	ports->getPorts();

    CPPUNIT_ASSERT_EQUAL(CORBA::ULong(2), names->length());
    CPPUNIT_ASSERT_EQUAL(string("mr"), string(names[0]));
    CPPUNIT_ASSERT_EQUAL(string("mw"), string(names[1]));

    // Now check directions
    CPPUNIT_ASSERT_EQUAL(RTT::Corba::Output,
	    ports->getPortType("mw"));
    CPPUNIT_ASSERT_EQUAL(RTT::Corba::Input,
	    ports->getPortType("mr"));

    // And check type names
    CPPUNIT_ASSERT_EQUAL(string("double"),
	    string(ports->getDataType("mw")));
}

#define ASSERT_PORT_SIGNALLING(code, read_port) \
    signalled_port = 0; \
    code; \
    CPPUNIT_ASSERT( read_port == signalled_port );

void CorbaTest::new_data_listener(PortInterface* port)
{
    signalled_port = port;
}
void CorbaTest::testPortDataConnection()
{
    // This test assumes that there is a data connection mw1 => mr2
    // Check if connection succeeded both ways:
    CPPUNIT_ASSERT( mw1->connected() );
    CPPUNIT_ASSERT( mr2->connected() );

    double value = 0;

    // Check if no-data works
    CPPUNIT_ASSERT( !mr2->read(value) );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2)
    CPPUNIT_ASSERT( mr2->read(value) );
    CPPUNIT_ASSERT_EQUAL( 1.0, value );

    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    CPPUNIT_ASSERT( mr2->read(value) );
    CPPUNIT_ASSERT_EQUAL( 2.0, value );
}

void CorbaTest::testPortBufferConnection()
{
    // This test assumes that there is a buffer connection mw1 => mr2 of size 3
    // Check if connection succeeded both ways:
    CPPUNIT_ASSERT( mw1->connected() );
    CPPUNIT_ASSERT( mr2->connected() );

    double value = 0;

    // Check if no-data works
    CPPUNIT_ASSERT( !mr2->read(value) );

    // Check if writing works
    ASSERT_PORT_SIGNALLING(mw1->write(1.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(2.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(3.0), mr2);
    ASSERT_PORT_SIGNALLING(mw1->write(4.0), 0);
    CPPUNIT_ASSERT( mr2->read(value) );
    CPPUNIT_ASSERT_EQUAL( 1.0, value );
    CPPUNIT_ASSERT( mr2->read(value) );
    CPPUNIT_ASSERT_EQUAL( 2.0, value );
    CPPUNIT_ASSERT( mr2->read(value) );
    CPPUNIT_ASSERT_EQUAL( 3.0, value );
    CPPUNIT_ASSERT( !mr2->read(value) );
}

void CorbaTest::testPortDisconnected()
{
    CPPUNIT_ASSERT( !mw1->connected() );
    CPPUNIT_ASSERT( !mr2->connected() );
}

void CorbaTest::testPortConnections()
{
    // This test tests the differen port-to-port connections.
    ts  = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::Corba::ConnPolicy policy;
    policy.type = RTT::Corba::Data;
    policy.init = false;
    policy.lock_policy = RTT::Corba::LockFree;
    policy.size = 0;

    // Set up an event handler to check if signalling works properly as well
    Handle hl( mr2->getNewDataOnPortEvent()->setup(
                boost::bind(&CorbaTest::new_data_listener, this, _1) ) );
    hl.connect();

    Corba::DataFlowInterface_var ports  = ts->server()->ports();
    Corba::DataFlowInterface_var ports2 = ts2->server()->ports();

    // Test cases that should not connect
    CPPUNIT_ASSERT( !ports->createConnection("mw", ports2, "does_not_exist", policy) );
    CPPUNIT_ASSERT( !ports->createConnection("does_not_exist", ports2, "mr", policy) );
    CPPUNIT_ASSERT( !ports->createConnection("does_not_exist", ports2, "does_not_exist", policy) );
    CPPUNIT_ASSERT( !ports->createConnection("mw", ports2, "mw", policy) );
    CPPUNIT_ASSERT( !ports->createConnection("mr", ports2, "mr", policy) );
    CPPUNIT_ASSERT( !ports->createConnection("mr", ports2, "mw", policy) );

    // WARNING: in the following, there is four configuration tested. There is
    // also three different ways to disconnect. We need to test those three
    // "disconnection methods", so beware when you change something ...

    policy.type = RTT::Corba::Data;
    policy.pull = false;
    CPPUNIT_ASSERT( ports->createConnection("mw", ports2, "mr", policy) );
    testPortDataConnection();
    ports->disconnect("mw");
    testPortDisconnected();

    policy.type = RTT::Corba::Data;
    policy.pull = true;
    CPPUNIT_ASSERT( ports->createConnection("mw", ports2, "mr", policy) );
    testPortDataConnection();
    ports2->disconnect("mr");
    testPortDisconnected();

    policy.type = RTT::Corba::Buffer;
    policy.pull = false;
    policy.size = 3;
    CPPUNIT_ASSERT( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    ports->disconnect("mw");
    testPortDisconnected();

    policy.type = RTT::Corba::Buffer;
    policy.pull = true;
    CPPUNIT_ASSERT( ports->createConnection("mw", ports2, "mr", policy) );
    testPortBufferConnection();
    // Here, check removal of specific connections. So first add another
    // connection ...
    mw1->createConnection(*mr1);
    // Remove the remote connection
    ports->disconnectPort("mw", ports2, "mr");
    // Check it is removed
    CPPUNIT_ASSERT(mw1->connected());
    CPPUNIT_ASSERT(mr1->connected());
    CPPUNIT_ASSERT(!mr2->connected());
}

void CorbaTest::testPortProxying()
{
    ts  = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp  = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2  = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2  = Corba::ControlTaskProxy::Create( ts2->server(), true );

    PortInterface* untyped_port;
     
    untyped_port = tp->ports()->getPort("mr");
    CPPUNIT_ASSERT(untyped_port);
    InputPortInterface* read_port = dynamic_cast<InputPortInterface*>(tp->ports()->getPort("mr"));
    CPPUNIT_ASSERT(read_port);
     
    untyped_port = tp->ports()->getPort("mr");
    CPPUNIT_ASSERT(untyped_port);
    OutputPortInterface* write_port = dynamic_cast<OutputPortInterface*>(tp2->ports()->getPort("mw"));
    CPPUNIT_ASSERT(write_port);

    // Just make sure 'read_port' and 'write_port' are actually proxies and not
    // the real thing
    CPPUNIT_ASSERT(dynamic_cast<Corba::RemoteInputPort*>(read_port));
    CPPUNIT_ASSERT(dynamic_cast<Corba::RemoteOutputPort*>(write_port));

    CPPUNIT_ASSERT(!read_port->connected());
    CPPUNIT_ASSERT(read_port->getTypeInfo() == mr1->getTypeInfo());
    CPPUNIT_ASSERT(!write_port->connected());
    CPPUNIT_ASSERT(write_port->getTypeInfo() == mw2->getTypeInfo());

    mw2->createConnection(*read_port);
    CPPUNIT_ASSERT(read_port->connected());
    CPPUNIT_ASSERT(write_port->connected());
    read_port->disconnect();
    CPPUNIT_ASSERT(!read_port->connected());
    CPPUNIT_ASSERT(!write_port->connected());

    mw2->createConnection(*read_port);
    CPPUNIT_ASSERT(read_port->connected());
    CPPUNIT_ASSERT(write_port->connected());
    write_port->disconnect();
    CPPUNIT_ASSERT(!read_port->connected());
    CPPUNIT_ASSERT(!write_port->connected());

    // Test cloning
    auto_ptr<InputPortInterface> read_clone(dynamic_cast<InputPortInterface*>(read_port->clone()));
    CPPUNIT_ASSERT(mw2->createConnection(*read_clone));
    CPPUNIT_ASSERT(read_clone->connected());
    CPPUNIT_ASSERT(!read_port->connected());
    mw2->disconnect();
}

