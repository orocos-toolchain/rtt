#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <TaskContext.hpp>
#include <DataPort.hpp>
#include <BufferPort.hpp>
#include <corba/ControlTaskServer.hpp>
#include <corba/ControlTaskProxy.hpp>
#include <string>

using namespace RTT;

class CorbaTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CorbaTest );

    CPPUNIT_TEST( setupCorba );
    CPPUNIT_TEST( testRemoteMethodC );
    CPPUNIT_TEST( testRemoteMethod );
    CPPUNIT_TEST( testAnyMethod );
    CPPUNIT_TEST( testPorts );
    CPPUNIT_TEST( testConnectPortsIDL );
    CPPUNIT_TEST( testConnectPortsRL );
    CPPUNIT_TEST( testConnectPortsLR );
    CPPUNIT_TEST( testConnectPortsRR );
    CPPUNIT_TEST( testConnectPortsLRC );
    CPPUNIT_TEST( testConnectPortsRLC );
    CPPUNIT_TEST( testConnectPortsRRC );
    CPPUNIT_TEST( testConnections );
    CPPUNIT_TEST( cleanupCorba );
    
    CPPUNIT_TEST_SUITE_END();

    TaskContext* tc;
    TaskContext* t2;
    Corba::ControlTaskProxy* tp;
    Corba::ControlTaskServer* ts;
    Corba::ControlTaskProxy* tp2;
    Corba::ControlTaskServer* ts2;
    TaskObject* createMethodFactory();
    
    // Ports
    DataPort<double>* md1;
    DataPort<double>* md1bis;
    DataPort<double>* md2;
    ReadDataPort<double>* mdr1;
    ReadDataPort<double>* mdr2;
    WriteDataPort<double>* mdw1;
    WriteDataPort<double>* mdw2;
    BufferPort<double>* mb1;
    BufferPort<double>* mb2;

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

    void setupCorba();
    void cleanupCorba();

    void setUp();
    void tearDown();

    void testRemoteMethodC();
    void testRemoteMethod();
    void testAnyMethod();

    void testPorts();
    void testConnectPortsIDL();
    void testConnectPortsRL();
    void testConnectPortsLR();
    void testConnectPortsRR();
    void testConnectPortsRLC();
    void testConnectPortsLRC();
    void testConnectPortsRRC();
    void testConnections();

    // helper test functions
    void testPortStats();
    void testPortDisconnect();
};

#endif
