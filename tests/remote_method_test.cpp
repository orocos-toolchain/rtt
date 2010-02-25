
#define ORO_TEST_METHOD

#include <iostream>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#ifdef ORO_REMOTING
#include <internal/RemoteMethod.hpp>
#endif
#include <interface/ServiceProvider.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  RemoteMethodTestSuite,  OperationsFixture )

#ifdef ORO_REMOTING
BOOST_AUTO_TEST_CASE(testRemoteMethod)
{
    Method<double(void)> m0;
    boost::shared_ptr<ActionInterface> implementation( new detail::RemoteMethod<double(void)>(tc->provides("methods"),"m0") );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    Method<double(int)> m1;
    implementation.reset( new detail::RemoteMethod<double(int)>(tc->provides("methods"),"m1") );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );

    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
}

BOOST_AUTO_TEST_CASE(testMethodsC)
{
    MethodC mc;
    double r = 0.0;
    mc = tc->provides("methods")->create("m0", tc->engine()).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -1.0 );

    mc = tc->provides("methods")->create("m2", tc->engine()).argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK( r == -3.0 );

//    mc = tc->provides("methods")->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
//    BOOST_CHECK( mc.call() );
//    BOOST_CHECK( r == -4.0 );

#if 0
        +" set r = methods.m0()\n"
        +" do methods.assert( r == -1.0 )\n"
        +" set r = methods.m1(1)\n"
        +" do methods.assert( r == -2.0 )\n"
        +" set r = methods.m2(1, 1.0)\n"
        +" do methods.assert( r == -3.0 )\n"
        +" set r = methods.m3(1, 1.0, true)\n"
        +" do methods.assert( r == -4.0 )\n"
        +" set r = methods.m4(1, 1.0, true, \"hello\")\n"
        +" do methods.assert( r == -5.0 )\n"
#endif
}

BOOST_AUTO_TEST_CASE(testMethodFromDS)
{
    ServiceProvider to("task");

    Method<double(void)> m0("m0", &OperationsFixture::m0, this);
    Method<double(int)> m1("m1", &OperationsFixture::m1, this);
    Method<double(int,double)> m2("m2", &OperationsFixture::m2, this);
    Method<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this);

    to.addOperation( &m0, "desc");
    to.addOperation( &m1, "desc", "a1", "d1");
    to.addOperation( &m2, "desc", "a1", "d1", "a2","d2");
    to.addOperation( &m3, "desc", "a1", "d1", "a2","d2","a3","d3");
    to.addOperation( &m4, "desc", "a1", "d1", "a2","d2","a3","d3", "a4","d4");

    double ret;
    MethodC mc0( to.methods(), "m0");
    mc0.ret(ret);
    MethodC mc1( to.methods(), "m1");
    mc1.argC(1).ret(ret);
    MethodC mc2( to.methods(), "m2");
    mc2.argC(1).argC(2.0).ret(ret);
    MethodC mc3( to.methods(), "m3");
    mc3.argC(1).argC(2.0).argC(false).ret(ret);
    MethodC mc4( to.methods(), "m4");
    mc4.argC(1).argC(2.0).argC(false).argC(std::string("hello")).ret(ret);

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
}
#endif

BOOST_AUTO_TEST_SUITE_END()
