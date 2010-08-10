
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#include <interface/Service.hpp>
#include <interface/ServiceRequester.hpp>

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
    // Tests using no caller
    Method<double(void)> m0("m0");
    m0 = tc->provides("methods");
    Method<double(int)> m1("m1", tc->provides("methods"));
    Method<double(int,double)> m2( tc->provides("methods")->getOperation("m2") );
    Method<double(int,double,bool)> m3("m3", tc->provides("methods"));
    Method<double(int,double,bool,std::string)> m4("m4", tc->provides("methods"));

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

BOOST_AUTO_TEST_CASE(testOwnThreadMethodCall)
{
    // Tests using no caller
    Method<double(void)> m0("o0");
    m0 = tc->provides("methods");
    Method<double(int)> m1("o1", tc->provides("methods"));
    Method<double(int,double)> m2( tc->provides("methods")->getOperation("o2") );
    Method<double(int,double,bool)> m3("o3", tc->provides("methods"));
    Method<double(int,double,bool,std::string)> m4("o4", tc->provides("methods"));

    BOOST_REQUIRE( tc->isRunning() );
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

    BOOST_REQUIRE( caller->isRunning() );
    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
}

BOOST_AUTO_TEST_CASE(testClientThreadMethodSend)
{
    // test using no caller
    Method<double(void)> m0("m0");
    m0 = tc->provides("methods");
    Method<double(int)> m1("m1", tc->provides("methods"));
    Method<double(int,double)> m2( tc->provides("methods")->getOperation("m2") );
    Method<double(int,double,bool)> m3("m3", tc->provides("methods"));
    Method<double(int,double,bool,std::string)> m4("m4", tc->provides("methods"));

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

BOOST_AUTO_TEST_CASE(testOwnThreadMethodSend)
{
    // Tests using no caller
    Method<double(void)> m0("o0");
    m0 = tc->provides("methods");
    Method<double(int)> m1("o1", tc->provides("methods"));
    Method<double(int,double)> m2( tc->provides("methods")->getOperation("o2") );
    Method<double(int,double,bool)> m3("o3", tc->provides("methods"));
    Method<double(int,double,bool,std::string)> m4("o4", tc->provides("methods"));

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

BOOST_AUTO_TEST_SUITE_END()
