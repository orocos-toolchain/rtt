/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  corba_mqueue_test.hpp

                        corba_mqueue_test.hpp -  description
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

class CorbaMQueueTest
{
public:
    CorbaMQueueTest() { this->setUp(); }
    ~CorbaMQueueTest() { this->tearDown(); }

    TaskContext* tc;
    TaskContext* t2;
    TaskContext* tp;
    corba::TaskContextServer* ts;
    TaskContext* tp2;
    corba::TaskContextServer* ts2;

    base::PortInterface* signalled_port;
    void new_data_listener(base::PortInterface* port);

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
