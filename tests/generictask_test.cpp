/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp 

                        generictask_test.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#include "generictask_test.hpp"
#include <unistd.h>
#include <iostream>
#include <FunctionGraph.hpp>
#include <Ports.hpp>
#include <Method.hpp>
#include <OperationInterface.hpp>

#include <SimulationActivity.hpp>
#include <SimulationThread.hpp>

#include <boost/function_types/function_type_signature.hpp>

#include <rtt-config.h>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( Generic_TaskTest );


void 
Generic_TaskTest::setUp()
{
    tc =  new TaskContext( "root" );
    tsim = new SimulationActivity(0.001, tc->engine() );
}


void 
Generic_TaskTest::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tsim->stop();
    SimulationThread::Instance()->stop();
    delete tc;
    delete tsim;
}

bool Generic_TaskTest::assertBool( bool b) { 
    return b;
}

TaskObject* Generic_TaskTest::createMethodFactory()
{
    TaskObject* to = new TaskObject("methods");

    to->methods()->addMethod( method("assert",  &Generic_TaskTest::assertBool, this), "assert","b","bd");

    to->methods()->addMethod( method("m0",  &Generic_TaskTest::m0, this), "M0");
    to->methods()->addMethod( method("m1",  &Generic_TaskTest::m1, this), "M1","a","ad");
    to->methods()->addMethod( method("m2",  &Generic_TaskTest::m2, this), "M2","a","ad","a","ad");
    to->methods()->addMethod( method("m3",  &Generic_TaskTest::m3, this), "M3","a","ad","a","ad","a","ad");
    to->methods()->addMethod( method("m4",  &Generic_TaskTest::m4, this), "M4","a","ad","a","ad","a","ad","a","ad");
    return to;
}

void Generic_TaskTest::testMethodsC()
{
    MethodC mc;
    double r = 0.0;
    mc = tc->getObject("methods")->methods()->create("m0").ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -1.0 );

    mc = tc->getObject("methods")->methods()->create("m2").argC(1).argC(1.0).ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -3.0 );

    mc = tc->getObject("methods")->methods()->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -4.0 );
    
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

void Generic_TaskTest::testMethod()
{
    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &Generic_TaskTest::m4, this);

    CPPUNIT_ASSERT_EQUAL( -1.0, m0() );
    CPPUNIT_ASSERT_EQUAL( -2.0, m1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, m2(1, 2.0) );
    CPPUNIT_ASSERT_EQUAL( -4.0, m3(1, 2.0, false) );
    CPPUNIT_ASSERT_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

void Generic_TaskTest::testMethodFactory()
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new Method object.

    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);

    TaskObject to("task");

    CPPUNIT_ASSERT( to.methods()->addMethod(&m0) );
    CPPUNIT_ASSERT( ! to.methods()->addMethod(&m0) );
    CPPUNIT_ASSERT( to.methods()->addMethod(&m1) );
    CPPUNIT_ASSERT( to.methods()->addMethod(&m2) );

    // test constructor
    Method<double(void)> mm0 = to.methods()->getMethod<double(void)>("m0");
    CPPUNIT_ASSERT( mm0.getMethodImpl() );
    CPPUNIT_ASSERT( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.methods()->getMethod<double(int)>("m1");
    CPPUNIT_ASSERT( mm1.getMethodImpl() );
    CPPUNIT_ASSERT( mm1.ready() );

    Method<double(int,double)> mm2 = to.methods()->getMethod<double(int,double)>("m2");
    CPPUNIT_ASSERT( mm2.getMethodImpl() );
    CPPUNIT_ASSERT( mm2.ready() );

    // start the activity, such that methods are accepted.
    CPPUNIT_ASSERT( tsim->start()) ;
    // execute methods and check status:
    CPPUNIT_ASSERT_EQUAL( -1.0, mm0() );
    
    CPPUNIT_ASSERT_EQUAL( -2.0, mm1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, mm2(1, 2.0) );

    // test error cases:
    // Add uninitialised method:
    Method<void(void)> mvoid;
    CPPUNIT_ASSERT(to.methods()->addMethod( &mvoid ) == false);
    mvoid = Method<void(void)>("voidm");
    CPPUNIT_ASSERT(to.methods()->addMethod( &mvoid ) == false);

    // wrong type 1:
    mvoid = to.methods()->getMethod<void(void)>("m1");
    CPPUNIT_ASSERT( mvoid.ready() == false );
    // wrong type 2:
    mvoid = to.methods()->getMethod<void(bool)>("m1");
    // wrong type 3:
    mvoid = to.methods()->getMethod<double(void)>("m0");
    CPPUNIT_ASSERT( mvoid.ready() == false );
    // non existing
    mvoid = to.methods()->getMethod<void(void)>("voidm");
    CPPUNIT_ASSERT( mvoid.ready() == false );

    // this line may not crash:
    mvoid();
    
}

void Generic_TaskTest::testCRMethod()
{
    this->ret = -3.3;

    Method<double&(void)> m0r("m0r", &Generic_TaskTest::m0r, this);
    Method<const double&(void)> m0cr("m0cr", &Generic_TaskTest::m0cr, this);

    Method<double(double&)> m1r("m1r", &Generic_TaskTest::m1r, this);
    Method<double(const double&)> m1cr("m1cr", &Generic_TaskTest::m1cr, this);

    CPPUNIT_ASSERT_EQUAL( -3.3, m0r() );
    CPPUNIT_ASSERT_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    CPPUNIT_ASSERT_EQUAL( 5.3*2, m1r(value) );
    CPPUNIT_ASSERT_EQUAL( 5.3*2, value );
    CPPUNIT_ASSERT_EQUAL( 5.3, m1cr(5.3) );
}


void Generic_TaskTest::testMethodFromDS()
{
    TaskObject to("task");

    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &Generic_TaskTest::m4, this);

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

    CPPUNIT_ASSERT( mc0.execute() );
    CPPUNIT_ASSERT_EQUAL(-1.0, ret);
    CPPUNIT_ASSERT( mc1.execute() );
    CPPUNIT_ASSERT_EQUAL(-2.0, ret);
    CPPUNIT_ASSERT( mc2.execute() );
    CPPUNIT_ASSERT_EQUAL(-3.0, ret);
    CPPUNIT_ASSERT( mc3.execute() );
    CPPUNIT_ASSERT_EQUAL(-4.0, ret);
    CPPUNIT_ASSERT( mc4.execute() );
    CPPUNIT_ASSERT_EQUAL(-5.0, ret);
}

void Generic_TaskTest::testDSMethod()
{
    TaskObject to("task");

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Method<double(Generic_TaskTest*)> meth0("m0",
                                          &Generic_TaskTest::m0);

    method_ds("m0", &Generic_TaskTest::m0);

    Method<double(Generic_TaskTest*,int)> meth1("m1",
                                          &Generic_TaskTest::m1);

    method_ds("m1", &Generic_TaskTest::m1);
    method_ds("ms",&Generic_TaskTest::comstr );

    boost::shared_ptr<Generic_TaskTest> ptr( new Generic_TaskTest() );
    ValueDataSource<boost::weak_ptr<Generic_TaskTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<Generic_TaskTest> >( ptr );
    CPPUNIT_ASSERT( to.methods()->addMethodDS( wp.get(), meth0, "desc" ) );
    CPPUNIT_ASSERT( to.methods()->addMethodDS( wp.get(), meth1, "desc", "a1", "d1" ) );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();
    
    CPPUNIT_ASSERT( tsim->start()) ;

    double ret;
    MethodC c0  = to.methods()->create("m0").ret(ret);
    CPPUNIT_ASSERT( c0.execute() );
    CPPUNIT_ASSERT_EQUAL( -1.0, ret );
    MethodC c1  = to.methods()->create("m1").argC(1).ret(ret);
    CPPUNIT_ASSERT( c1.execute() );
    CPPUNIT_ASSERT_EQUAL( -2.0, ret );

    CPPUNIT_ASSERT( tsim->stop()) ;

}

void Generic_TaskTest::testAddMethod()
{
    Method<double(void)> m0 = method("m0", &Generic_TaskTest::m0, this);

    Method<double(int)> m1 = method("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2 = method("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3 = method("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4 = method("m4", &Generic_TaskTest::m4, this);

    CPPUNIT_ASSERT_EQUAL( -1.0, m0() );
    CPPUNIT_ASSERT_EQUAL( -2.0, m1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, m2(1, 2.0) );
    CPPUNIT_ASSERT_EQUAL( -4.0, m3(1, 2.0, false) );
    CPPUNIT_ASSERT_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

