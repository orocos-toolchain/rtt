/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  service_test.cpp

                        service_test.cpp -  description
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



#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#include <Service.hpp>
#include <ServiceRequester.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

// Registers the suite into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ServiceTestSuite,  OperationsFixture )

/**
 * This test suite is very similar to method_test, but uses the provides
 * interface instead of the direct function pointers.
 */

BOOST_AUTO_TEST_CASE(testClientThreadCall)
{
    // Tests using no caller with methods (collect is not used).
    OperationCaller<double(void)> m0("m0");
    m0 = tc->provides("methods");
    OperationCaller<double(int)> m1("m1", tc->provides("methods"));
    OperationCaller<double(int,double)> m2( tc->provides("methods")->getOperation("m2") );
    OperationCaller<double(int,double,bool)> m3("m3", tc->provides("methods"));
    OperationCaller<double(int,double,bool,std::string)> m4("m4", tc->provides("methods"));

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );

    // Tests using a caller
    m0.setCaller( caller->engine() );
    m1.setCaller( caller->engine() );
    m2.setCaller( caller->engine() );
    m3.setCaller( caller->engine() );
    m4.setCaller( caller->engine() );

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );

}

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerComponentCall)
{
    // test using initial caller
    OperationCaller<double(void)> o0("o0", caller->engine() );
    o0 = tc->provides("methods");
    OperationCaller<double(int)> o1("o1", tc->provides("methods"), caller->engine() );
    OperationCaller<double(int,double)> o2( tc->provides("methods")->getOperation("o2"), caller->engine() );
    OperationCaller<double(int,double,bool)> o3("o3", tc->provides("methods"), caller->engine() );

    BOOST_REQUIRE( tc->isRunning() );

    BOOST_REQUIRE( o0.ready() );
    BOOST_REQUIRE( o1.ready() );
    BOOST_REQUIRE( o2.ready() );
    BOOST_REQUIRE( o3.ready() );

    BOOST_CHECK_EQUAL( -1.0, o0() );
    BOOST_CHECK_EQUAL( -2.0, o1(1) );
    BOOST_CHECK_EQUAL( -3.0, o2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, o3(1, 2.0, true) );

    // stress test sending operations while the receiving component is being
    // started and stopped:
    SendHandle<double(void)> sh0 = o0.send();
    SendHandle<double(int)>  sh1 = o1.send(1);
    SendHandle<double(int,double)>  sh2 = o2.send(1,2.0);
    SendHandle<double(int,double,bool)>  sh3 = o3.send(1,2.0,true);
    double foo;
    while ( sh0.collectIfDone(foo) != SendSuccess || sh1.collectIfDone(foo) != SendSuccess || sh2.collectIfDone(foo) != SendSuccess || sh3.collectIfDone(foo) != SendSuccess ) {
        cout << "doing it " << endl;
        tc->stop();
        tc->start();
    }
    // Tests setting the caller
    o0.setCaller( caller->engine() );
    o1.setCaller( caller->engine() );
    o2.setCaller( caller->engine() );
    o3.setCaller( caller->engine() );

    sh0 = o0.send();
    sh1 = o1.send(1);
    sh2 = o2.send(1,2.0);
    sh3 = o3.send(1,2.0,true);
    while ( sh0.collectIfDone(foo) != SendSuccess || sh1.collectIfDone(foo) != SendSuccess || sh2.collectIfDone(foo) != SendSuccess || sh3.collectIfDone(foo) != SendSuccess ) {
        cout << "doing it " << endl;
        tc->stop();
        tc->start();
    }

    BOOST_REQUIRE( caller->isRunning() );
    BOOST_CHECK_EQUAL( -1.0, o0() );
    BOOST_CHECK_EQUAL( -2.0, o1(1) );
    BOOST_CHECK_EQUAL( -3.0, o2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, o3(1, 2.0, true) );
}

BOOST_AUTO_TEST_CASE(testClientThreadOperationCallerSend)
{
    // test using initial caller
    OperationCaller<double(void)> m0("m0", caller->engine() );
    m0 = tc->provides("methods");
    OperationCaller<double(int)> m1("m1", tc->provides("methods"), caller->engine() );
    OperationCaller<double(int,double)> m2( tc->provides("methods")->getOperation("m2"), caller->engine() );
    OperationCaller<double(int,double,bool)> m3("m3", tc->provides("methods"), caller->engine() );
    OperationCaller<double(int,double,bool,std::string)> m4("m4", tc->provides("methods"), caller->engine() );

    BOOST_REQUIRE( tc->isRunning() );
    SendHandle<double(void)> h0 = m0.send();
    SendHandle<double(int)> h1 = m1.send(1);
    SendHandle<double(int,double)> h2 = m2.send(1, 2.0);
    SendHandle<double(int,double,bool)> h3 = m3.send(1, 2.0, true);
    SendHandle<double(int,double,bool,std::string)> h4 = m4.send(1, 2.0, true,"hello");

    double retn=0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // collectIfDone will certainly succeed after collect
    BOOST_CHECK_EQUAL( SendSuccess, h0.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );

    // Tests using a caller
    m0.setCaller( caller->engine() );
    m1.setCaller( caller->engine() );
    m2.setCaller( caller->engine() );
    m3.setCaller( caller->engine() );
    m4.setCaller( caller->engine() );

    BOOST_REQUIRE( caller->isRunning() );
    BOOST_REQUIRE( tc->isRunning() );
    h0 = m0.send();
    h1 = m1.send(1);
    h2 = m2.send(1, 2.0);
    h3 = m3.send(1, 2.0, true);
    h4 = m4.send(1, 2.0, true,"hello");

    retn=0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // collectIfDone will certainly succeed after collect
    BOOST_CHECK_EQUAL( SendSuccess, h0.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
}

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerSend)
{
    // test using initial caller
    OperationCaller<double(void)> m0("o0", caller->engine() );
    m0 = tc->provides("methods");
    OperationCaller<double(int)> m1("o1", tc->provides("methods"), caller->engine() );
    OperationCaller<double(int,double)> m2( tc->provides("methods")->getOperation("o2"), caller->engine() );
    OperationCaller<double(int,double,bool)> m3("o3", tc->provides("methods"), caller->engine() );
    OperationCaller<double(int,double,bool,std::string)> m4("o4", tc->provides("methods"), caller->engine() );

    BOOST_REQUIRE( tc->isRunning() );
    SendHandle<double(void)> h0 = m0.send();
    SendHandle<double(int)> h1 = m1.send(1);
    SendHandle<double(int,double)> h2 = m2.send(1, 2.0);
    SendHandle<double(int,double,bool)> h3 = m3.send(1, 2.0, true);
    SendHandle<double(int,double,bool,std::string)> h4 = m4.send(1, 2.0, true,"hello");

    double retn=0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // collectIfDone will certainly succeed after collect
    BOOST_CHECK_EQUAL( SendSuccess, h0.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );

    // Tests using a caller
    m0.setCaller( caller->engine() );
    m1.setCaller( caller->engine() );
    m2.setCaller( caller->engine() );
    m3.setCaller( caller->engine() );
    m4.setCaller( caller->engine() );

    BOOST_REQUIRE( caller->isRunning() );
    BOOST_REQUIRE( tc->isRunning() );
    h0 = m0.send();
    h1 = m1.send(1);
    h2 = m2.send(1, 2.0);
    h3 = m3.send(1, 2.0, true);
    h4 = m4.send(1, 2.0, true,"hello");

    retn=0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // collectIfDone will certainly succeed after collect
    BOOST_CHECK_EQUAL( SendSuccess, h0.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h1.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -2.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h2.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -3.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h3.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -4.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h4.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -5.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );

}

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerSend_ChangePolicy)
{
    // Tests changing the policy later on
    OperationCaller<double(void)> m0("o0", caller->engine() );

    // set to ClientThread and send/collect it:
    tc->provides("methods")->setOperationThread("o0",ClientThread);
    m0 = tc->provides("methods");

    BOOST_REQUIRE( tc->isRunning() );
    SendHandle<double(void)> h0 = m0.send();

    double retn=0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );

    // set to OwnThread and send/collect it:
    tc->provides("methods")->setOperationThread("o0",OwnThread);
    m0 = tc->provides("methods");

    BOOST_REQUIRE( tc->isRunning() );
    h0 = m0.send();

    retn = 0;
    BOOST_CHECK_EQUAL( SendSuccess, h0.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -1.0 );
}

BOOST_AUTO_TEST_SUITE_END()
