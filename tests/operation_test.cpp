#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <rtt-fwd.hpp>
#include <rtt/Operation.hpp>
#include <rtt/interface/ServiceProvider.hpp>
#include <rtt/Method.hpp>

using namespace std;
using namespace RTT::detail;
using namespace RTT;

/**
 * Test fixture.
 */
class OperationTest
{
public:
    OperationTest() : tc("TC"), op0r("op0r"), op0cr("op0cr"), op1r("op1r"), op1cr("op1cr"), op0("op0"), op1("op1"), op2("op2"), op3("op3"), op4("op4") {

        tc.provides()->addOperation("op0", &OperationTest::func0, this);
        tc.provides()->addOperation("op1", &OperationTest::func1, this);
        BOOST_CHECK( s->getOperation<double(void)>("op0") );
        BOOST_CHECK( s->getOperation<double(int)>("op1") );

     }

    ~OperationTest() {

    }

    double& func0r(void) { return ret; }
    const double& func0cr(void) { return ret; }

    double func1r(double& a) { a = 2*a; return a; }
    double func1cr(const double& a) { return a; }

    // plain argument tests:
    double func0() { return 1.0; }
    double func1(int i) { return 2.0; }
    double func2(int i, double d) { return 3.0; }
    double func3(int i, double d, bool c) { return 4.0; }
    double func4(int i, double d, bool c, std::string s) { return 5.0; }

    // The return values of signals are intentionally distinct than these above.
    void sig0() { return sig=-1.0; }
    double sig1(int i) { return (sig=-2.0); }
    void sig2(int i, double d) { sig=-3.0; }
    double sig3(int i, double d, bool c) { return (sig=-4.0); }
    void sig4(int i, double d, bool c, std::string s) { return sig=-5.0; }

    TaskContext tc;

    // check value for signals
    double sig;

    Operation<double&(void)> op0r;
    Operation<const double&(void)> op0cr;
    Operation<double(double&)> op1r;
    Operation<double&(const double&)> op1cr;
    Operation<double(void)> op0;
    Operation<double(int)> op1;
    Operation<double(int,double)> op2;
    Operation<double(int,double,bool)> op3;
    Operation<double(int,double,bool, std::string)> op4;
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  OperationTestSuite,  OperationTest )

// Test default properties of an operation.
BOOST_AUTO_TEST_CASE( testOperationCreate )
{
    // name
    BOOST_CHECK( op0.getName() == op0 );
    // not in interface, not ready.
    BOOST_CHECK( op0.ready() == false);

}

// Test adding and calling an operation (internal API)
BOOST_AUTO_TEST_CASE( testOperationCall )
{
    ServiceProvider* s =  new ServiceProvider("Service");

    tc.provides( s );

    op0.calls( &OperationTest::func0, this, s);
    op1.calls( &OperationTest::func1, this, s);
    op2.calls( &OperationTest::func2, this, s);
    op3.calls( &OperationTest::func3, this, s);
    op4.calls( &OperationTest::func4, this, s);

    s->provides( op0 );
    s->provides( op1 );
    s->provides( op2 );
    s->provides( op3 );
    s->provides( op4 );

    // Test calling an operation using a Method.
    Method<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = op0;
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

// Test calling an operation (user API)
BOOST_AUTO_TEST_CASE( testOperationCall2 )
{
    ServiceProvider* s =  new ServiceProvider("Service");

    tc.provides( s );

    s->addOperation("op0", &OperationTest::func0, this);
    s->addOperation("op1", &OperationTest::func1, this);
    s->addOperation("op2", &OperationTest::func2, this);
    s->addOperation("op3", &OperationTest::func3, this);
    s->addOperation("op4", &OperationTest::func4, this);

    // Test calling an operation using a Method.
    Method<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = s->getOperation<double(void)>("op0");
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

// Test adding an operation to the default service.
BOOST_AUTO_TEST_CASE( testOperationAdd )
{
    tc.provides()->addOperation("top0", &OperationTest::func0, this);
    tc.provides()->addOperation("top1", &OperationTest::func1, this);
    BOOST_CHECK( s->getOperation<double(void)>("top0") );
    BOOST_CHECK( s->getOperation<double(int)>("top1") );
}

// Test calling an operation of the default service.
BOOST_AUTO_TEST_CASE( testOperationCall0 )
{
    // Test calling an operation using a Method.
    Method<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = s->getOperation<double(void)>("op0");
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

// Test adding and signalling an operation to the default service.
BOOST_AUTO_TEST_CASE( testOperationSignal )
{
    // invoke sig0 according to call/thread specification.
    op0.signals(&OperationTest::sig0, this);

    // Test calling an operation using a Method.
    Method<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = s->getOperation<double(void)>("op0");
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
    BOOST_CHECK_EQUAL( -1.0, sig );
}


BOOST_AUTO_TEST_SUITE_END()

