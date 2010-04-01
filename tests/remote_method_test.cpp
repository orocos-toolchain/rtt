
#define ORO_TEST_METHOD

#include <iostream>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#include <internal/RemoteMethod.hpp>
#include <interface/ServiceProvider.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

/**
 * This test suite tests the RTT::internal::RemoteMethod class
 * and its dependencies, being MethodC and SendHandleC.
 */
BOOST_FIXTURE_TEST_SUITE(  RemoteMethodTestSuite,  OperationsFixture )

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

BOOST_AUTO_TEST_CASE(testMethodC_Call)
{
    MethodC mc;
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
}

BOOST_AUTO_TEST_CASE(testMethodC_Send)
{
    MethodC mc;
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

BOOST_AUTO_TEST_CASE(testRemoteMethodFactory)
{
    // Test the addition of methods to the operation interface,
    // and retrieving it back in a new Method object.
    // these operations may use the remoting facility to adapt

    Operation<double(void)> m0("m0");
    m0.calls(&OperationsFixture::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&OperationsFixture::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&OperationsFixture::m2, this);

    ServiceProvider to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );

    // test constructor
    Method<double(void)> mm0 = to.getOperation("m0");
    BOOST_CHECK( mm0.getMethodImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.getOperation("m1");
    BOOST_CHECK( mm1.getMethodImpl() );
    BOOST_CHECK( mm1.ready() );

    Method<double(int,double)> mm2 = to.getOperation("m2");
    BOOST_CHECK( mm2.getMethodImpl() );
    BOOST_CHECK( mm2.ready() );

    // execute methods and check status:
    BOOST_CHECK_EQUAL( -1.0, mm0() );

    BOOST_CHECK_EQUAL( -2.0, mm1(1) );
    BOOST_CHECK_EQUAL( -3.0, mm2(1, 2.0) );

    // test error cases:
    // Add uninitialised op:
    Operation<void(void)> ovoid("voidm");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == false);
    ovoid = Operation<void(void)>("voidm");
    BOOST_CHECK(to.addOperation( ovoid ).ready() == false);

    // wrong type 1:
    Method<void(void)> mvoid;
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

BOOST_AUTO_TEST_SUITE_END()
