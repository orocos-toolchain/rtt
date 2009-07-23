#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <corba/corba.h>
#include <Ports.hpp>
#include <TaskContext.hpp>
#include <corba/ControlTaskServer.hpp>
#include <corba/ControlTaskProxy.hpp>
#include <string>

using namespace RTT;

class CorbaTest
{
public:
    CorbaTest() { this->setUp(); }
    ~CorbaTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;
    TaskContext* tp;
    Corba::ControlTaskServer* ts;
    TaskContext* tp2;
    Corba::ControlTaskServer* ts2;
    TaskObject* createMethodFactory();

    PortInterface* signalled_port;
    void new_data_listener(PortInterface* port);

    // Ports
    InputPort<double>*  mr1;
    OutputPort<double>* mw1;
    InputPort<double>*  mr2;
    OutputPort<double>* mw2;

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

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

#endif
