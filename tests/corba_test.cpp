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

#include <Method.hpp>
#include <OperationInterface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace std;
using Corba::ControlTaskProxy;

void
CorbaTest::setUp()
{
    // connect DataPorts
    md1 = new DataPort<double>("md", 1.0);
    md1bis = new DataPort<double>("mdbis", 2.0); // a free port.
    md2 = new DataPort<double>("md", -1.0);
    // connect Write to Read ports (connection direction testing)
    mdr1 = new ReadDataPort<double>("mdrwA");
    mdr2 = new ReadDataPort<double>("mdrwB");
    mdw1 = new WriteDataPort<double>("mdrwB", 1.0);
    mdw2 = new WriteDataPort<double>("mdrwA", -1.0);

    // connect BufferPorts
    mb1 = new BufferPort<double>("mb", 10, 1.0);
    mb2 = new BufferPort<double>("mb", 20, -1.0);

    tc =  new TaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
    t2 = new TaskContext("other");

    tc->ports()->addPort( md1 );
    tc->ports()->addPort( md1bis );
    t2->ports()->addPort( md2 );
    tc->ports()->addPort( mdr1 );
    t2->ports()->addPort( mdr2 );
    tc->ports()->addPort( mdw1 );
    t2->ports()->addPort( mdw2 );
    tc->ports()->addPort( mb1 );
    t2->ports()->addPort( mb2 );

    ts = 0;
    ts2 = 0;
    tp = 0;
    tp2 = 0;
}


void
CorbaTest::tearDown()
{
    delete md1;
    delete md1bis;
    delete md2;
    delete mdr1;
    delete mdr2;
    delete mdw1;
    delete mdw2;
    delete mb1;
    delete mb2;

    Corba::ControlTaskServer::CleanupServer( tc );
    delete tc;
    //delete ts; //server is deleted by tc's destruction;
    //delete ts2;
    delete tp;
    delete tp2;
    delete t2;

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

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaTestSuite,  CorbaTest )


BOOST_AUTO_TEST_CASE( setupCorba )
{
    Corba::ControlTaskProxy::InitOrb(0,0);
}

BOOST_AUTO_TEST_CASE( testRemoteMethodC )
{

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    BOOST_CHECK( ts );
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
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

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );

    // This test tests 'transparant' remote invocation of Orocos methods.
    // This requires the RemoteMethod class, which does not work yet.
    Method<double(void)> m0 = tp->getObject("methods")->methods()->getMethod<double(void)>("m0");
    Method<double(int)> m1 = tp->getObject("methods")->methods()->getMethod<double(int)>("m1");
    Method<double(int,double)> m2 = tp->getObject("methods")->methods()->getMethod<double(int,double)>("m2");
    Method<double(int,double,bool)> m3 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool)>("m3");
    Method<double(int,double,bool,std::string)> m4 = tp->getObject("methods")->methods()->getMethod<double(int,double,bool,std::string)>("m4");

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

BOOST_AUTO_TEST_CASE( testAnyMethod )
{

    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server() , true);

    // This test tests the createMethodAny() function of the server.
    Corba::ControlObject_var co = ts->server()->getObject("methods");
    BOOST_CHECK( co.in() );

    Corba::MethodInterface_var methods = co->methods();
    BOOST_CHECK( methods.in() );

    Corba::AnyArguments_var any_args = new Corba::AnyArguments(0);
    Corba::Method_var vm0 = methods->createMethodAny("vm0", any_args.in());
    BOOST_CHECK( vm0.in() );

    BOOST_CHECK( vm0->executeAny( any_args.in() ) );

    Corba::Method_var m0 = methods->createMethodAny("m0", any_args.in());
    BOOST_CHECK( m0.in() );

    BOOST_CHECK( m0->executeAny( any_args.in() ) );

    any_args = new Corba::AnyArguments(1);
    any_args->length(1);
    unsigned int index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    Corba::Method_var m1;
    BOOST_CHECK_NO_THROW( m1 = methods->createMethodAny("m1", any_args.in()));
    BOOST_CHECK( m1.in() );

    BOOST_CHECK(m1->executeAny( any_args.in() ));

    any_args = new Corba::AnyArguments(2);
    any_args->length(2);
    index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    Corba::Method_var m2;
    BOOST_CHECK_NO_THROW( m2 = methods->createMethodAny("m2", any_args.in()));
    BOOST_CHECK( m2.in() );

    BOOST_CHECK(m2->executeAny( any_args.in() ));

    any_args = new Corba::AnyArguments(3);
    any_args->length(3);
     index = 0;
    any_args[index] <<= (CORBA::Long) 1;
    ++index;
    any_args[index] <<= (CORBA::Double) 2.0;
    ++index;
    any_args[index] <<= CORBA::Any::from_boolean( false );
    Corba::Method_var m3;
    BOOST_CHECK_NO_THROW( m3= methods->createMethodAny("m3", any_args.in()) );
    BOOST_CHECK( m3.in() );

    BOOST_CHECK(m3->executeAny( any_args.in() ));

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
    BOOST_CHECK_NO_THROW ( m4 = methods->createMethodAny("m4", any_args.in()) );
    BOOST_CHECK( m4.in() );

    BOOST_CHECK(m4->executeAny( any_args.in() ));
}

BOOST_AUTO_TEST_CASE( testPorts )
{
    // test create channel functions of dataflowinterface.
    // write to corba read from C++ and vice verse.
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );

    // DATA PORTS
    ::RTT::Corba::AssignableExpression_var unknown_data = ts->server()->ports()->createDataChannel("does_not_exist");
    BOOST_CHECK( CORBA::is_nil(unknown_data) );
    ::RTT::Corba::AssignableExpression_var buffer_as_data = ts->server()->ports()->createDataChannel("mb");
    BOOST_CHECK( CORBA::is_nil(buffer_as_data) );
    ::RTT::Corba::AssignableExpression_var data = ts->server()->ports()->createDataChannel("md");
    BOOST_CHECK( data.in() );

    CORBA::Any_var any = new CORBA::Any();
    double value = 5.0;
    // Write from corba, read from C++
    any <<= value;
    BOOST_CHECK( data->set( any.in() ) );
    BOOST_CHECK_EQUAL( value, md1->Get() );

    // Write from C++, read from corba
    value = -5.0;
    md1->Set( value );
    any = data->get();
    any >>= value;
    BOOST_CHECK_EQUAL( -5.0, value );

    // BUFFER PORTS
    ::RTT::Corba::BufferChannel_var unknown_buf = ts->server()->ports()->createBufferChannel("does_not_exist");
    BOOST_CHECK( CORBA::is_nil(unknown_buf) );
    ::RTT::Corba::BufferChannel_var data_as_buffer = ts->server()->ports()->createBufferChannel("md");
    BOOST_CHECK( CORBA::is_nil(data_as_buffer) );
    ::RTT::Corba::BufferChannel_var buf = ts->server()->ports()->createBufferChannel("mb");
    BOOST_CHECK(buf.in());
    BOOST_CHECK_EQUAL( (int)buf->size(), mb1->size() );
    BOOST_CHECK_EQUAL( (int)buf->capacity(), mb1->capacity() );

    any = new CORBA::Any();
    value = 5.0;
    // Write from corba, read from C++
    any <<= value;
    BOOST_CHECK( buf->push( any.in() ) );
    BOOST_CHECK_EQUAL( (int)buf->size(), mb1->size() );

    BOOST_CHECK_EQUAL( 5.0, mb1->front() );
    BOOST_CHECK( mb1->Pop( value ) );
    BOOST_CHECK_EQUAL( 5.0, value );
    BOOST_CHECK_EQUAL( (int)buf->size(), mb1->size() );

    // Write from C++, read from corba
    value = -5.0;
    mb1->Push( value );
    BOOST_CHECK_EQUAL( (int)buf->size(), mb1->size() );

    any = buf->front();
    value = 0.0;
    any >>= value;
    BOOST_CHECK_EQUAL( -5.0, value );
    BOOST_CHECK( buf->pull( any.out() ) );
    value = 0.0;
    any >>= value;
    BOOST_CHECK_EQUAL( -5.0, value );
    BOOST_CHECK_EQUAL( (int)buf->size(), mb1->size() );

}

// Test the IDL connectPorts statement.
BOOST_AUTO_TEST_CASE( testConnectPortsIDL )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    // Default direction is from ts to ts2, but it will also need to
    // connect ports from ts2 to ts when ts is reader and ts2 is writer.
    BOOST_CHECK( ts->server()->connectPorts( ts2->server() ) );

    testPortStats();
    testPortDisconnect();
}

BOOST_AUTO_TEST_CASE( testConnectPortsLR )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    BOOST_CHECK( connectPorts(tc, tp2 ) );

    testPortStats();
    testPortDisconnect();

}
BOOST_AUTO_TEST_CASE( testConnectPortsRL )
{
    ts  = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp  = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    BOOST_CHECK( connectPorts(tp, t2 ) );

    testPortStats();
    testPortDisconnect();

}

BOOST_AUTO_TEST_CASE( testConnectPortsRR )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    BOOST_CHECK( connectPorts(tp, tp2 ) );

    testPortStats();
    testPortDisconnect();

}

BOOST_AUTO_TEST_CASE( testConnectPortsLRC )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    // test connecting to existing connection:
    ConnectionInterface::shared_ptr ci = md1->createConnection(ConnectionTypes::lockfree);
    BOOST_CHECK( md1->connectTo(ci) );
    BOOST_CHECK( ci->connect() );

    BOOST_CHECK( connectPorts(tc, tp2 ) );

    testPortStats();
    testPortDisconnect();

}

BOOST_AUTO_TEST_CASE( testConnectPortsRLC )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    // test connecting to existing connection:
    ConnectionInterface::shared_ptr ci = md1->createConnection(ConnectionTypes::lockfree);
    BOOST_CHECK( md1->connectTo(ci) );
    BOOST_CHECK( ci->connect() );

    BOOST_CHECK( connectPorts(tp, t2 ) );

    testPortStats();
    testPortDisconnect();

}

BOOST_AUTO_TEST_CASE( testConnectPortsRRC )
{
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    // test connecting to existing connection:
    ConnectionInterface::shared_ptr ci = md1->createConnection(ConnectionTypes::lockfree);
    BOOST_CHECK( md1->connectTo(ci) );
    BOOST_CHECK( ci->connect() );

    BOOST_CHECK( connectPorts(tp, tp2 ) );

    testPortStats();
    testPortDisconnect();

}

BOOST_AUTO_TEST_CASE( testConnections )
{
    // This test tests the differen port-to-port connections.
    ts = Corba::ControlTaskServer::Create( tc, false ); //no-naming
    tp = Corba::ControlTaskProxy::Create( ts->server(), true );
    ts2 = Corba::ControlTaskServer::Create( t2, false ); //no-naming
    tp2 = Corba::ControlTaskProxy::Create( ts2->server(), true );

    // incompatible type should fail
    BOOST_CHECK( !ts->server()->ports()->connectPorts("md", ts2->server()->ports(), "mb") );

    BOOST_CHECK( ts->server()->ports()->connectPorts("md", ts2->server()->ports(), "md") );
    BOOST_CHECK( ts->server()->ports()->connectPorts("mb", ts2->server()->ports(), "mb") );
    BOOST_CHECK( ts->server()->ports()->connectPorts("mdrwA", ts2->server()->ports(), "mdrwA") );
    BOOST_CHECK( ts->server()->ports()->connectPorts("mdrwB", ts2->server()->ports(), "mdrwB") );

    testPortStats();
    testPortDisconnect();


}

BOOST_AUTO_TEST_CASE( cleanupCorba )
{
    Corba::ControlTaskProxy::DestroyOrb();
}

BOOST_AUTO_TEST_SUITE_END()


void CorbaTest::testPortStats()
{
    // ALWAYS connect from tc/tp TO t2/tp2
    // The test assumes the connection direction is tc->t2.
    // Tests if ports are correctly working, this test is called by the other test functions.
    // DATA PORTS
    // Check if connection succeeded both ways:
    BOOST_CHECK( md1->connected() );
    BOOST_CHECK( md2->connected() );
    // Check if both ports return same initial value:
    BOOST_CHECK_EQUAL( 1.0, md1->Get() );
    BOOST_CHECK_EQUAL( 1.0, md2->Get() );

    // Check writing from both ways:
    md1->Set( 3.0 );
    BOOST_CHECK_EQUAL( 3.0, md1->Get() );
    BOOST_CHECK_EQUAL( 3.0, md2->Get() );
    md2->Set( -3.0 );
    BOOST_CHECK_EQUAL( -3.0, md1->Get() );
    BOOST_CHECK_EQUAL( -3.0, md2->Get() );

    // READ/WRITE DATA PORTS
    // Check if connection succeeded both ways:
    BOOST_CHECK( mdr1->connected() );
    BOOST_CHECK( mdr2->connected() );
    BOOST_CHECK( mdw1->connected() );
    BOOST_CHECK( mdw2->connected() );
    // Check if both ports return same initial value:
    BOOST_CHECK_EQUAL( -1.0, mdr1->Get() );
    //BOOST_CHECK_EQUAL( 1.0, mdw1->Get() );
    BOOST_CHECK_EQUAL( 1.0, mdr2->Get() );
    //BOOST_CHECK_EQUAL( -1.0, mdw2->Get() );

    // Check writing from both ways:
    mdw1->Set( 3.0 );
    //BOOST_CHECK_EQUAL( 3.0, mdw1->Get() );
    BOOST_CHECK_EQUAL( 3.0, mdr2->Get() );
    mdw2->Set( -3.0 );
    //BOOST_CHECK_EQUAL( -3.0, mdw2->Get() );
    BOOST_CHECK_EQUAL( -3.0, mdr1->Get() );

    //
    // BUFFER PORTS
    // Check if connection succeeded both ways:
    double val = 0.0;
    BOOST_CHECK( mb1->connected() );
    BOOST_CHECK( mb2->connected() );
    BOOST_CHECK_EQUAL( 10, mb2->capacity() );
    BOOST_CHECK_EQUAL( 0, mb1->size() );
    BOOST_CHECK_EQUAL( 0, mb2->size() );

    // Check writing from both ways:
    BOOST_CHECK( mb1->Push( 3.0 ) );
    BOOST_CHECK( mb1->front() == 3.0 );
    BOOST_CHECK( mb2->front() == 3.0 );
    BOOST_CHECK_EQUAL( 1, mb1->size() );
    BOOST_CHECK_EQUAL( 1, mb2->size() );
    BOOST_CHECK( mb2->Pop( val ));
    BOOST_CHECK( val == 3.0 );
    BOOST_CHECK_EQUAL( 0, mb1->size() );
    BOOST_CHECK_EQUAL( 0, mb2->size() );

    BOOST_CHECK( mb2->Push( -3.0 ) );
    BOOST_CHECK( mb1->front() == -3.0 );
    BOOST_CHECK( mb2->front() == -3.0 );
    BOOST_CHECK_EQUAL( 1, mb1->size() );
    BOOST_CHECK_EQUAL( 1, mb2->size() );
    BOOST_CHECK( mb2->Pop( val ));
    BOOST_CHECK( val == -3.0 );
    BOOST_CHECK_EQUAL( 0, mb1->size() );
    BOOST_CHECK_EQUAL( 0, mb2->size() );
}

void CorbaTest::testPortDisconnect()
{
    // Connection management.
    // DATA PORTS
    BOOST_CHECK( md1->connected() );
    BOOST_CHECK( md2->connected() );

    // store -3.0 in connection.
    md1->Set( -3.0 );

    md1->disconnect();
    md2->disconnect();

    BOOST_CHECK( !md1->connected() );
    BOOST_CHECK( !md2->connected() );

    // Check no writing from both ways:
    md1->Set( 3.0 );
    BOOST_CHECK_EQUAL( 3.0, md1->Get() );
    BOOST_CHECK( 3.0 != md2->Get() );
    md2->Set( 6.0 );
    BOOST_CHECK_EQUAL( 3.0, md1->Get() );
    BOOST_CHECK_EQUAL( 6.0, md2->Get() );

    //
    // BUFFER PORTS
    // Check if connection succeeded both ways:
    double val = 0.0;
    BOOST_CHECK( mb1->connected() );
    BOOST_CHECK( mb2->connected() );

    // Store a value.
    BOOST_CHECK( mb1->Push( 3.0 ) );

    // Disconnect
    mb1->disconnect();
    mb2->disconnect();

    // should fail
    BOOST_CHECK( !mb1->Push( 3.0 ) );
    BOOST_CHECK( !mb2->Pop( val ));

    BOOST_CHECK( !mb2->Push( -3.0 ) );
    BOOST_CHECK( !mb2->Pop( val ));
}

