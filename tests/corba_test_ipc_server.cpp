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



#include "corba_test.hpp"

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <Method.hpp>
#include <interface/OperationInterface.hpp>
#include <transports/corba/DataFlowI.h>
#include <transports/corba/RemotePorts.hpp>
#include <transports/corba/OperationsC.h>
#include <transports/corba/OperationInterfaceC.h>
#include <transports/corba/corba.h>
#include <InputPort.hpp>
#include <OutputPort.hpp>
#include <TaskContext.hpp>
#include <transports/corba/ControlTaskServer.hpp>
#include <transports/corba/ControlTaskProxy.hpp>
#include <string>

using namespace RTT;
using namespace RTT::detail;

class TheServer : public TaskContext {
    // Ports
    InputPort<double>  mi1;
    OutputPort<double> mo1;

public:
    TheServer(): TaskContext("peer"), mi1("mi"), mo1("mo") {

        ports()->addEventPort( &mi1 );
        ports()->addPort( &mo1 );

    }
    void updateHook(){
        double d = 123456.789;
        mi1.read(d);
        mo1.write(d);
    }
};

class CorbaTest
{
public:
    CorbaTest() { this->setUp(); }
    ~CorbaTest() { this->tearDown(); }

    TaskContext* tc;
    corba::ControlTaskServer* ts;

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

    void setUp();
    void tearDown();
};

using namespace std;

void
CorbaTest::setUp()
{
    // connect DataPorts
    tc =  new TheServer();
    tc->addObject( this->createMethodFactory() );
}


void
CorbaTest::tearDown()
{
    delete ts;
    delete tc;

    delete mi1;
    delete mo1;
}

TaskObject* CorbaTest::createMethodFactory()
{
    TaskObject* to = new TaskObject("methods");

    to->methods()->addMethod( method("vm0",  &CorbaTest::vm0, this), "VM0");
    to->methods()->addMethod( method("m0",  &CorbaTest::m0, this), "M0");
    to->methods()->addMethod( method("m1",  &CorbaTest::m1, this), "M1","a","ad");
    to->methods()->addMethod( method("m2",  &CorbaTest::m2, this), "M2","a","ad","a","ad");
    to->methods()->addMethod( method("m3",  &CorbaTest::m3, this), "M3","a","ad","a","ad","a","ad");
    to->methods()->addMethod( method("m4",  &CorbaTest::m4, this), "M4","a","ad","a","ad","a","ad","a","ad");
    return to;
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  CorbaServerTestSuite,  CorbaTest )

BOOST_AUTO_TEST_CASE( testBeServer )
{
    corba::ControlTaskProxy::InitOrb(0,0);
    tc->start();
    ts = corba::ControlTaskServer::Create( tc, true ); //use-naming
    BOOST_REQUIRE( ts );

    // wait for shutdown.
    corba::ControlTaskServer::RunOrb();
    tc->stop();
    corba::ControlTaskProxy::DestroyOrb();
}

BOOST_AUTO_TEST_SUITE_END()

