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

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CorbaTest );

void 
CorbaTest::setUp()
{
    md1 = new DataPort<double>("md", 1.0);
    md2 = new DataPort<double>("md", 1.0);
    mb1 = new BufferPort<double>("mb", 10, 1.0);
    mb2 = new BufferPort<double>("mb", 10, 1.0);
    
    tc =  new TaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
    t2 = new TaskContext("other");

    tc->ports()->addPort( md1 );
    t2->ports()->addPort( md2 );
    tc->ports()->addPort( mb1 );
    t2->ports()->addPort( mb2 );
}


void 
CorbaTest::tearDown()
{
    //delete tc;
    //delete ts;
    //delete tp;

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
    tp = Corba::ControlTaskProxy::Create( ts->server() );

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
    tp = Corba::ControlTaskProxy::Create( ts->server() );

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
    tp = Corba::ControlTaskProxy::Create( ts->server() );

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

void CorbaTest::testPorts()
{
    // test create channel functions of dataflowinterface.
    // write to corba read from C++ and vice verse.
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server() );

    // DATA PORTS
    ::RTT::Corba::AssignableExpression_var data = ts->server()->ports()->createDataChannel("md");
    CPPUNIT_ASSERT( data.in() );

    CORBA::Any_var any = new CORBA::Any();
    double value = 5.0;
    // Write from corba, read from C++
    any <<= value;
    CPPUNIT_ASSERT( data->set( any.in() ) );
    CPPUNIT_ASSERT_EQUAL( value, md1->Get() );

    // Write from C++, read from corba
    value = -5.0;
    md1->Set( value );
    any = data->get();
    any >>= value;
    CPPUNIT_ASSERT_EQUAL( -5.0, value );
    
    // BUFFER PORTS
    ::RTT::Corba::BufferChannel_var buf = ts->server()->ports()->createBufferChannel("mb");
    CPPUNIT_ASSERT(buf.in());
    CPPUNIT_ASSERT_EQUAL( buf->size(), mb1->size() );
    CPPUNIT_ASSERT_EQUAL( buf->capacity(), mb1->capacity() );

    any = new CORBA::Any();
    value = 5.0;
    // Write from corba, read from C++
    any <<= value;
    CPPUNIT_ASSERT( buf->push( any.in() ) );
    CPPUNIT_ASSERT_EQUAL( buf->size(), mb1->size() );

    CPPUNIT_ASSERT_EQUAL( 5.0, mb1->front() );
    CPPUNIT_ASSERT( mb1->Pop( value ) );
    CPPUNIT_ASSERT_EQUAL( 5.0, value );
    CPPUNIT_ASSERT_EQUAL( buf->size(), mb1->size() );

    // Write from C++, read from corba
    value = -5.0;
    mb1->Push( value );
    CPPUNIT_ASSERT_EQUAL( buf->size(), mb1->size() );

    any = buf->front();
    value = 0.0;
    any >>= value;
    CPPUNIT_ASSERT_EQUAL( -5.0, value );
    CPPUNIT_ASSERT( buf->pull( any.out() ) );
    value = 0.0;
    any >>= value;
    CPPUNIT_ASSERT_EQUAL( -5.0, value );
    CPPUNIT_ASSERT_EQUAL( buf->size(), mb1->size() );

}

// Test the IDL connectPorts statement.
void CorbaTest::testConnectPorts()
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server() );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server() );
    
    CPPUNIT_ASSERT( ts->server()->connectPorts( ts2->server() ) );

    testPortStats();
    testPortDisconnect();
}

void CorbaTest::testPortStats()
{
    // Tests if ports are correctly working, is called by one of the other functions.
    // DATA PORTS
    // Check if connection succeeded both ways:
    CPPUNIT_ASSERT( md1->connected() );
    CPPUNIT_ASSERT( md2->connected() );
    // Check if both ports return same initial value: 
    CPPUNIT_ASSERT_EQUAL( 1.0, md1->Get() );
    CPPUNIT_ASSERT_EQUAL( 1.0, md2->Get() );
    
    // Check writing from both ways:
    md1->Set( 3.0 );
    CPPUNIT_ASSERT_EQUAL( 3.0, md1->Get() );
    CPPUNIT_ASSERT_EQUAL( 3.0, md2->Get() );
    md2->Set( -3.0 );
    CPPUNIT_ASSERT_EQUAL( -3.0, md1->Get() );
    CPPUNIT_ASSERT_EQUAL( -3.0, md2->Get() );
    
    // 
    // BUFFER PORTS
    // Check if connection succeeded both ways:
    double val = 0.0;
    CPPUNIT_ASSERT( mb1->connected() );
    CPPUNIT_ASSERT( mb2->connected() );
    //CPPUNIT_ASSERT_EQUAL( 10, mb2->size() );
    
    // Check writing from both ways:
    CPPUNIT_ASSERT( mb1->Push( 3.0 ) );
    CPPUNIT_ASSERT( mb1->front() == 3.0 );
    CPPUNIT_ASSERT( mb2->front() == 3.0 );
    CPPUNIT_ASSERT( mb2->Pop( val ));
    CPPUNIT_ASSERT( val == 3.0 );
    
    CPPUNIT_ASSERT( mb2->Push( -3.0 ) );
    CPPUNIT_ASSERT( mb1->front() == -3.0 );
    CPPUNIT_ASSERT( mb2->front() == -3.0 );
    CPPUNIT_ASSERT( mb2->Pop( val ));
    CPPUNIT_ASSERT( val == -3.0 );
}

void CorbaTest::testPortDisconnect()
{
    // Connection management.
    // DATA PORTS
    CPPUNIT_ASSERT( md1->connected() );
    CPPUNIT_ASSERT( md2->connected() );

    // store -3.0 in connection.
    md1->Set( -3.0 );

    md1->disconnect();
    md2->disconnect();

    CPPUNIT_ASSERT( !md1->connected() );
    CPPUNIT_ASSERT( !md2->connected() );

    // Check no writing from both ways:
    md1->Set( 3.0 );
    CPPUNIT_ASSERT_EQUAL( 3.0, md1->Get() );
    CPPUNIT_ASSERT( 3.0 != md2->Get() );
    md2->Set( 6.0 );
    CPPUNIT_ASSERT_EQUAL( 3.0, md1->Get() );
    CPPUNIT_ASSERT_EQUAL( 6.0, md2->Get() );
    
    // 
    // BUFFER PORTS
    // Check if connection succeeded both ways:
    double val = 0.0;
    CPPUNIT_ASSERT( mb1->connected() );
    CPPUNIT_ASSERT( mb2->connected() );
    
    // Store a value.
    CPPUNIT_ASSERT( mb1->Push( 3.0 ) );

    // Disconnect
    mb1->disconnect();
    mb2->disconnect();

    // should fail
    CPPUNIT_ASSERT( !mb1->Push( 3.0 ) );
    CPPUNIT_ASSERT( !mb2->Pop( val ));
    
    CPPUNIT_ASSERT( !mb2->Push( -3.0 ) );
    CPPUNIT_ASSERT( !mb2->Pop( val ));
}

void CorbaTest::testConnections()
{
    // This test tests the differen port-to-port connections.
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server() );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server() );

    // incompatible type should fail
    CPPUNIT_ASSERT( !ts->server()->ports()->connectPorts("md", ts2->server()->ports(), "mb") );

    CPPUNIT_ASSERT( ts->server()->ports()->connectPorts("md", ts2->server()->ports(), "md") );
    CPPUNIT_ASSERT( ts->server()->ports()->connectPorts("mb", ts2->server()->ports(), "mb") );

    testPortStats();
    testPortDisconnect();
    
    
}

