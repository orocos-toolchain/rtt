
#define ORO_TEST_METHOD

#include <iostream>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#include <internal/RemoteOperationCaller.hpp>
#include <interface/Service.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

/**
 * This test suite tests the RTT::internal::RemoteOperationCaller class
 * and its dependencies, being OperationCallerC and SendHandleC.
 */
BOOST_FIXTURE_TEST_SUITE(  RemoteOperationCallerTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(testRemoteOperationCaller)
{
    OperationCaller<double(void)> m0("mo");
    boost::shared_ptr<DisposableInterface> implementation( new RemoteOperationCaller<double(void)>(tc->provides("methods")->getPart("m0"),"m0", caller->engine() ) );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    OperationCaller<double(int)> m1;
    implementation.reset( new RemoteOperationCaller<double(int)>(tc->provides("methods")->getPart("m1"),"m1", caller->engine()) );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );

    BOOST_CHECK_EQUAL(  2.0, m1(0) );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
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

    Service to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );

    // test constructor
    OperationCaller<double(void)> mm0 = to.getOperation("m0");
    BOOST_CHECK( mm0.getOperationCallerImpl() );
    BOOST_CHECK( mm0.ready() );

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

BOOST_AUTO_TEST_SUITE_END()
