/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  method_test.cpp

                        method_test.cpp -  description
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



#define ORO_TEST_OPERATION_CALLER

#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Operation.hpp>
#include <Service.hpp>

#include "unit.hpp"
#include "operations_fixture.hpp"

/**
 * This test suite tests the RTT::OperationCaller object's LocalOperationCaller implementation.
 */
BOOST_FIXTURE_TEST_SUITE(  OperationCallerTestSuite,  OperationsFixture )

BOOST_AUTO_TEST_CASE(testClientThreadOperationCaller)
{
    // Tests using no caller, no sender
    OperationCaller<double(void)> m0("m0", &OperationsFixture::m0, this);
    OperationCaller<double(int)> m1("m1", &OperationsFixture::m1, this);
    OperationCaller<double(int,double)> m2("m2", &OperationsFixture::m2, this);
    OperationCaller<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this);
    OperationCaller<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this);
    OperationCaller<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this);
    OperationCaller<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this);
    OperationCaller<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this);

    BOOST_CHECK_EQUAL( -1.0, m0() );
    BOOST_CHECK_EQUAL( -2.0, m1(1) );
    BOOST_CHECK_EQUAL( -3.0, m2(1, 2.0) );
    BOOST_CHECK_EQUAL( -4.0, m3(1, 2.0, true) );
    BOOST_CHECK_EQUAL( -5.0, m4(1, 2.0, true,"hello") );
    BOOST_CHECK_EQUAL( -6.0, m5(1, 2.0, true,"hello",5.0) );
    BOOST_CHECK_EQUAL( -7.0, m6(1, 2.0, true,"hello",5.0,'a') );
    BOOST_CHECK_EQUAL( -8.0, m7(1, 2.0, true,"hello",5.0,'a',7) );
}

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerCall)
{
    // Tests using caller and sender
    OperationCaller<double(void)> m0("m0", &OperationsFixture::m0, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int)> m1("m1", &OperationsFixture::m1, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double)> m2("m2", &OperationsFixture::m2, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, tc->engine(), caller->engine(), OwnThread);

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

BOOST_AUTO_TEST_CASE(testClientThreadOperationCallerSend)
{
    // we set the owner engine to zero and our caller engine to be able to send.
    OperationCaller<double(void)> m0("m0", &OperationsFixture::m0, this, 0, caller->engine());
    OperationCaller<double(int)> m1("m1", &OperationsFixture::m1, this, 0, caller->engine());
    OperationCaller<double(int,double)> m2("m2", &OperationsFixture::m2, this, 0, caller->engine());
    OperationCaller<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, 0, caller->engine());
    OperationCaller<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, 0, caller->engine());
    OperationCaller<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, 0, caller->engine());
    OperationCaller<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, 0, caller->engine());
    OperationCaller<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, 0, caller->engine());


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

BOOST_AUTO_TEST_CASE(testOwnThreadOperationCallerSend)
{
    OperationCaller<double(void)> m0("m0", &OperationsFixture::m0, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int)> m1("m1", &OperationsFixture::m1, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double)> m2("m2", &OperationsFixture::m2, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool)> m3("m3", &OperationsFixture::m3, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string)> m4("m4", &OperationsFixture::m4, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float)> m5("m5", &OperationsFixture::m5, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float,char)> m6("m6", &OperationsFixture::m6, this, tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(int,double,bool,std::string,float,char,unsigned int)> m7("m7", &OperationsFixture::m7, this, tc->engine(), caller->engine(), OwnThread);

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

BOOST_AUTO_TEST_CASE(testLocalOperationCallerFactory)
{
    // Test the addition of 'simple' operationCallers to the operation interface,
    // and retrieving it back in a new OperationCaller object.
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
    OperationCaller<double(void)> mm0 = to.getLocalOperation("m0");
    BOOST_CHECK( mm0.getOperationCallerImpl() );
    BOOST_CHECK( mm0.ready() );

    // test operator=()
    OperationCaller<double(int)> mm1;
    mm1 = to.getLocalOperation("m1");
    BOOST_CHECK( mm1.getOperationCallerImpl() );
    BOOST_CHECK( mm1.ready() );

    OperationCaller<double(int,double)> mm2 = to.getLocalOperation("m2");
    BOOST_CHECK( mm2.getOperationCallerImpl() );
    BOOST_CHECK( mm2.ready() );

    // execute operationCallers and check status:
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
    OperationCaller<void(void)> mvoid;
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

BOOST_AUTO_TEST_CASE(testRefAndConstRefOperationCallerCall_ClientThread)
{
    this->ret = -3.3;

    OperationCaller<double&(void)> m0r("m0r", &OperationsFixture::m0r, this, tc->engine(), caller->engine(), ClientThread );
    OperationCaller<const double&(void)> m0cr("m0cr", &OperationsFixture::m0cr, this, tc->engine(), caller->engine(), ClientThread );

    OperationCaller<double(double&)> m1r("m1r", &OperationsFixture::m1r, this, tc->engine(), caller->engine(), ClientThread );
    OperationCaller<double(const double&)> m1cr("m1cr", &OperationsFixture::m1cr, this, tc->engine(), caller->engine(), ClientThread );

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

BOOST_AUTO_TEST_CASE(testRefAndConstRefOperationCallerCall_OwnThread)
{
    this->ret = -3.3;

    OperationCaller<double&(void)> m0r("m0r", &OperationsFixture::m0r, this,tc->engine(), caller->engine(), OwnThread);
    OperationCaller<const double&(void)> m0cr("m0cr", &OperationsFixture::m0cr, this,tc->engine(), caller->engine(), OwnThread);

    OperationCaller<double(double&)> m1r("m1r", &OperationsFixture::m1r, this,tc->engine(), caller->engine(), OwnThread);
    OperationCaller<double(const double&)> m1cr("m1cr", &OperationsFixture::m1cr, this,tc->engine(), caller->engine(), OwnThread);

    BOOST_CHECK_EQUAL( -3.3, m0r() );
    BOOST_CHECK_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    BOOST_CHECK_EQUAL( 5.3*2, m1r(value) );
    BOOST_CHECK_EQUAL( 5.3*2, value );
    BOOST_CHECK_EQUAL( 5.3, m1cr(5.3) );
}

BOOST_AUTO_TEST_CASE(testDSOperationCaller)
{
    ServicePtr to (new Service("task", tc) );

    // A operationCaller of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Operation<double(shared_ptr<OperationsFixture>)> meth0("m0");
    meth0.calls( boost::bind(&OperationsFixture::m0, _1));

    //method_ds("m0", &OperationsFixture::m0);

    Operation<double(shared_ptr<OperationsFixture>,int)> meth1("m1");
    meth1.calls(boost::bind(&OperationsFixture::m1, _1,_2));

    //method_ds("m1", &OperationsFixture::m1);
    //method_ds("ms",&OperationsFixture::comstr );

    boost::shared_ptr<OperationsFixture> ptr( new OperationsFixture() );
    ValueDataSource<boost::shared_ptr<OperationsFixture> >::shared_ptr wp = new ValueDataSource<boost::shared_ptr<OperationsFixture> >( ptr );
    BOOST_CHECK( to->addOperationDS( wp.get(), meth0).doc("desc" ).ready() );
    BOOST_CHECK( to->addOperationDS( wp.get(), meth1).doc("desc").arg("a1", "d1" ).ready() );

    // this actually works ! the operationCaller will detect the deleted pointer.
    //ptr.reset();

    double ret;
    OperationCallerC c0  = to->create("m0", tc->engine()).ret(ret);
    BOOST_CHECK( c0.call() );
    BOOST_CHECK_EQUAL( -1.0, ret );
    OperationCallerC c1  = to->create("m1", tc->engine()).argC(1).ret(ret);
    BOOST_CHECK( c1.call() );
    BOOST_CHECK_EQUAL( -2.0, ret );

}

BOOST_AUTO_TEST_SUITE_END()
