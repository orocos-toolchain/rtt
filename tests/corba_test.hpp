#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <TaskContext.hpp>
#include <corba/ControlTaskServer.hpp>
#include <corba/ControlTaskProxy.hpp>
#include <string>

using namespace RTT;

class CorbaTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CorbaTest );

    CPPUNIT_TEST( testRemoteMethodC );
    //CPPUNIT_TEST( testRemoteMethod );
    CPPUNIT_TEST( testAnyMethod );
    
    CPPUNIT_TEST_SUITE_END();

    TaskContext* tc;
    Corba::ControlTaskProxy* tp;
    Corba::ControlTaskServer* ts;
    TaskObject* createMethodFactory();

    // ref/const-ref tests:
    double ret;
    double& m0r() { return ret; }
    const double& m0cr() { return ret; }

    // test const std::string& argument for command_ds
    bool comstr(const std::string& cs) { return !cs.empty(); }

    double m1r(double& a) { a = 2*a; return a; }
    double m1cr(const double& a) { return a; }

    // plain argument tests:
    double m0() { return -1.0; }
    double m1(int i) { return -2.0; }
    double m2(int i, double d) { return -3.0; }
    double m3(int i, double d, bool c) { return -4.0; }
    double m4(int i, double d, bool c, std::string s) { return -5.0; }

    // void(void) function test:
    void vm0(void) { ; }

    bool assertBool( bool );
public:

    void setUp();
    void tearDown();

    void testRemoteMethodC();
    void testRemoteMethod();
    void testAnyMethod();
};

#endif
