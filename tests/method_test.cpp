
#define ORO_TEST_METHOD

#include <TaskContext.hpp>
#include <Method.hpp>
#include <Operation.hpp>
#include <interface/Service.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

/**
 * This test suite tests the RTT::Method object's LocalMethod implementation.
 */
BOOST_FIXTURE_TEST_SUITE(  MethodTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(testClientThreadMethod)
{
    // Tests using no caller, no sender
    Method<double(void)> m0("m0", &OperationsFixture::m0, this);
    Method<double(int)> m1("m1", &OperationsFixture::m1, this);
    Method<double(int,double)> m2("m2", &OperationsFixture::m2, this);
    Method<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this);
    Method<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this);
    Method<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this);
    Method<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this);

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, m5(1, 2.0, true,"hello",5.0) );
    BOOST_CHECK_EQUAL( -7.0, m6(1, 2.0, true,"hello",5.0,'a') );
    BOOST_CHECK_EQUAL( -8.0, m7(1, 2.0, true,"hello",5.0,'a',7) );
}

BOOST_AUTO_TEST_CASE(testOwnThreadMethodCall)
{
    // Tests using caller and sender
    Method<double(void)> m0("m0", &OperationsFixture::m0, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int)> m1("m1", &OperationsFixture::m1, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double)> m2("m2", &OperationsFixture::m2, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, tc->engine(), caller->engine(), OwnThread);

    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, m5(1, 2.0, true,"hello",5.0) );
    BOOST_CHECK_EQUAL( -7.0, m6(1, 2.0, true,"hello",5.0,'a') );
    BOOST_CHECK_EQUAL( -8.0, m7(1, 2.0, true,"hello",5.0,'a',7) );
}

BOOST_AUTO_TEST_CASE(testClientThreadMethodSend)
{
    // we set the owner engine to zero and our caller engine to be able to send.
    Method<double(void)> m0("m0", &OperationsFixture::m0, this, 0, caller->engine());
    Method<double(int)> m1("m1", &OperationsFixture::m1, this, 0, caller->engine());
    Method<double(int,double)> m2("m2", &OperationsFixture::m2, this, 0, caller->engine());
    Method<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, 0, caller->engine());
    Method<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, 0, caller->engine());
    Method<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, 0, caller->engine());
    Method<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, 0, caller->engine());
    Method<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, 0, caller->engine());


    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    SendHandle<double(void)> h0 = m0.send();
    SendHandle<double(int)> h1 = m1.send(1);
    SendHandle<double(int,double)> h2 = m2.send(1, 2.0);
    SendHandle<double(int,double,bool)> h3 = m3.send(1, 2.0, true);
    SendHandle<double(int,double,bool,std::string)> h4 = m4.send(1, 2.0, true,"hello");
    SendHandle<double(int,double,bool,std::string,float)> h5 = m5.send(1, 2.0, true,"hello",5.0);
    SendHandle<double(int,double,bool,std::string,float,char)> h6 = m6.send(1, 2.0, true,"hello",5.0,'a');
    SendHandle<double(int,double,bool,std::string,float,char,unsigned int)> h7 = m7.send(1, 2.0, true,"hello",5.0,'a',7);

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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );

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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );

    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, h5.ret(1, 2.0, true,"hello",5.0) );
    BOOST_CHECK_EQUAL( -7.0, h6.ret(1, 2.0, true,"hello",5.0,'a') );
    BOOST_CHECK_EQUAL( -8.0, h7.ret(1, 2.0, true,"hello",5.0,'a',7) );

    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
    BOOST_CHECK_EQUAL( -6.0, h5.ret() );
    BOOST_CHECK_EQUAL( -7.0, h6.ret() );
    BOOST_CHECK_EQUAL( -8.0, h7.ret() );
}

BOOST_AUTO_TEST_CASE(testOwnThreadMethodSend)
{
    Method<double(void)> m0("m0", &OperationsFixture::m0, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int)> m1("m1", &OperationsFixture::m1, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double)> m2("m2", &OperationsFixture::m2, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, tc->engine(), caller->engine(), OwnThread);
    Method<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, tc->engine(), caller->engine(), OwnThread);

    BOOST_REQUIRE( tc->isRunning() );
    BOOST_REQUIRE( caller->isRunning() );
    SendHandle<double(void)> h0 = m0.send();
    SendHandle<double(int)> h1 = m1.send(1);
    SendHandle<double(int,double)> h2 = m2.send(1, 2.0);
    SendHandle<double(int,double,bool)> h3 = m3.send(1, 2.0, true);
    SendHandle<double(int,double,bool,std::string)> h4 = m4.send(1, 2.0, true,"hello");
    SendHandle<double(int,double,bool,std::string,float)> h5 = m5.send(1, 2.0, true,"hello",5.0);
    SendHandle<double(int,double,bool,std::string,float,char)> h6 = m6.send(1, 2.0, true,"hello",5.0,'a');
    SendHandle<double(int,double,bool,std::string,float,char,unsigned int)> h7 = m7.send(1, 2.0, true,"hello",5.0,'a',7);


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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collect(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );

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
    BOOST_CHECK_EQUAL( SendSuccess, h5.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -6.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h6.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -7.0 );
    BOOST_CHECK_EQUAL( SendSuccess, h7.collectIfDone(retn) );
    BOOST_CHECK_EQUAL( retn, -8.0 );


    // the return value api.
    BOOST_CHECK_EQUAL( -1.0, h0.ret() );
    BOOST_CHECK_EQUAL( -2.0, h1.ret(1) );
    BOOST_CHECK_EQUAL( -3.0, h2.ret(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, h3.ret(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, h4.ret(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, h5.ret(1, 2.0, true,"hello",5.0) );
    BOOST_CHECK_EQUAL( -7.0, h6.ret(1, 2.0, true,"hello",5.0,'a') );
    BOOST_CHECK_EQUAL( -8.0, h7.ret(1, 2.0, true,"hello",5.0,'a',7) );

    BOOST_CHECK_EQUAL( -2.0, h1.ret() );
    BOOST_CHECK_EQUAL( -3.0, h2.ret() );
    BOOST_CHECK_EQUAL( -4.0, h3.ret() );
    BOOST_CHECK_EQUAL( -5.0, h4.ret() );
    BOOST_CHECK_EQUAL( -6.0, h5.ret() );
    BOOST_CHECK_EQUAL( -7.0, h6.ret() );
    BOOST_CHECK_EQUAL( -8.0, h7.ret() );
}

BOOST_AUTO_TEST_CASE(testLocalMethodFactory)
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new Method object.
    // local operations do not use the remoting facility.

    Operation<double(void)> m0("m0");
    m0.calls(&OperationsFixture::m0, this);
    Operation<double(int)> m1("m1");
    m1.calls(&OperationsFixture::m1, this);
    Operation<double(int,double)> m2("m2");
    m2.calls(&OperationsFixture::m2, this);
    BOOST_CHECK( !m0.ready() );
    BOOST_CHECK( !m1.ready() );
    BOOST_CHECK( !m2.ready() );

    Service to("task");

    // allow to add an operation even if no owner is set.
    BOOST_CHECK( to.addLocalOperation(m0) );
    BOOST_CHECK( !m0.ready() );
    to.setOwner(tc);
    BOOST_CHECK( m0.ready() );

    // Overriding and adding:
    BOOST_CHECK( to.addLocalOperation(m0) );
    BOOST_CHECK( to.addLocalOperation(m0) );
    BOOST_CHECK( to.addLocalOperation(m1) );
    BOOST_CHECK( to.addLocalOperation(m2) );

    // test constructor
    Method<double(void)> mm0 = to.getLocalOperation("m0");
    BOOST_CHECK( mm0.getMethodImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.getLocalOperation("m1");
    BOOST_CHECK( mm1.getMethodImpl() );
    BOOST_CHECK( mm1.ready() );

    Method<double(int,double)> mm2 = to.getLocalOperation("m2");
    BOOST_CHECK( mm2.getMethodImpl() );
    BOOST_CHECK( mm2.ready() );

    // execute methods and check status:
    BOOST_CHECK_EQUAL( -1.0, mm0() );

    BOOST_CHECK_EQUAL( -2.0, mm1(1) );
    BOOST_CHECK_EQUAL( -3.0, mm2(1, 2.0) );

    // test error cases:
    // Add uninitialised op:
    Operation<void(void)> ovoid("ovoid");
    BOOST_CHECK(to.addLocalOperation( ovoid ) == true);
    ovoid = Operation<void(void)>("ovoid");
    BOOST_CHECK(to.addLocalOperation( ovoid ) == true);

    // wrong type 1:
    Method<void(void)> mvoid;
    mvoid = to.getLocalOperation("m1");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.getLocalOperation("m2");
    BOOST_CHECK( mvoid.ready() == false );
    // wrong type 3:
    mvoid = to.getLocalOperation("m0");
    BOOST_CHECK( mvoid.ready() == false );
    // non existing
    mvoid = to.getLocalOperation("voidm");
    BOOST_CHECK( mvoid.ready() == false );

    // this line may not crash:
    mvoid();

}

BOOST_AUTO_TEST_CASE(testRefAndConstRefMethodCall_ClientThread)
{
    this->ret = -3.3;

    Method<double&(void)> m0r("m0r", &OperationsFixture::m0r, this, tc->engine(), caller->engine(), ClientThread );
    Method<const double&(void)> m0cr("m0cr", &OperationsFixture::m0cr, this, tc->engine(), caller->engine(), ClientThread );

    Method<double(double&)> m1r("m1r", &OperationsFixture::m1r, this, tc->engine(), caller->engine(), ClientThread );
    Method<double(const double&)> m1cr("m1cr", &OperationsFixture::m1cr, this, tc->engine(), caller->engine(), ClientThread );

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

BOOST_AUTO_TEST_CASE(testRefAndConstRefMethodCall_OwnThread)
{
    this->ret = -3.3;

    Method<double&(void)> m0r("m0r", &OperationsFixture::m0r, this,tc->engine(), caller->engine(), OwnThread);
    Method<const double&(void)> m0cr("m0cr", &OperationsFixture::m0cr, this,tc->engine(), caller->engine(), OwnThread);

    Method<double(double&)> m1r("m1r", &OperationsFixture::m1r, this,tc->engine(), caller->engine(), OwnThread);
    Method<double(const double&)> m1cr("m1cr", &OperationsFixture::m1cr, this,tc->engine(), caller->engine(), OwnThread);

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

BOOST_AUTO_TEST_CASE(testDSMethod)
{
    ServicePtr to (new Service("task", tc) );

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Operation<double(OperationsFixture*)> meth0("m0");
    meth0.calls( boost::bind(&OperationsFixture::m0, _1));

    //method_ds("m0", &OperationsFixture::m0);

    Operation<double(OperationsFixture*,int)> meth1("m1");
    meth1.calls(boost::bind(&OperationsFixture::m1, _1,_2));

    //method_ds("m1", &OperationsFixture::m1);
    //method_ds("ms",&OperationsFixture::comstr );

    boost::shared_ptr<OperationsFixture> ptr( new OperationsFixture() );
    ValueDataSource<boost::shared_ptr<OperationsFixture> >::shared_ptr wp = new ValueDataSource<boost::shared_ptr<OperationsFixture> >( ptr );
    BOOST_CHECK( to->addOperationDS( wp.get(), meth0).doc("desc" ).ready() );
    BOOST_CHECK( to->addOperationDS( wp.get(), meth1).doc("desc").arg("a1", "d1" ).ready() );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();

    double ret;
    MethodC c0  = to->create("m0", tc->engine()).ret(ret);
    BOOST_CHECK( c0.call() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    MethodC c1  = to->create("m1", tc->engine()).argC(1).ret(ret);
    BOOST_CHECK( c1.call() );
    BOOST_CHECK_EQUAL( -2.0, ret );

}

BOOST_AUTO_TEST_SUITE_END()
