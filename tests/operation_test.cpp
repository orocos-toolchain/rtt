/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  operation_test.cpp

                        operation_test.cpp -  description
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


#include "tests/unit.hpp"

#include "rtt/rtt-fwd.hpp"
#include "rtt/Operation.hpp"
#include "rtt/Service.hpp"
#include "rtt/OperationCaller.hpp"
#include "rtt/TaskContext.hpp"

using namespace std;
using namespace RTT::detail;
using namespace RTT;

/**
 * Tests The RTT::Operation and OperationCaller objects and its
 * interaction with the Service class. Only
 * tests the local use, not the remoting or datasource side.
 */
class OperationTest
{
public:
    OperationTest() : tc("TC"),
    op0r("op0r"), op0cr("op0cr"), op1r("op1r"), op1cr("op1cr"), op0("op0"), op1("op1"), op2("op2"), op3("op3"), op4("op4"), op5("op5"),
    opc0r("op0r"), opc0cr("op0cr"), opc1r("op1r"), opc1cr("op1cr"), opc0("op0"), opc1("op1"), opc2("op2"), opc3("op3"), opc4("op4"), opc5("op5")
    {

        tc.provides()->addOperation("op", &OperationTest::func, this);
        tc.provides()->addOperation("op0", &OperationTest::func0, this);
        tc.provides()->addOperation("op1", &OperationTest::func1, this);
        tc.provides()->addOperation("op2", &OperationTest::func2, this);
        tc.provides()->addOperation("op3", &OperationTest::func3, this);
        tc.provides()->addOperation("op4", &OperationTest::func4, this);
        tc.provides()->addOperation("op5", &OperationTest::func5, this);
        BOOST_CHECK( tc.provides()->getOperation("op0") );
        BOOST_CHECK( tc.provides()->getOperation("op1") );
        BOOST_CHECK( tc.provides()->getOperation("op2") );
        BOOST_CHECK( tc.provides()->getOperation("op3") );
        BOOST_CHECK( tc.provides()->getOperation("op4") );
        BOOST_CHECK( tc.provides()->getOperation("op5") );

        tc.provides()->addOperation("op0r", &OperationTest::func0r, this);

        tc.provides()->addOperation("op1r", &OperationTest::func1r, this);
        tc.provides()->addOperation("op1cr", &OperationTest::func1cr, this);
     }

    ~OperationTest() {

    }
    double ret;
    double& func0r(void) { return ret; }
    const double& func0cr(void) { return ret; }

    double func1r(double& a) { a = 2*a; return a; }
    double func1cr(const double& a) { return a; }

    // plain argument tests:
    void   func() { return; }
    double func0() { return 1.0; }
    double func1(int i) { return 2.0; }
    double func2(int i, double d) { return 3.0; }
    double func3(int i, double d, bool c) { return 4.0; }
    double func4(int i, double d, bool c, std::string s) { return 5.0; }
    double func5(int i, double d, bool c, std::string s, float f) { return 6.0; }

    // The return values of signals are intentionally distinct than these above.
    double sig0() { return sig=-1.0; }
    double sig1(int i) { return (sig=-2.0); }
    double sig2(int i, double d) { return sig=-3.0; }
    double sig3(int i, double d, bool c) { return (sig=-4.0); }
    double sig4(int i, double d, bool c, std::string s) { return sig=-5.0; }
    double sig5(int i, double d, bool c, std::string s, float f) { return sig=-6.0; }

    static double freefunc0(void) { return 1.0; }
    static double freefunc1(int i) { return 2.0; }

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
    Operation<double(int,double,bool, std::string, float)> op5;

    OperationCaller<double&(void)> opc0r;
    OperationCaller<const double&(void)> opc0cr;
    OperationCaller<double(double&)> opc1r;
    OperationCaller<double&(const double&)> opc1cr;
    OperationCaller<double(void)> opc0;
    OperationCaller<double(int)> opc1;
    OperationCaller<double(int,double)> opc2;
    OperationCaller<double(int,double,bool)> opc3;
    OperationCaller<double(int,double,bool, std::string)> opc4;
    OperationCaller<double(int,double,bool, std::string, float)> opc5;
};

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  OperationTestSuite,  OperationTest )

// Test default properties of an operation.
BOOST_AUTO_TEST_CASE( testOperationCreate )
{
    // name
    BOOST_CHECK( op0.getName() == "op0" );
    // not in interface, not ready.
    BOOST_CHECK( op0.ready() == false);

}

// Test adding and calling an operation (internal API)
BOOST_AUTO_TEST_CASE( testOperationCall )
{
    Service::shared_ptr s =  boost::make_shared<Service>("Service");

    tc.provides()->addService( s );

    op0.calls( &OperationTest::func0, this);
    op1.calls( &OperationTest::func1, this);
    op2.calls( &OperationTest::func2, this);
    op3.calls( &OperationTest::func3, this);
    op4.calls( &OperationTest::func4, this);
    op5.calls( &OperationTest::func5, this);

    s->addOperation( op0 );
    s->addOperation( op1 );
    s->addOperation( op2 );
    s->addOperation( op3 );
    s->addOperation( op4 );
    s->addOperation( op5 );

    // Test calling an operation using a OperationCaller.
    OperationCaller<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = op0.getImplementation();
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

// Test calling an operation (user API)
BOOST_AUTO_TEST_CASE( testOperationCall2 )
{
    Service::shared_ptr s =  boost::make_shared<Service>("Service");

    tc.provides()->addService( s );

    s->addOperation("op0", &OperationTest::func0, this);
    s->addOperation("op1", &OperationTest::func1, this);
    s->addOperation("op2", &OperationTest::func2, this);
    s->addOperation("op3", &OperationTest::func3, this);
    s->addOperation("op4", &OperationTest::func4, this);
    s->addOperation("op5", &OperationTest::func5, this);

    // Test calling an operation using a OperationCaller.
    OperationCaller<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = s->getOperation("op0");
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

// Test adding a C++ function to the services.
BOOST_AUTO_TEST_CASE( testOperationAddCpp )
{
    // Add to custom service:
    Service::shared_ptr s =  boost::make_shared<Service>("Service");
    tc.provides()->addService( s );
    s->addOperation("top0", &OperationTest::func0, this);
    s->addOperation("top1", &OperationTest::func1, this);
    BOOST_CHECK( s->getOperation("top0") );
    BOOST_CHECK( s->getOperation("top1") );
    opc0 = s->getOperation("top0");
    opc1 = s->getOperation("top1");
    BOOST_CHECK( opc0.ready() );
    BOOST_CHECK( opc1.ready() );
    BOOST_CHECK_EQUAL(opc0(), 1.0);
    BOOST_CHECK_EQUAL(opc1(1), 2.0);

    // Add to default service:
    tc.addOperation("tcop0", &OperationTest::func0, this);
    tc.addOperation("tcop1", &OperationTest::func1, this);
    BOOST_CHECK( tc.provides()->getOperation("tcop0") );
    BOOST_CHECK( tc.provides()->getOperation("tcop1") );
    opc0 = tc.provides()->getOperation("tcop0");
    opc1 = tc.provides()->getOperation("tcop1");
    BOOST_CHECK( opc0.ready() );
    BOOST_CHECK( opc1.ready() );
    BOOST_CHECK_EQUAL(opc0(), 1.0);
    BOOST_CHECK_EQUAL(opc1(1), 2.0);

    // Override existing ops:
    // note: we add different signature with same name to check if new op was used
    tc.addOperation("tcop0", &OperationTest::func1, this);
    tc.addOperation("tcop1", &OperationTest::func2, this);
    BOOST_CHECK( tc.provides()->getOperation("tcop0") );
    BOOST_CHECK( tc.provides()->getOperation("tcop1") );
    opc1 = tc.provides()->getOperation("tcop0");
    opc2 = tc.provides()->getOperation("tcop1");
    BOOST_CHECK( opc1.ready() );
    BOOST_CHECK( opc2.ready() );
    BOOST_CHECK_EQUAL(opc1(1), 2.0);
    BOOST_CHECK_EQUAL(opc2(1,2.0), 3.0);
}

// Test adding a C function to the services.
BOOST_AUTO_TEST_CASE( testOperationAddC )
{
    // Using custom Service
    Service::shared_ptr s =  boost::make_shared<Service>("Service");
    tc.provides()->addService( s );
    s->addOperation("top0", &OperationTest::freefunc0);
    s->addOperation("top1", &OperationTest::freefunc1);
    BOOST_CHECK( s->getOperation("top0") );
    BOOST_CHECK( s->getOperation("top1") );
    opc0 = s->getOperation("top0");
    opc1 = s->getOperation("top1");
    BOOST_CHECK( opc0.ready() );
    BOOST_CHECK( opc1.ready() );
    BOOST_CHECK_EQUAL(opc0(), 1.0);
    BOOST_CHECK_EQUAL(opc1(1), 2.0);

    // Using TaskContext API
    tc.addOperation("top0", &OperationTest::freefunc0);
    tc.addOperation("top1", &OperationTest::freefunc1);
    BOOST_CHECK( tc.getOperation("top0") );
    BOOST_CHECK( tc.getOperation("top1") );
    opc0 = tc.getOperation("top0");
    opc1 = tc.getOperation("top1");
    BOOST_CHECK( opc0.ready() );
    BOOST_CHECK( opc1.ready() );
    BOOST_CHECK_EQUAL(opc0(), 1.0);
    BOOST_CHECK_EQUAL(opc1(1), 2.0);
}

// Test calling an operation of the default service.
BOOST_AUTO_TEST_CASE( testOperationCall0 )
{
    // Test calling an operation using a OperationCaller.
    OperationCaller<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = tc.getOperation("op0");
    BOOST_CHECK( m0.ready() );
    BOOST_CHECK_EQUAL( 1.0, m0.call() );
}

#ifdef ORO_SIGNALLING_OPERATIONS
// Test adding and signalling an operation without an implementation
BOOST_AUTO_TEST_CASE( testOperationSignal )
{
    // invoke sig0 according to call/thread specification.
    Handle h = op0.signals(&OperationTest::sig0, this);

    BOOST_CHECK( h.connected() );
    // Test signal when calling a method:
    OperationCaller<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = op0.getImplementation();
    BOOST_CHECK( m0.ready() );

    try {
        m0.call(); // return unspecified number (no implementation!).
    } catch (SendStatus sf) {
        BOOST_FAIL("Should not throw a SendFailure!");
    }

    BOOST_CHECK_EQUAL( -1.0, sig );

    // Test direct calling of op.signal
    sig = 0.0;
    op0.signal->emit();
    BOOST_CHECK_EQUAL( -1.0, sig );
}

BOOST_AUTO_TEST_CASE( testOperationCallAndSignal )
{
    // invoke sig0 according to call/thread specification.
    Handle h = op0.signals(&OperationTest::sig0, this);
    op0.calls(&OperationTest::func0, this);

    BOOST_CHECK( h.connected() );
    // Test signal when calling a method:
    OperationCaller<double(void)> m0("op0");
    BOOST_CHECK( m0.ready() == false );
    m0 = op0.getImplementation();
    BOOST_CHECK( m0.ready() );

    BOOST_CHECK_EQUAL( 1.0, m0.call() ); // return result of func0().

    BOOST_CHECK_EQUAL( -1.0, sig );

    // Test direct calling of op.signal
    sig = 0.0;
    op0.signal->emit();
    BOOST_CHECK_EQUAL( -1.0, sig );
}
#endif
BOOST_AUTO_TEST_SUITE_END()

