
#include <iostream>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#include <internal/RemoteMethod.hpp>
#include <interface/ServiceProvider.hpp>

#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

class MethodTest {
public:
    MethodTest()
    {
        tc =  new TaskContext( "root" );
        tc->addService( this->createMethodFactory() );
        caller = new TaskContext("caller");
        caller->start();
        tc->start();
    }

    TaskContext* tc, *caller;

    // ref/const-ref tests:
    double ret;
    double& m0r() { return ret; }
    const double& m0cr() { return ret; }

    // test const std::string& argument for command_ds
    bool comstr(const std::string& cs) { return !cs.empty(); }

    double m1r(double& a) { a = 2*a; return a; }
    double m1cr(const double& a) { return a; }

    // plain argument tests:
    double m0() { return -d0(); }
    double m1(int i) { return -d1(i); }
    double m2(int i, double d) { return -d2(i,d); }
    double m3(int i, double d, bool c) { return -d3(i,d,c); }
    double m4(int i, double d, bool c, std::string s) { return -d4(i,d,c,s); }

    double d0() const { return 1.0; }
    double d1(int i) const { return 2.0; }
    double d2(int i, double d) const { return 3.0; }
    double d3(int i, double d, bool c) const { return 4.0; }
    double d4(int i, double d, bool c, std::string s) const { return 5.0; }

    bool assertBool(bool b) { return b; }

    ~MethodTest()
    {
        //     if ( tc->getPeer("programs") )
        //         delete tc->getPeer("programs");
        tc->stop();
        SimulationThread::Instance()->stop();
        delete tc;
        delete  caller;
    }

    ServiceProvider* createMethodFactory()
        {
        ServiceProvider* to = new ServiceProvider("methods");

        to->addOperation("assert", &MethodTest::assertBool, this).doc("assert").arg("b", "bd");

        to->addOperation("m0", &MethodTest::m0, this).doc("M0");
        to->addOperation("m1", &MethodTest::m1, this).doc("M1").arg("a", "ad");
        to->addOperation("m2", &MethodTest::m2, this).doc("M2").arg("a", "ad").arg("a", "ad");
        //to->addOperation("m3", &MethodTest::m3, this).doc("M3").arg("a", "ad").arg("a", "ad").arg("a", "ad");
        //to->addOperation("m4", &MethodTest::m4, this).doc("M4").arg("a", "ad").arg("a", "ad").arg("a", "ad").arg("a", "ad");
        return to;
        }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MethodTestSuite,  MethodTest )

BOOST_AUTO_TEST_CASE(testClientThreadMethod)
{
    Method<double(void)> m0("m0", &MethodTest::m0, this);
    Method<double(int)> m1("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2("m2", &MethodTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this);

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

BOOST_AUTO_TEST_CASE(testOwnThreadMethodCall)
{
    Method<double(void)> m0("m0", &MethodTest::m0, this, tc->engine(), caller->engine());
    Method<double(int)> m1("m1", &MethodTest::m1, this, tc->engine(), caller->engine());
    Method<double(int,double)> m2("m2", &MethodTest::m2, this, tc->engine(), caller->engine());
    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this);

    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

BOOST_AUTO_TEST_CASE(testOwnThreadMethodSend)
{
    Method<double(void)> m0("m0", &MethodTest::m0, this, tc->engine(), caller->engine());
    Method<double(int)> m1("m1", &MethodTest::m1, this, tc->engine(), caller->engine());
    Method<double(int,double)> m2("m2", &MethodTest::m2, this, tc->engine(), caller->engine());
    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this, tc->engine(), caller->engine());
    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this, tc->engine(), caller->engine());

    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    SendHandle<double(void)> h0 = m0.send();
    SendHandle<double(int)> h1 = m1.send(1);
    SendHandle<double(int,double)> h2 = m2.send(1, 2.0);
    SendHandle<double(int,double,bool)> h3 = m3.send(1, 2.0, false);
    SendHandle<double(int,double,bool,std::string)> h4 = m4.send(1, 2.0, false,"hello");

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
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, false,"hello") );
    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
}

#if 0
BOOST_AUTO_TEST_CASE(testRemoteMethod)
{
    Method<double(void)> m0;
    boost::shared_ptr<ActionInterface> implementation( new detail::RemoteMethod<double(void)>(tc->provides("methods")->methods(),"m0") );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    Method<double(int)> m1;
    implementation.reset( new detail::RemoteMethod<double(int)>(tc->provides("methods")->methods(),"m1") );
    m1 = implementation;
    BOOST_CHECK( m1.ready() );

    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -1.0, m0() );
}
#endif

BOOST_AUTO_TEST_CASE(testMethodsC)
{
    MethodC mc;
    double r = 0.0;
    mc = tc->provides("methods")->create("m0").ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -1.0 );

    mc = tc->provides("methods")->create("m2").argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -3.0 );

//    mc = tc->provides("methods")->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
//    BOOST_CHECK( mc.execute() );
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

BOOST_AUTO_TEST_CASE(testMethodFactory)
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new Method object.

    Operation<double(void)> m0("m0");
    m0.calls(&MethodTest::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&MethodTest::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&MethodTest::m2, this);

    ServiceProvider to("task");

    BOOST_CHECK( !to.addOperation(m0).ready() );
    to.setOwner(tc);
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m0).ready() );
    BOOST_CHECK( to.addOperation(m1).ready() );
    BOOST_CHECK( to.addOperation(m2).ready() );

    // test constructor
    Method<double(void)> mm0 = to.getOperation<double(void)>("m0");
    BOOST_CHECK( mm0.getMethodImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.getOperation<double(int)>("m1");
    BOOST_CHECK( mm1.getMethodImpl() );
    BOOST_CHECK( mm1.ready() );

    Method<double(int,double)> mm2 = to.getOperation<double(int,double)>("m2");
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
    mvoid = to.getOperation<void(void)>("m1");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.getOperation<void(bool)>("m1");
    // wrong type 3:
    mvoid = to.getOperation<double(void)>("m0");
    BOOST_CHECK( mvoid.ready() == false );
    // non existing
    mvoid = to.getOperation<void(void)>("voidm");
    BOOST_CHECK( mvoid.ready() == false );

    // this line may not crash:
    mvoid();

}

BOOST_AUTO_TEST_CASE(testCRMethod)
{
    this->ret = -3.3;

    Method<double&(void)> m0r("m0r", &MethodTest::m0r, this);
    Method<const double&(void)> m0cr("m0cr", &MethodTest::m0cr, this);

    Method<double(double&)> m1r("m1r", &MethodTest::m1r, this);
    Method<double(const double&)> m1cr("m1cr", &MethodTest::m1cr, this);

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

#if 0
BOOST_AUTO_TEST_CASE(testMethodFromDS)
{
    ServiceProvider to("task");

    Method<double(void)> m0("m0", &MethodTest::m0, this);
    Method<double(int)> m1("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2("m2", &MethodTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this);

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

    BOOST_CHECK( mc0.execute() );
    BOOST_CHECK_EQUAL(-1.0, ret);
    BOOST_CHECK( mc1.execute() );
    BOOST_CHECK_EQUAL(-2.0, ret);
    BOOST_CHECK( mc2.execute() );
    BOOST_CHECK_EQUAL(-3.0, ret);
    BOOST_CHECK( mc3.execute() );
    BOOST_CHECK_EQUAL(-4.0, ret);
    BOOST_CHECK( mc4.execute() );
    BOOST_CHECK_EQUAL(-5.0, ret);
}
#endif

BOOST_AUTO_TEST_CASE(testDSMethod)
{
    ServiceProvider to("task");

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Operation<double(MethodTest*)> meth0("m0");
    meth0.calls( boost::bind(&MethodTest::m0, _1));

    //method_ds("m0", &MethodTest::m0);

    Operation<double(MethodTest*,int)> meth1("m1");
    meth1.calls(boost::bind(&MethodTest::m1, _1,_2));

    //method_ds("m1", &MethodTest::m1);
    //method_ds("ms",&MethodTest::comstr );

    boost::shared_ptr<MethodTest> ptr( new MethodTest() );
    ValueDataSource<boost::weak_ptr<MethodTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<MethodTest> >( ptr );
    BOOST_CHECK( to.addOperationDS( wp.get(), meth0).doc("desc" ).ready() );
    BOOST_CHECK( to.addOperationDS( wp.get(), meth1).doc("desc").arg("a1", "d1" ).ready() );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();

    double ret;
    MethodC c0  = to.create("m0").ret(ret);
    BOOST_CHECK( c0.execute() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    MethodC c1  = to.create("m1").argC(1).ret(ret);
    BOOST_CHECK( c1.execute() );
    BOOST_CHECK_EQUAL( -2.0, ret );

}

#if 0
BOOST_AUTO_TEST_CASE(testAddMethod)
{
    Method<double(void)> m0 = method("m0", &MethodTest::m0, this);

    Method<double(int)> m1 = method("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2 = method("m2", &MethodTest::m2, this);
    Method<double(int,double,bool)> m3 = method("m3", &MethodTest::m3, this);
    Method<double(int,double,bool,std::string)> m4 = method("m4", &MethodTest::m4, this);

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}
#endif

BOOST_AUTO_TEST_SUITE_END()
