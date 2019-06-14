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
#include <transports/corba/CorbaConnPolicy.hpp>
#include <transports/corba/RTTCorbaConversion.hpp>

#include "operations_fixture.hpp"

#include <memory>

using namespace std;
using corba::TaskContextProxy;

class CorbaTest : public OperationsFixture
{
public:
    CorbaTest() :
        pint1("pint1", "", 3), pdouble1(new Property<double>("pdouble1", "", -3.0)),
        aint1(3), adouble1(-3.0)
    {
        // check operations (moved from OperationCallerComponent constructor for reuseability in corba-ipc-server)
        BOOST_REQUIRE( caller->ready() );

        // connect DataPorts
        mi1 = new InputPort<double> ("mi");
        mo1 = new OutputPort<double> ("mo");

        mi2 = new InputPort<double> ("mi");
        mo2 = new OutputPort<double> ("mo");

        tc->ports()->addEventPort(*mi1);
        tc->ports()->addPort(*mo1);
        tc->start();

        t2 = new TaskContext("local");
        t2->ports()->addEventPort(*mi2,boost::bind(&CorbaTest::new_data_listener, this, _1));
        t2->ports()->addPort(*mo2);

        ts2 = ts = 0;
        tp2 = tp = 0;

        // store nested properties:
        tc->provides()->addProperty(pint1);
        storeProperty(*tc->provides()->properties(), "s1.s2", pdouble1 );

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
    Property<double>* pdouble1;

    int aint1;
    double adouble1;

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

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
    // This test assumes that there is a data connection mo1 => mi2
    // Check if connection succeeded both ways:
    BOOST_CHECK( mo1->connected() );
    BOOST_CHECK( mi2->connected() );

    double value = 0;

    // Check if no-data works
    BOOST_CHECK_EQUAL( mi2->read(value), NoData );

    // Check if writing works (including signalling)
    ASSERT_PORT_SIGNALLING(mo1->write(1.0), mi2);
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

template <typename T>
static void testCorbaType(const T &value = T()) {
    CORBA::Any any;
    T copy = T();

    BOOST_TEST_CHECKPOINT("Testing CORBA conversion for type " << typeid(T).name());
    BOOST_CHECK( RTT::corba::AnyConversion<T>::updateAny(value, any) );
    BOOST_CHECK( RTT::corba::AnyConversion<T>::update(any, copy) );
    BOOST_CHECK_EQUAL( copy, value );
}

template <typename T>
static void testCorbaTypeSequence(std::size_t size = 3, const T &value = T())
{
    CORBA::Any any;

    BOOST_TEST_CHECKPOINT("Testing CORBA conversion for a vector with elements of type " << typeid(T).name());
    std::vector<T> vec(size, value);
    BOOST_CHECK( RTT::corba::AnyConversion< std::vector<T> >::updateAny(vec, any) );
    BOOST_CHECK( RTT::corba::AnyConversion< std::vector<T> >::update(any, vec) );
}

namespace RTT {
    static bool operator==(const ConnPolicy &, const ConnPolicy &) { return true; }
}

BOOST_AUTO_TEST_CASE( testCorbaTypes )
{
    testCorbaType<double>(1.0);
    testCorbaTypeSequence<double>(3, 1.0);
    testCorbaType<float>(2.0);
    testCorbaTypeSequence<float>(3, 2.0);
    testCorbaType<int>(-3);
    testCorbaTypeSequence<int>(3, -3);
    testCorbaType<unsigned int>(4);
    testCorbaTypeSequence<unsigned int>(3, 4);
    testCorbaType<long long>(-9223372036854775807ll);
    testCorbaTypeSequence<long long>(3, 9223372036854775807ll);
    testCorbaType<unsigned long long>(18446744073709551615ull);
    testCorbaTypeSequence<unsigned long long>(3, 18446744073709551615ull);
    testCorbaType<bool>(true);
    testCorbaType<char>('c');
    testCorbaTypeSequence<char>(3, 'c');
    testCorbaType<std::string>("foo");
    testCorbaTypeSequence<std::string>(3, "foo");
    testCorbaType<RTT::ConnPolicy>();
#ifdef OS_RT_MALLOC
    testCorbaType<rt_string>("bar");
#endif
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

    // initial read through get:
    BOOST_CHECK_EQUAL( proxy_int.get(), 3);
    // reading through set:
    aint1 = 4;
    BOOST_CHECK_EQUAL( proxy_int.set(), 4);
    // doing remote set:
    proxy_int.set( 5 );
    BOOST_CHECK_EQUAL( aint1, 5);

    BOOST_CHECK( tp->provides()->hasAttribute("adouble1") );
    Attribute<double> proxy_double = tp->provides()->getAttribute("adouble1");
    BOOST_REQUIRE( proxy_double.ready() );

    // initial reading through set:
    BOOST_CHECK_EQUAL( proxy_double.set(), -3.0 );
    // doing remote set, check local and remote result:
    proxy_double.set( 5.0 );
    BOOST_CHECK_EQUAL( adouble1, 5.0 );
    BOOST_CHECK_EQUAL( proxy_double.get(), 5.0);
    adouble1 = 6.0;
    // reading changed remote:
    BOOST_CHECK_EQUAL( proxy_double.get(), 6.0);
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
    // initial read through get:
    BOOST_CHECK_EQUAL( proxy_int.get(), 3);
    // reading through set:
    pint1 = 4;
    BOOST_CHECK_EQUAL( proxy_int.set(), 4);
    // doing remote set:
    proxy_int.set( 5 );
    BOOST_CHECK_EQUAL( pint1, 5);

    BOOST_CHECK( findProperty( *tp->provides()->properties(), "s1.s2.pdouble1") );
    Property<double> proxy_d = findProperty( *tp->provides()->properties(), "s1.s2.pdouble1");
    BOOST_REQUIRE( proxy_d.ready() );
    // initial reading through set:
    BOOST_CHECK_EQUAL( proxy_d.set(), -3.0 );
    // doing remote set, check local and remote result:
    proxy_d.set( 5.0 );
    BOOST_CHECK_EQUAL( pdouble1->get(), 5.0 );
    BOOST_CHECK_EQUAL( proxy_d.get(), 5.0);
    pdouble1->set( 6.0 );
    // reading changed remote:
    BOOST_CHECK_EQUAL( proxy_d.get(), 6.0);
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

    mc = tp->provides("methods")->create("m0except", tc->engine() );
    BOOST_CHECK_THROW( mc.call(), std::runtime_error );
    BOOST_REQUIRE( tc->inException() );
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

#ifndef RTT_CORBA_SEND_ONEWAY_OPERATIONS
    mc = tp->provides("methods")->create("m0except", caller->engine());
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_THROW( shc.collect(), std::runtime_error);
    BOOST_REQUIRE( tc->inException() );
#endif
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
    RTT::OperationCaller<void(void)> m0e = tp->provides("methods")->getOperation("m0except");

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
    BOOST_CHECK_THROW( m0e(), std::runtime_error );
    BOOST_REQUIRE( tc->inException() );
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

    any_args = new corba::CAnyArguments(0);
    BOOST_CHECK_THROW(co->callOperation("m0except", any_args.inout() ), ::RTT::corba::CCallError);
    BOOST_REQUIRE( tc->inException() );
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
    // This test tests the different port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    ts2 = corba::TaskContextServer::Create( t2, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::data());
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = ts2->server()->ports();

    // Test cases that should not connect
    BOOST_CHECK_THROW( ports->createConnection("mo", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("does_not_exist", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("does_not_exist", ports2, "does_not_exist", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mo", ports2, "mo", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mi", ports2, "mi", policy), CNoSuchPortException );
    BOOST_CHECK_THROW( ports->createConnection("mi", ports2, "mo", policy), CNoSuchPortException );

    // must be running to catch event port signalling.
    BOOST_CHECK( t2->start() );
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
#ifndef RTT_CORBA_PORTS_DISABLE_SIGNAL
    testPortDataConnection();
#endif // RTT_CORBA_PORTS_DISABLE_SIGNAL
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
#ifndef RTT_CORBA_PORTS_DISABLE_SIGNAL
    testPortBufferConnection();
#endif // RTT_CORBA_PORTS_DISABLE_SIGNAL
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

BOOST_AUTO_TEST_CASE( testSharedConnections )
{
    // This test installs shared connections between mo1 and mo2 as writers and mi2 and mi3 as readers

//    // Add a second input port mo3 to tc
//#if __cplusplus > 199711L
//    unique_ptr<RTT::OutputPort<double> >
//#else
//    auto_ptr<RTT::OutputPort<double> >
//#endif
//            mo3(new RTT::OutputPort<double>());

//    tc->addPort("mo3", *mo3);

    // Add a second input port mi3 to t2
#if __cplusplus > 199711L
    unique_ptr<RTT::InputPort<double> >
#else
    auto_ptr<RTT::InputPort<double> >
#endif
            mi3(new RTT::InputPort<double>());
    t2->addPort("mi3", *mi3);

    // This test tests shared connections port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    ts2 = corba::TaskContextServer::Create( t2, false ); //no-naming

    // must be running to catch event port signalling.
    BOOST_CHECK( t2->start() );

    // Create a CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::data(ConnPolicy::LOCKED));
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    corba::CDataFlowInterface_var ports2 = ts2->server()->ports();
    double value = 0.0;

    // Shared push connection...
    policy.buffer_policy = RTT::corba::CShared;
    policy.pull = false;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi3", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( mi3->connected() );
    BOOST_CHECK( mo2->connected() );
    BOOST_REQUIRE( mi2->getManager()->getSharedConnection() );
    BOOST_REQUIRE( mi3->getManager()->getSharedConnection() );
    BOOST_CHECK_EQUAL( mi2->getManager()->getSharedConnection()->getName(), mi3->getManager()->getSharedConnection()->getName() );
    BOOST_CHECK_EQUAL( mi3->read(value), NoData );
    testPortDataConnection(); // communication between mo and mi should work the same as for private connections
    BOOST_CHECK_EQUAL( mi3->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 2.0 );
    BOOST_CHECK_EQUAL( mo2->write(3.0), WriteSuccess );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi3->read(value), NewData );
    BOOST_CHECK_EQUAL( value, 3.0 );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi2->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 3.0 );

    ports->disconnectPort("mo"); // disconnect from the output side
    ports2->disconnectPort("mo"); // disconnect from the output side
    BOOST_CHECK( !mo1->connected() );
    BOOST_CHECK( !mo2->connected() );
    BOOST_CHECK( !mi2->connected() );
    BOOST_CHECK( !mi3->connected() );

    // Shared pull connection...
    policy.buffer_policy = RTT::corba::CShared;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi3", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( mi3->connected() );
    BOOST_CHECK( mo2->connected() );
    BOOST_REQUIRE( mi2->getManager()->getSharedConnection() );
    BOOST_REQUIRE( mi3->getManager()->getSharedConnection() );
    BOOST_CHECK_EQUAL( mi2->getManager()->getSharedConnection()->getName(), mi3->getManager()->getSharedConnection()->getName() );
    BOOST_CHECK_EQUAL( mi3->read(value), NoData );
#ifndef RTT_CORBA_PORTS_DISABLE_SIGNAL
    testPortDataConnection(); // communication between mo and mi should work the same as for private connections
    BOOST_CHECK_EQUAL( mi3->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 2.0 );
#endif // RTT_CORBA_PORTS_DISABLE_SIGNAL
    BOOST_CHECK_EQUAL( mo2->write(3.0), WriteSuccess );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi3->read(value), NewData );
    BOOST_CHECK_EQUAL( value, 3.0 );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi2->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 3.0 );

    ports->disconnectPort("mo"); // disconnect from the output side
    ports2->disconnectPort("mo"); // disconnect from the output side
    BOOST_CHECK( !mo1->connected() );
    BOOST_CHECK( !mo2->connected() );
    BOOST_CHECK( !mi2->connected() );
    BOOST_CHECK( !mi3->connected() );

    // PerOutputPort pull connection...
    policy.buffer_policy = RTT::corba::CPerOutputPort;
    policy.pull = true;
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi", policy) );
    BOOST_CHECK( ports->createConnection("mo", ports2, "mi3", policy) );
    BOOST_CHECK( ports2->createConnection("mo", ports2, "mi", policy) ); // cannot use the DataFlowInterface CORBA API here, as it will find the local SharedConnection instance and fail.
    BOOST_CHECK( mi3->connected() );
    BOOST_CHECK( mo2->connected() );
    BOOST_CHECK( mo1->getSharedBuffer() );
    BOOST_CHECK( mo2->getSharedBuffer() );
    BOOST_CHECK_EQUAL( mi3->read(value), NoData );
#ifndef RTT_CORBA_PORTS_DISABLE_SIGNAL
    testPortDataConnection(); // communication between mo and mi should work the same as for private connections
    BOOST_CHECK_EQUAL( mi3->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 2.0 );
#endif // RTT_CORBA_PORTS_DISABLE_SIGNAL
    BOOST_CHECK_EQUAL( mo1->write(3.0), WriteSuccess );
    BOOST_CHECK_EQUAL( mo2->write(4.0), WriteSuccess );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi3->read(value), NewData );
    BOOST_CHECK_EQUAL( value, 3.0 );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi2->read(value), NewData );
    BOOST_CHECK_EQUAL( value, 4.0 );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi3->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 3.0 );
    value = 0.0;
    BOOST_CHECK_EQUAL( mi2->read(value), OldData );
    BOOST_CHECK_EQUAL( value, 4.0 );

    ports2->disconnectPort("mi"); // disconnect from the input side
    ports2->disconnectPort("mi3"); // disconnect from the input side
    BOOST_CHECK( !mo1->connected() );
    BOOST_CHECK( !mo2->connected() );
    BOOST_CHECK( !mi2->connected() );
    BOOST_CHECK( !mi3->connected() );
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
#if __cplusplus > 199711L
    unique_ptr<base::InputPortInterface>
#else
    auto_ptr<base::InputPortInterface>
#endif
            read_clone(dynamic_cast<base::InputPortInterface*>(read_port->clone()));
    BOOST_CHECK(mo2->createConnection(*read_clone));
    BOOST_CHECK(read_clone->connected());
    BOOST_CHECK(!read_port->connected());
    mo2->disconnect();
}

BOOST_AUTO_TEST_CASE( testRemotePortDisconnect )
{
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming
    tp  = corba::TaskContextProxy::Create( ts->server(), true );
    ts2  = corba::TaskContextServer::Create( t2, false ); //no-naming
    tp2  = corba::TaskContextProxy::Create( ts2->server(), true );

    // Create a default CORBA policy specification
    RTT::ConnPolicy policy = ConnPolicy::data();
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    base::PortInterface* untyped_port;

    //mi1
    untyped_port = tp->ports()->getPort("mi");
    BOOST_CHECK(untyped_port);
    base::InputPortInterface* read_port1 = dynamic_cast<base::InputPortInterface*>(tp->ports()->getPort("mi"));
    BOOST_CHECK(read_port1);

    //mi2
    untyped_port = tp2->ports()->getPort("mi");
    BOOST_CHECK(untyped_port);
    base::InputPortInterface* read_port2 = dynamic_cast<base::InputPortInterface*>(tp2->ports()->getPort("mi"));
    BOOST_CHECK(read_port2);

    //mo2
    untyped_port = tp2->ports()->getPort("mo");
    BOOST_CHECK(untyped_port);
    base::OutputPortInterface* write_port2 = dynamic_cast<base::OutputPortInterface*>(tp2->ports()->getPort("mo"));
    BOOST_CHECK(write_port2);

    //mo1
    untyped_port = tp->ports()->getPort("mo");
    BOOST_CHECK(untyped_port);
    base::OutputPortInterface* write_port1 = dynamic_cast<base::OutputPortInterface*>(tp->ports()->getPort("mo"));
    BOOST_CHECK(write_port1);

    // Just make sure 'read_port' and 'write_port' are actually proxies and not
    // the real thing
    BOOST_CHECK(dynamic_cast<corba::RemoteInputPort*>(read_port1));
    BOOST_CHECK(dynamic_cast<corba::RemoteInputPort*>(read_port2));
    BOOST_CHECK(dynamic_cast<corba::RemoteOutputPort*>(write_port1));
    BOOST_CHECK(dynamic_cast<corba::RemoteOutputPort*>(write_port2));

    //create remote connections:
    write_port2->connectTo(read_port1, policy);
    BOOST_CHECK(read_port1->connected());
    BOOST_CHECK(write_port2->connected());

    //second connection to this input port
    write_port1->connectTo(read_port1, policy);
    BOOST_CHECK(write_port1->connected());

    //disconnect single port connection (both remote), same tcs object.
    write_port1->disconnect(read_port1);
    BOOST_CHECK(read_port1->connected());
    BOOST_CHECK(!write_port1->connected());

    //disconnect single port connection, both remote, different tcs.
    write_port2->disconnect(read_port1);
    BOOST_CHECK(!read_port1->connected());

    //check disconnecting call on reader port. (build connection again beforehand).
    write_port2->connectTo(read_port1, policy);
    BOOST_CHECK(read_port1->connected());
    BOOST_CHECK(write_port2->connected());
    BOOST_CHECK(read_port1->disconnect(write_port2));
    BOOST_CHECK(!read_port1->connected());
    BOOST_CHECK(!write_port2->connected());

    //check connect and disconnect certain port, remote output to local input
    //should give false cause not supported yet!
    write_port2->connectTo(mi1, policy);
    BOOST_CHECK(mi1->connected());
    BOOST_CHECK(write_port2->connected());
    BOOST_CHECK(!write_port2->disconnect(mi1));
    mi1->disconnect(); //remove all connections works, so required for cleanup.

    //check disconnect remote input port from local output port.
    mo2->connectTo(read_port1, policy);
    BOOST_CHECK(read_port1->connected());
    BOOST_CHECK(mo2->connected());
    BOOST_CHECK(read_port1->disconnect(mo2));
    BOOST_CHECK(!mo2->connected());
    BOOST_CHECK(!read_port1->connected());

}

BOOST_AUTO_TEST_CASE( testDataHalfs )
{
    double result;
    // This test tests the differen port-to-port connections.
    ts  = corba::TaskContextServer::Create( tc, false ); //no-naming

    // Create a default CORBA policy specification
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::data());
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), CNoData );
    // Check read of new data
    mo1->write( 3.33 );
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();

    // test unbuffered Corba write --> C++ read
    cce = ports->buildChannelOutput("mi", policy);
    cce->channelReady(policy);
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
    RTT::corba::CConnPolicy policy = toCORBA(ConnPolicy::buffer(10));
    policy.init = false;
    policy.transport = ORO_CORBA_PROTOCOL_ID; // force creation of non-local connections

    corba::CDataFlowInterface_var ports  = ts->server()->ports();
    BOOST_REQUIRE( ports.in() );

    // test unbuffered C++ write --> Corba read
    policy.pull = false; // note: buildChannelInput must correct policy to pull = true (adds a buffer).
    CChannelElement_var cce = ports->buildChannelInput("mo", policy);
    CORBA::Any_var sample = new CORBA::Any();
    BOOST_REQUIRE( cce.in() );

    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), CNoData );
    // Check read of new data
    mo1->write( 6.33 );
    mo1->write( 3.33 );
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 6.33);
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), CNewData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    // Check re-read of old data.
    sample <<= 0.0;
    BOOST_CHECK_EQUAL( cce->read( sample.out(), true ), COldData );
    sample >>= result;
    BOOST_CHECK_EQUAL( result, 3.33);

    cce->disconnect();

    // test unbuffered Corba write --> C++ read
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

