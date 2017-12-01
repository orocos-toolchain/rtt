/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  remote_method_test.cpp

                        remote_method_test.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#define ORO_TEST_OPERATION_CALLER

#include <iostream>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#include <internal/RemoteOperationCaller.hpp>
#include <Service.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

#define wait_for_equal( a, b, times ) do { \
    bool wait_for_helper; \
    int wait = 0; \
    while( (wait_for_helper = ((a) != (b))) && wait++ != times ) \
        usleep(100000); \
    if (wait_for_helper) BOOST_CHECK_EQUAL( a, b ); \
} while(0)

/**
 * This test suite tests the RTT::internal::RemoteOperationCaller class
 * and its dependencies, being OperationCallerC and SendHandleC.
 */
BOOST_FIXTURE_TEST_SUITE(  RemoteOperationCallerTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(testRemoteOperationCaller)
{
    OperationCaller<double(void)> m0("mo");
    boost::shared_ptr<DisposableInterface> implementation( new RemoteOperationCaller<double(void)>(tc->provides("methods")->getPart("m0"),"m0", 0 ) );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    OperationCaller<double(int)> m1;
    implementation.reset( new RemoteOperationCaller<double(int)>(tc->provides("methods")->getPart("m1"),"m1", 0) );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );
    
    OperationCaller<void(void)> m0e;
    implementation.reset( new RemoteOperationCaller<void(void)>(tc->provides("methods")->getPart("m0except"),"m0except", 0) );
    m0e = implementation;
    BOOST_CHECK( m0e.ready() );

    BOOST_CHECK_EQUAL(  2.0, m1(0) );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_THROW( m0e(), std::runtime_error);
    BOOST_REQUIRE( tc->inException() );
}

BOOST_AUTO_TEST_CASE(testOperationCallerC_Call)
{
    OperationCallerC mc;
    double r = 0.0;
    mc = tc->provides("methods")->create("m0", caller->engine()).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK_EQUAL( r, -1.0 );

    mc = tc->provides("methods")->create("m2", caller->engine()).argC(1).argC(2.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK_EQUAL( r, -3.0 );

    mc = tc->provides("methods")->create("m3", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true);
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -4.0 );

    mc = tc->provides("methods")->create("m4", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello"));
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -5.0 );

    mc = tc->provides("methods")->create("m5", caller->engine()).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -6.0 );

    mc = tc->provides("methods")->create("m6", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f).argC('a');
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -7.0 );

    mc = tc->provides("methods")->create("m7", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f).argC('a').argC((unsigned int)7);
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -8.0 );
    
    mc = tc->provides("methods")->create("m0except", caller->engine());
    BOOST_CHECK_THROW( mc.call(), std::runtime_error);
 
    mc = tc->provides("methods")->create("o0except", caller->engine());
    BOOST_CHECK_THROW( mc.call(), std::runtime_error);
    BOOST_REQUIRE( tc->inException() );
}

BOOST_AUTO_TEST_CASE(testOperationCallerC_Send)
{
    OperationCallerC mc;
    SendHandleC shc;
    double r = 0.0;
    double cr = 0.0;
    mc = tc->provides("methods")->create("m0", caller->engine()).ret( r );
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -1.0 );

    mc = tc->provides("methods")->create("m2", caller->engine()).argC(1).argC(2.0).ret( r );
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -3.0 );

    mc = tc->provides("methods")->create("m3", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true);
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -4.0 );

    mc = tc->provides("methods")->create("m4", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello"));
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -5.0 );

    mc = tc->provides("methods")->create("m5", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f);
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -6.0 );

    mc = tc->provides("methods")->create("m6", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f).argC('a');
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -7.0 );

    mc = tc->provides("methods")->create("m7", caller->engine()).ret( r ).argC(1).argC(2.0).argC(true).argC(string("hello")).argC(5.0f).argC('a').argC((unsigned int)7);
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    shc.arg(cr);
    BOOST_CHECK( shc.ready() ); // 1 argument to collect.
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_EQUAL( shc.collect(), SendSuccess);
    BOOST_CHECK_EQUAL( r, 0.0 );
    BOOST_CHECK_EQUAL( cr, -8.0 );

    mc = tc->provides("methods")->create("m0except", caller->engine());
    BOOST_CHECK_NO_THROW( mc.check() );
    shc = mc.send();
    BOOST_CHECK( shc.ready() );
    BOOST_CHECK_NO_THROW( shc.check() );
    // now collect:
    BOOST_CHECK_THROW( shc.collect(), std::runtime_error );
    BOOST_REQUIRE(tc->inException() );
}

BOOST_AUTO_TEST_CASE(testOperationCallerFromDS)
{
    ServicePtr sp = tc->provides("methods");

    double ret;
    OperationCallerC mc0 = sp->create("m0", caller->engine() );
    mc0.ret(ret);
    OperationCallerC mc1 = sp->create("m1", caller->engine() );
    mc1.argC(1).ret(ret);
    OperationCallerC mc2 = sp->create("m2", caller->engine() );
    mc2.argC(1).argC(2.0).ret(ret);
    OperationCallerC mc3 = sp->create("m3", caller->engine() );
    mc3.argC(1).argC(2.0).argC(true).ret(ret);
    OperationCallerC mc4 = sp->create("m4", caller->engine() );
    mc4.argC(1).argC(2.0).argC(true).argC(std::string("hello")).ret(ret);
    OperationCallerC mc5 = sp->create("m5", caller->engine() );
    mc5.argC(1).argC(2.0).argC(true).argC(std::string("hello")).argC(5.0f).ret(ret);
    OperationCallerC mc6 = sp->create("m6", caller->engine() );
    mc6.argC(1).argC(2.0).argC(true).argC(std::string("hello")).argC(5.0f).argC('a').ret(ret);
    OperationCallerC mc7 = sp->create("m7", caller->engine() );
    mc7.argC(1).argC(2.0).argC(true).argC(std::string("hello")).argC(5.0f).argC('a').argC((unsigned int)7).ret(ret);

    OperationCallerC mc0e = sp->create("m0except", caller->engine() );

    BOOST_CHECK( mc0.call() );
    BOOST_CHECK_EQUAL(-1.0, ret);
    BOOST_CHECK( mc1.call() );
    BOOST_CHECK_EQUAL(-2.0, ret);
    BOOST_CHECK( mc2.call() );
    BOOST_CHECK_EQUAL(-3.0, ret);
    BOOST_CHECK( mc3.call() );
    BOOST_CHECK_EQUAL(-4.0, ret);
    BOOST_CHECK( mc4.call() );
    BOOST_CHECK_EQUAL(-5.0, ret);
    BOOST_CHECK( mc5.call() );
    BOOST_CHECK_EQUAL(-6.0, ret);
    BOOST_CHECK( mc6.call() );
    BOOST_CHECK_EQUAL(-7.0, ret);
    BOOST_CHECK( mc7.call() );
    BOOST_CHECK_EQUAL(-8.0, ret);

    BOOST_CHECK_THROW( mc0e.call(), std::runtime_error );
    BOOST_REQUIRE(tc->inException() );
}

BOOST_AUTO_TEST_CASE(testRemoteOperationCallerFactory)
{
    // Test the addition of methods to the operation interface,
    // and retrieving it back in a new OperationCaller object.
    // these operations may use the remoting facility to adapt

    Operation<double(void)> m0("m0");
    m0.calls(&OperationsFixture::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&OperationsFixture::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&OperationsFixture::m2, this);
    Operation<void(void)> m0e("m0e");
    m0e.calls(&OperationsFixture::m0except, this);

    Service to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );
    BOOST_CHECK( to.addOperation(m0e).ready() );

    // test constructor
    OperationCaller<double(void)> mm0 = to.getOperation("m0");
    BOOST_CHECK( mm0.getOperationCallerImpl() );
    BOOST_CHECK( mm0.ready() );

    OperationCaller<void(void)> mm0e = to.getOperation("m0e");
    BOOST_CHECK( mm0e.getOperationCallerImpl() );
    BOOST_CHECK( mm0e.ready() );

    // test operator=()
    OperationCaller<double(int)> mm1;
    mm1 = to.getOperation("m1");
    BOOST_CHECK( mm1.getOperationCallerImpl() );
    BOOST_CHECK( mm1.ready() );

    OperationCaller<double(int,double)> mm2 = to.getOperation("m2");
    BOOST_CHECK( mm2.getOperationCallerImpl() );
    BOOST_CHECK( mm2.ready() );

    // execute methods and check status:
    BOOST_CHECK_EQUAL( -1.0, mm0() );

    BOOST_CHECK_EQUAL( -2.0, mm1(1) );
    BOOST_CHECK_EQUAL( -3.0, mm2(1, 2.0) );

    BOOST_CHECK_THROW( mm0e(), std::runtime_error );

    // Add uninitialised op:
    Operation<void(void)> ovoid("ovoid");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == true);
    ovoid = Operation<void(void)>("ovoid");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == true);

    // wrong type 1:
    OperationCaller<void(void)> mvoid;
    mvoid = to.getOperation("m1");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.getOperation("m2");
    BOOST_CHECK( mvoid.ready() == false );
    // auto-converts double to void:
    mvoid = to.getOperation("m0");
    BOOST_CHECK( mvoid.ready() == true );
    // non existing
    mvoid = to.getOperation("voidm");
    BOOST_CHECK( mvoid.ready() == false );

    // this line may not crash:
    mvoid();
}

BOOST_AUTO_TEST_CASE(testRemoteOperationCallerSleep)
{
    OperationCaller<int(int)> sleepAndIncOwnThread("sleepAndIncOwnThread", caller->engine());

    boost::shared_ptr<DisposableInterface> implementation( new RemoteOperationCaller<int(int)>(tc->provides("methods")->getPart("sleepAndIncOwnThread"), "sleepAndIncOwnThread", 0) );
    sleepAndIncOwnThread = implementation;
    BOOST_CHECK( sleepAndIncOwnThread.ready() );

    i = 0;
    sleepAndIncOwnThread.call(1);
    sleepAndIncOwnThread.call(1);
    sleepAndIncOwnThread.call(1);
    BOOST_CHECK_EQUAL( 3, i );

    i = 0;
    sleepAndIncOwnThread.send(1);
    sleepAndIncOwnThread.send(1);
    sleepAndIncOwnThread.send(1);
    wait_for_equal(3, i, 50 /* * 100ms */ );
}

BOOST_AUTO_TEST_CASE(testRemoteOperationCallerReferences)
{
    OperationCaller<const void *(const int&)> returnAddressOfConst("returnAddressOfConst", caller->engine());
    OperationCaller<void *(int&)> returnAddressOf("returnAddressOf", caller->engine());

    boost::shared_ptr<DisposableInterface> implementation( new RemoteOperationCaller<const void *(const int&)>(tc->provides("methods")->getPart("returnAddressOfConst"), "returnAddressOfConst", 0) );
    returnAddressOfConst = implementation;
    BOOST_CHECK( returnAddressOfConst.ready() );

    implementation.reset( new RemoteOperationCaller<void *(int&)>(tc->provides("methods")->getPart("returnAddressOf"), "returnAddressOf", 0) );
    returnAddressOf = implementation;
    BOOST_CHECK( returnAddressOf.ready() );

    int var = 0;

    // call
    BOOST_CHECK_EQUAL(&var, returnAddressOf(var));
    BOOST_CHECK_EQUAL(&var, returnAddressOfConst(var));

    // send
    // Collecting from a RemoteOperationCaller returning void * segfaults!
    if (false) {
        SendHandle<void *(int&)> sh = returnAddressOf.send(var);
        BOOST_CHECK_EQUAL(SendSuccess, sh.collect());
        BOOST_CHECK_EQUAL(&var, sh.ret());
    }

    // compare with LocalOperationCaller...
    // call
    returnAddressOfConst = tc->provides("methods")->getPart("returnAddressOfConst");
    returnAddressOf = tc->provides("methods")->getPart("returnAddressOf");
    BOOST_CHECK_EQUAL(&var, returnAddressOf(var));
    BOOST_CHECK_EQUAL(&var, returnAddressOfConst(var));

    // send
    {
        SendHandle<void *(int&)> sh = returnAddressOf.send(var);
        BOOST_CHECK_EQUAL(SendSuccess, sh.collect());
        BOOST_CHECK_EQUAL(&var, sh.ret());
    }

    // call by value
    OperationCaller<const void *(int)> returnAddressOfByValue("returnAddressOfByValue", caller->engine());
    returnAddressOfByValue = tc->provides("methods")->getPart("returnAddressOfConst");
    BOOST_CHECK(returnAddressOfByValue.ready());

    BOOST_CHECK_NE(&var, returnAddressOfByValue(var));
}

BOOST_AUTO_TEST_SUITE_END()
