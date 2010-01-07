
#include <iostream>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <internal/RemoteMethod.hpp>
#include <internal/TaskObject.hpp>

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
        tc->addObject( this->createMethodFactory() );
        tsim = new SimulationActivity(0.001, tc->engine() );
    }

    TaskContext* tc;
    ActivityInterface* tsim;

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
        tsim->stop();
        SimulationThread::Instance()->stop();
        delete tc;
        delete tsim;
    }

    TaskObject* createMethodFactory()
        {
        TaskObject* to = new TaskObject("methods");

        to->methods()->addMethod( method("assert",  &MethodTest::assertBool, this), "assert","b","bd");

        to->methods()->addMethod( method("m0",  &MethodTest::m0, this), "M0");
        to->methods()->addMethod( method("m1",  &MethodTest::m1, this), "M1","a","ad");
        to->methods()->addMethod( method("m2",  &MethodTest::m2, this), "M2","a","ad","a","ad");
        //to->methods()->addMethod( method("m3",  &MethodTest::m3, this), "M3","a","ad","a","ad","a","ad");
        //to->methods()->addMethod( method("m4",  &MethodTest::m4, this), "M4","a","ad","a","ad","a","ad","a","ad");
        return to;
        }
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  MethodTestSuite,  MethodTest )

BOOST_AUTO_TEST_CASE(testMethod)
{
    Method<double(void)> m0("m0", &MethodTest::m0, this);
    Method<double(int)> m1("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2("m2", &MethodTest::m2, this);
//    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this);
//    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this);

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
//    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, false) );
//    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

#if 0
BOOST_AUTO_TEST_CASE(testRemoteMethod)
{
    Method<double(void)> m0;
    boost::shared_ptr<ActionInterface> implementation( new detail::RemoteMethod<double(void)>(tc->getObject("methods")->methods(),"m0") );
    m0 = implementation;
    BOOST_CHECK( m0.ready() );

    Method<double(int)> m1;
    implementation.reset( new detail::RemoteMethod<double(int)>(tc->getObject("methods")->methods(),"m1") );
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
    mc = tc->getObject("methods")->methods()->create("m0").ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -1.0 );

    mc = tc->getObject("methods")->methods()->create("m2").argC(1).argC(1.0).ret( r );
    BOOST_CHECK( mc.execute() );
    BOOST_CHECK( r == -3.0 );

//    mc = tc->getObject("methods")->methods()->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
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

    Method<double(void)> m0("m0", &MethodTest::m0, this);
    Method<double(int)> m1("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2("m2", &MethodTest::m2, this);

    TaskObject to("task");

    BOOST_CHECK( to.methods()->addMethod(&m0) );
    BOOST_CHECK( ! to.methods()->addMethod(&m0) );
    BOOST_CHECK( to.methods()->addMethod(&m1) );
    BOOST_CHECK( to.methods()->addMethod(&m2) );

    // test constructor
    Method<double(void)> mm0 = to.methods()->getMethod<double(void)>("m0");
    BOOST_CHECK( mm0.getMethodImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.methods()->getMethod<double(int)>("m1");
    BOOST_CHECK( mm1.getMethodImpl() );
    BOOST_CHECK( mm1.ready() );

    Method<double(int,double)> mm2 = to.methods()->getMethod<double(int,double)>("m2");
    BOOST_CHECK( mm2.getMethodImpl() );
    BOOST_CHECK( mm2.ready() );

    // start the activity, such that methods are accepted.
    BOOST_CHECK( tsim->start()) ;
    // execute methods and check status:
    BOOST_CHECK_EQUAL( -1.0, mm0() );

    BOOST_CHECK_EQUAL( -2.0, mm1(1) );
    BOOST_CHECK_EQUAL( -3.0, mm2(1, 2.0) );

    // test error cases:
    // Add uninitialised method:
    Method<void(void)> mvoid;
    BOOST_CHECK(to.methods()->addMethod( &mvoid ) == false);
    mvoid = Method<void(void)>("voidm");
    BOOST_CHECK(to.methods()->addMethod( &mvoid ) == false);

    // wrong type 1:
    mvoid = to.methods()->getMethod<void(void)>("m1");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.methods()->getMethod<void(bool)>("m1");
    // wrong type 3:
    mvoid = to.methods()->getMethod<double(void)>("m0");
    BOOST_CHECK( mvoid.ready() == false );
    // non existing
    mvoid = to.methods()->getMethod<void(void)>("voidm");
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
    TaskObject to("task");

    Method<double(void)> m0("m0", &MethodTest::m0, this);
    Method<double(int)> m1("m1", &MethodTest::m1, this);
    Method<double(int,double)> m2("m2", &MethodTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &MethodTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &MethodTest::m4, this);

    to.methods()->addMethod( &m0, "desc");
    to.methods()->addMethod( &m1, "desc", "a1", "d1");
    to.methods()->addMethod( &m2, "desc", "a1", "d1", "a2","d2");
    to.methods()->addMethod( &m3, "desc", "a1", "d1", "a2","d2","a3","d3");
    to.methods()->addMethod( &m4, "desc", "a1", "d1", "a2","d2","a3","d3", "a4","d4");

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
    TaskObject to("task");

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Method<double(MethodTest*)> meth0("m0",
                                          &MethodTest::m0);

    method_ds("m0", &MethodTest::m0);

    Method<double(MethodTest*,int)> meth1("m1",
                                          &MethodTest::m1);

    method_ds("m1", &MethodTest::m1);
    method_ds("ms",&MethodTest::comstr );

    boost::shared_ptr<MethodTest> ptr( new MethodTest() );
    ValueDataSource<boost::weak_ptr<MethodTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<MethodTest> >( ptr );
    BOOST_CHECK( to.methods()->addMethodDS( wp.get(), meth0, "desc" ) );
    BOOST_CHECK( to.methods()->addMethodDS( wp.get(), meth1, "desc", "a1", "d1" ) );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();

    BOOST_CHECK( tsim->start()) ;

    double ret;
    MethodC c0  = to.methods()->create("m0").ret(ret);
    BOOST_CHECK( c0.execute() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    MethodC c1  = to.methods()->create("m1").argC(1).ret(ret);
    BOOST_CHECK( c1.execute() );
    BOOST_CHECK_EQUAL( -2.0, ret );

    BOOST_CHECK( tsim->stop()) ;

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
