/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  corba_test.hpp

                        corba_test.hpp -  description
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


#ifndef CORBA_TEST_H
#define CORBA_TEST_H

#include <transports/corba/corba.h>
#include <rtt/InputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/TaskContext.hpp>
#include <transports/corba/TaskContextServer.hpp>
#include <transports/corba/TaskContextProxy.hpp>
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
    corba::TaskContextServer* ts;
    TaskContext* tp2;
    corba::TaskContextServer* ts2;
    Service* createOperationCallerFactory();

    base::PortInterface* signalled_port;
    void new_data_listener(base::PortInterface* port);

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

    void testRemoteOperationCallerC();
    void testRemoteOperationCaller();
    void testAnyOperationCaller();

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
