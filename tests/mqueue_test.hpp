#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <InputPort.hpp>
#include <OutputPort.hpp>
#include <TaskContext.hpp>
#include <string>

using namespace RTT;
using namespace RTT::detail;

class MQueueTest
{
public:
    MQueueTest() { this->setUp(); }
    ~MQueueTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;

    PortInterface* signalled_port;
    void new_data_listener(PortInterface* port);

    // Ports
    InputPort<double>*  mr1;
    OutputPort<double>* mw1;
    InputPort<double>*  mr2;
    OutputPort<double>* mw2;

    void setUp();
    void tearDown();

    void testPortConnections();
    void testPortStreams();

    // helper test functions
    void testPortDataConnection();
    void testPortBufferConnection();
    void testPortDisconnected();
};

#endif
