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

class CorbaMQueueTest
{
public:
    CorbaMQueueTest() { this->setUp(); }
    ~CorbaMQueueTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;
    TaskContext* tp;
    corba::ControlTaskServer* ts;
    TaskContext* tp2;
    corba::ControlTaskServer* ts2;

    PortInterface* signalled_port;
    void new_data_listener(PortInterface* port);

    // Ports
    InputPort<double>*  mr1;
    OutputPort<double>* mw1;
    InputPort<double>*  mr2;
    OutputPort<double>* mw2;

    void setupCorba();
    void cleanupCorba();

    void setUp();
    void tearDown();

    void testPortConnections();
    void testPortProxying();

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

#endif
