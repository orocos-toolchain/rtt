
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
    Method<double(void)> m0("mo");
    boost::shared_ptr<DisposableInterface> implementation( new RemoteMethod<double(void)>(tc->provides("methods")->getPart("m0"),"m0", caller->engine() ) );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    Method<double(int)> m1;
    implementation.reset( new RemoteMethod<double(int)>(tc->provides("methods")->getPart("m1"),"m1", caller->engine()) );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );

    BOOST_CHECK_EQUAL(  2.0, m1(0) );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
}

BOOST_AUTO_TEST_CASE(testMethodsC)
{
    MethodC mc;
    double r = 0.0;
    mc = tc->provides("methods")->create("m0", caller->engine()).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK_EQUAL( r, -1.0 );

    mc = tc->provides("methods")->create("m2", caller->engine()).argC(1).argC(2.0).ret( r );
    BOOST_CHECK( mc.call() );
    BOOST_CHECK_EQUAL( r, -3.0 );

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
    ServiceProviderPtr sp = tc->provides("methods");

    double ret;
    MethodC mc0 = sp->create("m0", caller->engine() );
    mc0.ret(ret);
    MethodC mc1 = sp->create("m1", caller->engine() );
    mc1.argC(1).ret(ret);
    MethodC mc2 = sp->create("m2", caller->engine() );
    mc2.argC(1).argC(2.0).ret(ret);
    MethodC mc3 = sp->create("m3", caller->engine() );
    mc3.argC(1).argC(2.0).argC(true).ret(ret);
    MethodC mc4 = sp->create("m4", caller->engine() );
    mc4.argC(1).argC(2.0).argC(true).argC(std::string("hello")).ret(ret);

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
