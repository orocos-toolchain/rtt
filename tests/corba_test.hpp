#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <transports/corba/corba.h>
#include <InputPort.hpp>
#include <OutputPort.hpp>
#include <TaskContext.hpp>
#include <transports/corba/ControlTaskServer.hpp>
#include <transports/corba/ControlTaskProxy.hpp>
#include <string>

using namespace RTT;
using namespace RTT::detail;

class CorbaTest
{
public:
    CorbaTest() { this->setUp(); }
    ~CorbaTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;
    TaskContext* tp;
    corba::ControlTaskServer* ts;
    TaskContext* tp2;
    corba::ControlTaskServer* ts2;
    TaskObject* createMethodFactory();

    PortInterface* signalled_port;
    void new_data_listener(PortInterface* port);

    // Ports
    InputPort<double>*  mi1;
    OutputPort<double>* mo1;
    InputPort<double>*  mi2;
    OutputPort<double>* mo2;

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

    void setupCorba();
    void cleanupCorba();

    void setUp();
    void tearDown();

    void testRemoteMethodC();
    void testRemoteMethod();
    void testAnyMethod();

    void testDataFlowInterface();

    void testPortConnections();
    void testPortProxying();

    void testHalfs();

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

#endif
