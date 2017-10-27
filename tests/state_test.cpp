/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  state_test.cpp

                        state_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"

#include <rtt-config.h>
#include <extras/SimulationThread.hpp>
#include <extras/SimulationActivity.hpp>
#include <scripting/StateMachine.hpp>
#include <scripting/ParsedStateMachine.hpp>
#include <scripting/DumpObject.hpp>
#include <scripting/parse_exception.hpp>
#include <rtt/internal/GlobalEngine.hpp>

#include <Service.hpp>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Port.hpp>
#include <scripting/ScriptingService.hpp>
#include <rt_string.hpp>
#include "operations_fixture.hpp"

#include <string>
#include <iostream>
#include <sstream>

using namespace RTT;
using namespace RTT::detail;
using namespace RTT::internal;
using namespace std;

class StateTest
    : public OperationsFixture
{
public:
    InputPort<double> d_event;
    InputPort<bool>   b_event;
    InputPort<int>    t_event;
    Operation<void(void)>   v_event;
    Operation<void(double)> o_event;
    Operation<void(void)>   v1_event;
    Operation<void(void)>   v2_event;
    Operation<void(void)>   v3_event;
    Operation<void(double)> c_event;
    OutputPort<double> d_event_source;
    OutputPort<bool>   b_event_source;
    OutputPort<int>    t_event_source;
    ScriptingService::shared_ptr sa;

    RTT::Operation<void(RTT::rt_string)>    setState_op;
    void setState(RTT::rt_string state) {
        mrt_state = state;
        //cout << "State = " << mrt_state.c_str() << endl;
    }

    RTT::rt_string mrt_state;

    int var_i;
    int const_i;
    SendStatus tss;

    void log(const std::string& msg) {
        Logger::log(Logger::Info) << msg << endlog();
    }
    void doState(const std::string& name, const std::string& prog, TaskContext*, bool test=true, int runs = 1000 );
    void parseState( const std::string& prog, TaskContext*, bool test=true );
    void runState(const std::string& name, TaskContext*, bool trace=true, bool test=true, int runs = 1000 );
    void checkState( const std::string& name, TaskContext*, bool test=true );
    void finishState( std::string const& name, TaskContext*, bool test=true );

    std::string sline;
public:
    StateTest()
        :
         d_event("d_event"), b_event("b_event"), t_event("t_event"), v_event("v_event"),o_event("o_event"),
         v1_event("v1_event"),v2_event("v2_event"),v3_event("v3_event"),c_event("c_event"),
         d_event_source("d_event_source"), b_event_source("b_event_source"), t_event_source("t_event_source")
         ,sa( ScriptingService::Create(tc) ),
         setState_op("setState", &StateTest::setState, this, RTT::OwnThread)
    {
        tc->stop();
        tc->setActivity( new SimulationActivity(0.001) );

        tc->addPeer(caller);

        tc->ports()->addPort( d_event );
        tc->ports()->addPort( b_event );
        tc->ports()->addPort( t_event );
#ifdef ORO_SIGNALLING_OPERATIONS
        tc->provides()->addEventOperation( o_event );
        tc->provides()->addEventOperation( v_event );
        tc->provides()->addEventOperation( v1_event );
        tc->provides()->addEventOperation( v2_event );
        tc->provides()->addEventOperation( v3_event );
        caller->provides()->addEventOperation( c_event );
#else
        tc->provides()->addOperation( o_event );
        tc->provides()->addOperation( v_event );
        tc->provides()->addOperation( v1_event );
        tc->provides()->addOperation( v2_event );
        tc->provides()->addOperation( v3_event );
        caller->provides()->addOperation( c_event );
#endif

        tc->provides()->addOperation(setState_op).doc("Communicates state from SM").arg("state", "Name of state");

        tc->ports()->addPort( d_event_source );
        tc->ports()->addPort( b_event_source );
        tc->ports()->addPort( t_event_source );

        d_event_source.connectTo( &d_event );
        b_event_source.connectTo( &b_event );
        t_event_source.connectTo( &t_event );
        tc->start();
        i = 0;
        SimulationThread::Instance()->stop();

        tc->addOperation("log", &StateTest::log, this);

        tc->provides()->addAttribute("tvar_i", var_i);
        tc->provides()->addAttribute("tss", tss);
        tc->provides()->addConstant("tconst_i", const_i);

        const_i = -1;
        var_i = -1;
        tss = SendNotReady;
    }
    ~StateTest(){
    }
};

BOOST_FIXTURE_TEST_SUITE( StateTestSuite, StateTest )

BOOST_AUTO_TEST_CASE( testParseState)
{
    // a state which should never fail
    string prog = string("StateMachine X {\n")
        + " param int isten\n"
        + " param bool isflse\n"
        + " param bool isok\n"
        + " param double isnegative\n"
        + " var bool istrrue = true\n"
        + " var double d_dummy = -1.0\n"
        + " var int    i_dummy = -1\n"
        + " var bool   varinit = false\n"
        + " var bool   b_dummy = true\n" // 10
        + " initial state INIT {\n"
        // XXX bug : preconditions are not checked in the initial state.
//         + " preconditions {\n"
//         + "     if (istrrue == false) || (isflse == true) || (isten != 10) ||( isnegative >= 0. )  then select PRE_PARAMFAIL\n"
//         + "     if false then select PRE_ERROR\n"
//         + "     if (isnegative != -1.) then select PRE_PARAMFAIL\n"
//         + "     if (istrrue != true) then select PRE_PARAMFAIL\n"
//         + "     if (isflse != false) then select PRE_PARAMFAIL\n"
//         + "     if (isten != 10 ) then select PRE_PARAMFAIL\n"
//         + "     if (d_dummy != -1.) || (i_dummy != -1) then select PRE_VARFAIL\n"
//         + " }\n"
        + " entry {\n"
        + "     set varinit = (d_dummy != -1.) || (i_dummy != -1) \n"
        + "     do test.good()\n"
        + "     set d_dummy = 1.234\n"
        + "     set i_dummy = -2\n"
        + "     do test.good()\n"
        + " }\n"
        + " handle {\n"
        + "     do test.good()\n" // 20
        + " }\n"
        + " exit {\n"
        + "     do test.good()\n"
        + "     set d_dummy = 0.0\n"
        + "     set i_dummy = 0\n"
        + " }\n"
        + " transitions {\n"
        + "     if false then select ERROR\n"
        + "     if varinit == true then select PRE_VARFAIL\n"
        + "     if (d_dummy != 1.234) || (i_dummy != -2)  then select ENTRYFAIL\n" // 30
        + " }\n"
        + " transition if (istrrue == false) || (isflse == true) || (isten != 10) ||( isnegative >= 0. )  then select PARAMFAIL\n"
        + " transition if isok == false then select PARAMFAIL\n"
        + " transition select FINI\n"
        + " transition select ERROR\n" // do not reach
        + " }\n"
        + " state PRE_ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PRE_PARAMFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PRE_VARFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PARAMFAIL {\n"
        + "      entry { \n"
        + "      do test.assertMsg( isten == 10, \"isten parameter not correctly initialised\")\n"
        + "      do test.assertMsg( istrrue == true, \"istrrue parameter not correctly initialised\")\n"
        + "      do test.assertMsg( isok == true, \"isok parameter not correctly initialised\")\n"
        + "      do test.assertMsg( isflse == false, \"isflse parameter not correctly initialised\")\n"
        + "      do test.assertMsg( true == true, \"true ident not correctly done\")\n"
        + "      do test.assertMsg( false == false, \"false ident not correctly done\")\n"
        + "      do test.assertMsg( isnegative == -1.0, \"isnegative parameter not correctly initialised\")\n"
        + "      }\n"
        + " }\n"
        + " state VARFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state EXITFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state ENTRYFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " final state FINI {\n"
        + " preconditions {\n"
        + "     if (d_dummy != 0.) || (i_dummy != 0)  then select EXITFAIL\n"
        + "     if false then select ERROR\n"
        + " }\n"
        + " entry {\n"
        + "     do test.good()\n"
        + "     set d_dummy = -1.\n"
        + "     set i_dummy = -1\n"
        + " }\n"
        + " handle {\n"
        + "     do test.good()\n"
        + " }\n"
        + " exit {\n"
        + "     do test.good()\n"
        + " }\n"
        + " transitions {\n"
        + "     if false then select ERROR\n"
        + "     select INIT\n"
        + "     select ERROR\n" // do not reach
        + " }\n"
        + " }\n"
        + " }\n"
        //        + " RootMachine X x( isten = 10, isflse = false, isnegative = -1.0) \n" // instantiate a non hierarchical SC
        + " RootMachine X x( isten = 10, isok = true, isflse=false, isnegative = -1.0) \n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}

BOOST_AUTO_TEST_CASE( testStateFailure)
{
    // test _command_ (through methods though) failure detection on several places.
    // it is an incomplete test, even more that parsing should fail on the second
    // run since the type 'X' is defined twice.
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " entry {\n"
        + "     do test.increase()\n"                // set i to i+1
        + "     do test.assert( test.i != 1)\n" // fail if i == 1
        + " }\n"
        + " run {\n"
        + "     do test.assert( test.i != 2)\n"
        + " }\n"
        + " exit {\n"
        + "     do test.assert( test.i != 3)\n"
        + " }\n"
        + " transitions {\n"
        + "     if (true) then { do test.assert( test.i != 7); } select FINI\n"
        + " }\n"
        + " }\n"
        + " state ERROR {\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do test.assert( test.i != 4)\n"
        + " }\n"
        + " run {\n"
        + "     do test.assert( test.i != 5)\n"
        + " }\n"
        + " exit {\n"
        + "     do test.assert( test.i != 6)\n"
        + " }\n"
        + " transitions {\n"
        + "     select ERROR\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    // should fail each time
    const int max = 7;
    int x = 0;
    while ( i < max && x < max) {
        this->doState("x", prog, tc, false );
        //cout << "i is: "<< i <<endl;
        // assert that an error happened :
        BOOST_CHECK_MESSAGE( sa->getStateMachineStatus("x") == StateMachine::Status::error, "Status is: " + sa->getStateMachineStatusStr("x") );

        this->finishState( "x", tc, false);
        ++x;
    }
}
BOOST_AUTO_TEST_CASE( testStateChildren)
{
    // instantiate two children and check init of vars and params
    string prog = string("StateMachine Y {\n")
        + " param double isnegative\n"
        + " var   double t = 1.0\n"
        + " initial state INIT {\n"
        + " entry {\n"
        + "     do test.good()\n"
        + " }\n"
        + " transitions {\n"
        + "     if isnegative >= 0. then select PARAMFAIL\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " state ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PARAMFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state VARFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state EXITFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state ENTRYFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do test.good()\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + string("StateMachine Z {\n")
        + " param double neg\n"
        + " initial state INIT {\n"
        + " transitions {\n"
        + "     if neg >= 0. then select PARAMFAIL\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " state PARAMFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " final state FINI {\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + string("StateMachine X {\n")
        + " param double isnegative\n"
        + " var double d_dummy = -2.0\n"
        + " var int    i_dummy = -1\n"
        + " SubMachine Y y1(isnegative = d_dummy)\n"
        + " SubMachine Y y2(isnegative = -3.0)\n"
        + " SubMachine Y y3(isnegative = isnegative)\n"
        + " SubMachine Z z1( neg = d_dummy)\n"
        + " initial state INIT {\n"
        + " entry {\n"
        + "     do test.good()\n"
        + "     do y1.activate()\n"
        + "     do y2.activate()\n"
        + "     do y3.activate()\n"
        + "     do z1.activate()\n"
        + " }\n"
        + " exit {\n"
        + "     do y1.start()\n"
        + "     do y2.start()\n"
        + "     do y3.start()\n"
        + "     do z1.start()\n"
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " state ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PARAMFAIL {\n"
        + "      entry { \n"
        + "      do test.assertMsg( y3.isnegative == isnegative, \"y3 parameter not correctly initialised\")\n"
        + "      do test.assertMsg( y2.isnegative == -3.0, \"y2 parameter not correctly initialised\")\n"
        + "      do test.assertMsg( y1.isnegative == d_dummy, \"y1 parameter not correctly initialised\")\n"
        + "      do test.assertMsg( z1.neg == d_dummy, \"z1 parameter not correctly initialised\")\n"
        + "      }\n"
        + " }\n"
        + " state VARFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state EXITFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state ENTRYFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do test.good()\n"
        + "     do y1.stop()\n"
        + "     do y2.stop()\n"
        + "     do y3.stop()\n"
        + "     do z1.stop()\n"
        + " }\n"
        + " exit {\n"
        + "     do test.good()\n"
        + "     do y1.deactivate()\n"
        + "     do y2.deactivate()\n"
        + "     do y3.deactivate()\n"
        + "     do z1.deactivate()\n"
        + " }\n"
        + " transitions {\n"
        + "     if z1.neg != d_dummy then select PARAMFAIL\n"
        + "     if y1.isnegative != d_dummy then select PARAMFAIL\n"
        + "     if y2.isnegative != -3.0 then select PARAMFAIL\n"
        + "     if y3.isnegative != isnegative then select PARAMFAIL\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x( isnegative = -1.0) \n" // instantiate a hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateEmpty)
{
    // test processing of completely empty states
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " transitions {\n"
        + "     select TEST;\n" // only a transition
        + " }\n"
        + " }\n"
        + " state TEST {\n"  // not even used
        + " }\n"
        + " final state FINI {\n" // completely empty
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateEmptyChild)
{
    // test processing of completely empty states
    string prog = string("StateMachine Y {\n")
        + " initial state INIT {\n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + "}\n"
        + "StateMachine X {\n"
        + "  SubMachine Y y;\n"
        + " initial state INIT {\n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x\n"
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateComments)
{
    // test parsing of comments
    string prog = string("// Start here\n")
        + "StateMachine X { //\n"
        + " // comment on INIT\n"
        + " initial state INIT /* INIT */ { // \n"
        + " // comment in INIT\n"
        + " entry /* entry */ { // \n"
        + " // in entry\n"
        + " } // end \n\n"
        + "//\n"
        + " }\n"
        + " // intermediate comment \n"
        + " final state FINI // \n"
        + " /* ... */\n"
        + " {\n"
        + "//\n"
        + " entry {\n"
        + " // in entry\n"
        + " /* in entry */\n"
        + " }\n\n"
        + "//\n"
        + " }\n"
        + "// final comment\n"
        + "//\n"
        + "}\n"
        + "// instantiate comment\n"
        + "//\n"
        + "RootMachine X x // end\n"
        ;
    this->doState("x", prog, tc );
    this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperations)
{
    // test processing of operations (OwnThread + ClientThread):
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + "   entry {\n"
        + "   setState( rt_string(\"INIT-ENTRY\") )\n"
        + " }\n"
        + " transitions {\n"
        + "     select TEST;\n" // only a transition
        + " }\n"
        + " }\n"
        + " state TEST {\n"
        + "   var double dret\n"
        + "   entry {\n"
        + "   setState( rt_string(\"TEST-ENTRY\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   }\n"
        + "   run {"
        + "   setState( rt_string(\"TEST-RUN\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   }\n"
        + "   exit {"
        + "   setState( rt_string(\"TEST-EXIT\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   }\n"
        + " transitions {\n"
        + "     if true then \n"
        + "   {"
        + "   setState( rt_string(\"TEST-TRANSIT\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   } select TEST2;\n"
        + " }\n"
        + " }\n"
        + " state TEST2 {\n"
        + "   entry {\n"
        + "   setState( rt_string(\"TEST-ENTRY\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   }\n"
        + "   run {"
        + "   setState( rt_string(\"TEST-RUN\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   }\n"
        + "   exit {"
        + "   setState( rt_string(\"TEST-EXIT\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   }\n"
        + " transitions {\n"
        + "     if true then \n"
        + "   {"
        + "   setState( rt_string(\"TEST-TRANSIT\") )\n"
        + "   methods.m0()\n"
        + "   methods.m1(1)\n"
        + "   methods.m2(1,2.0)\n"
        + "   methods.m3(1,2.0,true)\n"
        + "   methods.m4(1,2.0,true,\"hello\")\n"
        + "   methods.m5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.m6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.m7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   methods.o0()\n"
        + "   methods.o1(1)\n"
        + "   methods.o2(1,2.0)\n"
        + "   methods.o3(1,2.0,true)\n"
        + "   methods.o4(1,2.0,true,\"hello\")\n"
        + "   methods.o5(1,2.0,true,\"hello\",5.0)\n"
        + "   methods.o6(1,2.0,true,\"hello\",5.0,'a')\n"
        + "   dret = methods.o7(1,2.0,true,\"hello\",5.0,'a',7)\n"
        + "   test.assert( dret == -8.0 )\n"
        + "   } select FINI;\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n" // completely empty
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     parseState( prog, tc, true);

     tc->stop();
     BOOST_REQUIRE( tc->setActivity( new Activity(0, 0.001) ) ); // deliberately test with real thread instead of simulation.
     BOOST_REQUIRE( tc->start() );

     StateMachinePtr sm = sa->getStateMachine("x");
     BOOST_REQUIRE( sm );
     sm->trace(true);
     OperationCaller<bool(StateMachinePtr)> act = tc->provides("x")->getOperation("activate");
     OperationCaller<bool(StateMachinePtr)> autom = tc->provides("x")->getOperation("automatic");
     BOOST_CHECK( act(sm) );
     BOOST_CHECK( autom(sm) );

     sleep(1); // we must allow the thread to transition...

     checkState( "x", tc);
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc, false); // don't test
}


BOOST_AUTO_TEST_CASE( testStateTransitions)
{
    // test processing of transition statements.
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " var int i = 0;\n" // transition counter
        + " var int j = 0;\n" // entry counter
        + " var int k = 0;\n" // run counter
        + " entry {\n"
        + "   set j = j + 1\n"
        + " }\n"
        + " run {\n"
        + "   set k = k + 1\n"
        + " }\n"
        + " transitions {\n"
        + "  if i < 5 then {\n"
        + "    set i = i + 1;\n" // execute this program
        + "  } select INIT\n" // test EXPLICIT transition to self: no entry/exit.
        + "  if i < 10 then {\n"
        + "    set i = i + 1;\n" // execute this program
        + "  }\n" // test IMPLICIT transition to self: no entry/exit.
        + "  if i < 10 then {\n"
        + "  } select TRANS_SHOULD_NOT_CHECK\n" // should never be reached.
        + "  if i == 10 then {\n"
        + "    set i = i + 1;\n" // execute this program
        + "  } select TEST_ENTRY\n"
        + " }\n"
        + " }\n"
        + " state TEST_ENTRY {\n" // test if entry was executed one time
        + " transitions {\n"
        + "  if k != i then {\n"
        + "  } select RUN_FAILED\n"
        + "  if j != 1 then {\n"
        + "  } select ENTRY_FAILED\n"
        + "  else select FINI\n"
        + " }\n"
        + " }\n"
        + " state TRANS_SHOULD_NOT_CHECK {\n" // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " state ENTRY_FAILED {\n"           // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " state RUN_FAILED {\n"           // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " final state FINI {\n" // Success state.
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateTransitionStop )
{
    // test processing of transition statements.
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " transitions {\n"
        + "  if stop() == true then select NEXT\n" // calls stop on the component !
        + " }\n"
        + " }\n"
        + " state NEXT {\n" // Success state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " final state FINI {\n" // Failure state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("NEXT") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateYield )
{
    // test processing of yield statements when an eventTransition occurs:
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " var double d = 0.0\n"
        + " run { do o_event(1.0); test.i = 5; do test.assert(test.i == 5);\n" // synchronous call on o_event, so signal must be delivered when we return.
        + "       do yield;\n"
        + "       test.i = 10;\n"
        + "       do test.assert(false); }\n"
        + " transition o_event(d) select NEXT;\n"
        + " transitions {\n"
        + "       select FINI\n"
        + " }\n"
        + " }\n"
        + " state NEXT {\n" // Success state.
        + " entry { do test.assert(test.i == 5); }\n"
        + " }\n"
        + " final state FINI {\n" // Failure state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("NEXT") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateYieldbySend )
{
    // test processing of yield statements when a sent eventTransition occurs:
    // make o_event an ownthread op :
    this->o_event.getOperationCaller()->setThread(OwnThread, tc->engine() );
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " var double d = 0.0\n"
        + " run { do o_event.send(1.0); test.i = 5; do test.assert(test.i == 5);\n" // asynchronous send on o_event, so signal must be processed when we return.
        + "       do yield;\n"
        + "       test.i = 10;\n"
        + "       do test.assert(false); }\n"
        + " transition o_event(d) select NEXT;\n"
        + " transitions {\n"
        + "       select FINI\n"
        + " }\n"
        + " }\n"
        + " state NEXT {\n" // Success state.
        + " entry { do test.assert(test.i == 5); }\n"
        + " }\n"
        + " final state FINI {\n" // Failure state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("NEXT") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateYieldbyCmd )
{
    // test yielding and checking .cmd syntax
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " var double d = 0.0\n"
        + " run {\n"
        + "   test.assertEqual( test.i, 0 )\n"
        + "   var SendStatus ss\n"
        + "   ss = test.increaseCmd.cmd() \n"
        + "   test.assert( ss == SendSuccess )\n"
        + "   test.assertEqual( test.i, 1 )\n"
        + "   ss = test.increaseCmd.cmd()\n"
        + "   test.assert( ss == SendSuccess )\n"
        + "   test.assertEqual( test.i , 2 )\n"

        + "   tss = methods.vo0.cmd() \n" // bug : does not evaluate conditions !
        + "   test.assert( tss == SendSuccess )\n"
        + "   tss = methods.vo0.cmd()\n"
        + "   test.assert( tss == SendSuccess )\n"
        + " }\n"
        + " transitions {\n"
        + "       select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n" // Success state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateSendFunction )
{
    // test yielding and checking .send syntax
    string func = string("export function foo(int arg) {\n")
        + "  do test.assert( tvar_i == arg ) \n"
        + "  do test.assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = tvar_i+2\n"
        + "  do test.assert( tvar_i == arg + 2 ) \n"
        + "}\n";
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " run {\n"
        + "   tvar_i = 0\n"

        + "   var SendHandle sh, sh2\n"
        + "   sh = foo.send(tvar_i) \n"

        + "   sh\n" // tests accidental sh evaluation

        + "   test.assert( sh.collectIfDone() == SendNotReady )\n"
        + "   test.assertEqual( tvar_i, 0 )\n"

        + "   while ( sh.collectIfDone() == SendNotReady) \n"
        + "      yield\n"
        + "   test.assert( sh.collectIfDone() == SendSuccess )\n"
        + "   test.assertEqual( tvar_i, 2 )\n"

        + "   sh2 = foo.send(tvar_i) \n"
        + "   test.assert( sh2.collectIfDone() == SendNotReady )\n"
        + "   test.assertEqual( tvar_i, 2 )\n"
        + "   while ( sh2.collectIfDone() == SendNotReady) \n"
        + "      yield\n"
        + "   test.assert( sh2.collectIfDone() == SendSuccess )\n"
        + "   test.assertEqual( tvar_i , 4 )\n"

        + " }\n"
        + " transitions {\n"
        + "       select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n" // Success state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
    BOOST_REQUIRE( sa->loadPrograms(func, "func.ops", false) );
    this->doState("x", prog, tc, true, 25 );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateCmdFunction )
{
    // test yielding and checking .cmd syntax
    string func = string("export function foo(int arg) {\n")
        + "  do test.assert( tvar_i == arg ) \n"
        + "  do test.assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = tvar_i+2\n"
        + "  do test.assert( tvar_i == arg + 2 ) \n"
        + "}\n";
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " run {\n"
        + "   var SendStatus ss\n"
        + "   tvar_i = 0\n"

        + "   ss = foo.cmd(tvar_i)\n"
        + "   test.assert( ss == SendSuccess )\n"
        + "   test.assertEqual( tvar_i, 2 )\n"

        + "   ss = foo.cmd(tvar_i)\n"
        + "   test.assert( ss == SendSuccess )\n"
        + "   test.assertEqual( tvar_i , 4 )\n"

        + "   tss = foo.cmd(tvar_i)\n"
        + "   test.assert( tss == SendSuccess )\n"
        + "   tss = foo.cmd(tvar_i)\n"
        + "   test.assert( tss == SendSuccess )\n"
        + "   test.assertEqual( tvar_i , 8 )\n"

        + " }\n"
        + " transitions {\n"
        + "       select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n" // Success state.
        + " entry { do test.assert(true); }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
    BOOST_REQUIRE( sa->loadPrograms(func, "func.ops", false) );
    this->doState("x", prog, tc, true, 25 );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}


BOOST_AUTO_TEST_CASE( testStateGlobalTransitions)
{
    // test processing of transition statements.
    string prog = string("StateMachine X {\n")
        + " var int gi = 0;\n" // transition counter
        + " transitions {\n"

        + "  if gi < 5 then {\n"
        + "    set gi = gi + 1;\n" // execute this program
        + "  } select INIT\n" // test EXPLICIT transition to self: no entry/exit.

        + "  if gi < 10 then {\n"
        + "    if gi < 5 then do test.assert(false);\n"
        + "    set gi = gi + 1;\n" // execute this program
        + "  }\n" // test IMPLICIT transition to self: no entry/exit.
        + "  if gi < 10 then {\n"
        + "   do test.assert(false);\n"
        + "  } select TRANS_SHOULD_NOT_CHECK\n" // should never be reached.
        + "  if gi >= 10 then {\n"
        + "  } select FINI\n" // we must be checked before
        + " }\n"
        + " initial state INIT {\n"
        + " var int i = 0;\n" // transition counter
        + " var int j = 0;\n" // entry counter
        + " var int k = 0;\n" // run counter
        + " entry {\n"
        + "   set j = j + 1\n"
        + " }\n"
        + " run {\n"
        + "   set k = k + 1\n"
        + " }\n"
        + " transitions {\n"
        + "  if i < 5 then {\n"
        + "    set i = i + 1;\n" // execute this program
        + "  } select INIT\n" // test EXPLICIT transition to self: no entry/exit.
        + "  if i < 10 then {\n"
        + "    set i = i + 1;\n" // execute this program
        + "  }\n" // test IMPLICIT transition to self: no entry/exit.
        + "  if i < 10 then {\n"
        + "  } select TRANS_SHOULD_NOT_CHECK\n" // should never be reached.
        + " }\n"
        + " }\n"
        + " state TRANS_SHOULD_NOT_CHECK {\n" // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " state ENTRY_FAILED {\n"           // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " state RUN_FAILED {\n"           // failure state
        + " entry { do test.assert(false); }\n"
        + " }\n"
        + " final state FINI {\n" // Success state.
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
     this->doState("x", prog, tc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}


BOOST_AUTO_TEST_CASE( testStateSubStateVars)
{
    // test get/set access of substate variables and parameters
    string prog = string("StateMachine Y {\n")
        + " param double isnegative\n"
        + " var   double t = 1.0\n"
        + " initial state INIT {\n"
        + " transitions {\n"
        + "     if isnegative >= 0. then select PARAMFAIL\n"
        + "     if t >= 0. then select VARFAIL\n"
        + "     select FINI\n"
        + " }\n"
        + " exit { set isnegative = +1.0 }\n"
        + " }\n" // 11
        + " state ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " state PARAMFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state VARFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state EXITFAIL { entry { do test.assert(false) }\n"
        + " }\n"
        + " state ENTRYFAIL { entry { do test.assert(false) }\n"
        + " }\n" // 21
        + " final state FINI {\n"
        + " transitions {\n"
        + "     if isnegative <= 0. then select PARAMFAIL\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + string("StateMachine X {\n")
        + " param double isnegative\n"
        + " var double d_dummy = -2.0\n"
        + " var int    i_dummy = -1\n" //31
        + " SubMachine Y y1(isnegative = d_dummy)\n"
        + " initial state INIT {\n"
        + " entry {\n"
        + "     do y1.trace(true)\n"
        + "     do y1.activate()\n"
        + "     set y1.t = -1.0 \n"
        + " }\n"
        + " exit {\n"
        + "     do test.assert( y1.start() )\n"
        + " }\n"
        + " transitions {\n"
        + "     select TEST\n"
        + " }\n"
        + " }\n"
        + " state TEST {\n"
        + " entry {\n"
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"FINI\") )\n" // if y1 not in FINI, stop here.
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do y1.stop()\n" // prepare y1 to start-over
        + " }\n"
        + " exit {\n"
        + "     set y1.isnegative = -1.0 \n"
        + "     do y1.deactivate()\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n" // start-over y1.
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x( isnegative = -1.0) \n" // instantiate a hierarchical SC
        ;

     this->doState("x", prog, tc );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testStateSubStateCommands)
{
    // test get/set access of substate variables and parameters
    string prog = string("StateMachine Y {\n")
        + " param double isnegative\n"
        + " var   double t = 1.0\n"
        + " initial state INIT {\n"
        + " transitions {\n"
        + "     if isnegative < 0. then select ISNEGATIVE\n"
        + "     if t >= 0. then select ISPOSITIVE\n"
        + "     select DEFAULT\n"
        + " }\n"
        + " }\n"
        + " state ISNEGATIVE {\n"
        + " transitions {\n"
        + "      select INIT\n"
        + " }\n"
        + " }\n"
        + " state ISPOSITIVE {\n"
        + " transitions {\n"
        + "      select INIT\n"
        + " }\n"
        // 20 :
        + " }\n"
        + " state DEFAULT {\n"
        + " transitions {\n"
        + "      select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + " }\n"
        + string("StateMachine X {\n") // 1
        + " SubMachine Y y1(isnegative = -1.0)\n"
        + " initial state INIT {\n"
        + " entry {\n"
        + "     set y1.t = -1.0 \n"
        + "     do y1.activate()\n"
        + "     do y1.requestState(\"ISNEGATIVE\")\n"
        + "     do test.assert( y1.inState(\"ISNEGATIVE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     set y1.isnegative = +1.0 \n"
        + "     try y1.requestState(\"ISNEGATIVE\") \n "
        + "     catch \n{\n"
        + "         do test.assert( y1.inState(\"INIT\") )\n" // do not leave INIT
        + "     }\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n" // do not leave INIT
        + "     do y1.requestState(\"FINI\")\n"      // request final state
        + "     do test.assert( y1.inState(\"FINI\") )\n"
        + " }\n"
        + " exit {\n"
        + "     do y1.requestState(\"INIT\")\n"      // request initial state
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     set y1.isnegative = +1.0 \n"
        + "     set y1.t = -1.0 \n"
        + "     do y1.start()\n"  // must reach FINI after a while.
        + "     while ! y1.inState(\"FINI\") \n"
        + "        do nothing\n"
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do y1.stop()\n"
        + " }\n"
        + " exit {\n"
        + "     do y1.deactivate()\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x() \n" // instantiate a hierarchical SC
        ;

     this->doState("x", prog, tc );
     this->finishState( "x", tc);
}

#ifdef ORO_SIGNALLING_OPERATIONS
BOOST_AUTO_TEST_CASE( testStateOperationSignalTransition )
{
    // test event reception from own component
    string prog = string("StateMachine X {\n")
        + " var   double et = 0.0\n"
        + " initial state INIT {\n"
        + "    transition o_event(et) { test.assert(et == 3.33); } select FINI\n" // test signal transition
        + " }\n"
        + " final state FINI { entry { test.assert( et == 3.33);} } \n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    this->runState("x", tc);
    checkState( "x", tc);
    OperationCaller<void(double)> mo( tc->provides()->getOperation("o_event") );
    mo(3.33);
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(2) );
    checkState( "x", tc);
    BOOST_CHECK( sm->inState("FINI") );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationCallerSignalTransition )
{
    // test event reception from another component
    string prog = string("StateMachine X {\n")
        + " var   double et = 0.0\n"
        + " initial state INIT {\n"
        + "    transition caller.c_event(et) select FINI\n" // test signal transition
        + " }\n"
        + " final state FINI {} \n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    this->runState("x", tc);
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(2) );
    checkState( "x", tc);
    OperationCaller<void(double)> mc( caller->provides()->getOperation("c_event") );
    mc(6.66);
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(2) );
    checkState( "x", tc);
    BOOST_CHECK( sm->inState("FINI") );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationSignalMultiTransition )
{
    // test multi-event reception from own component
    string prog = string("StateMachine X {\n")
    + " initial state INIT {\n"
    + "    transitions { select STATE1 }\n"
    + " }\n"
    + " state STATE1 {\n"
    + "    transition v_event() select STATE2\n" // test signal transition
    + " }\n"
    + " state STATE2 {\n"
    + "    transition v_event() select FINI\n"   // test signal transition
    + " }\n"
    + " final state FINI {} \n"
    + "}\n"
    + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    sm->trace(true);
    // into STATE1
    this->runState("x", tc);
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE1", sm->getCurrentStateName() );
    // remain in STATE1
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE1", sm->getCurrentStateName() );
    // into STATE2
    OperationCaller<void(void)> mo( tc->provides()->getOperation("v_event"), tc->engine());
    BOOST_REQUIRE( mo.ready() );
    mo();
    BOOST_CHECK( SimulationThread::Instance()->run(1) ); // allow to transition
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE2", sm->getCurrentStateName() );
    // remain in STATE2
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE2", sm->getCurrentStateName() );
    // into FINI
    mo();
    BOOST_CHECK( SimulationThread::Instance()->run(1) ); // allow to transition
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "FINI", sm->getCurrentStateName() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "FINI", sm->getCurrentStateName() );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationSignalTransitionPriority )
{
    // test event reception transition priority
    string prog = string("StateMachine X {\n")
    + " initial state INIT {\n"
    + "    transitions { select STATE1 }\n"
    + " }\n"
    + " state STATE1 {\n"
    + "    var double d;\n"       
    + "    transition v_event() select STATE2\n" // test signal transition
    + "    transition v_event() select ERROR\n"   // This one should not be taken.
    + "    transition o_event(d) select ERROR\n"   // This one should not be taken.
    + "    transition v_event() select ERROR\n"   // This one should not be taken.
    + " }\n"
    + " state STATE2 {\n"
    + "    transition v_event() select FINI\n"   // test signal transition
    + "    transition v_event() select ERROR\n"   // This one should not be taken.
    + "    transition o_event(d) select ERROR\n"   // This one should not be taken.
    + "    transition v_event() select ERROR\n"   // This one should not be taken.
    + " }\n"
    + " state ERROR {} \n"
    + " final state FINI {} \n"
    + "}\n"
    + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    sm->trace(true);
    // into STATE1
    this->runState("x", tc);
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE1", sm->getCurrentStateName() );
    // remain in STATE1
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE1", sm->getCurrentStateName() );
    // into STATE2
    OperationCaller<void(void)> mo( tc->provides()->getOperation("v_event"), tc->engine());
    OperationCaller<void(double)> mo2( tc->provides()->getOperation("o_event"), tc->engine());
    BOOST_REQUIRE( mo.ready() );
    mo();
    mo();
    mo2(3);
    mo2(3);
    BOOST_CHECK( SimulationThread::Instance()->run(1) ); // allow to transition
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE2", sm->getCurrentStateName() );
    // remain in STATE2
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "STATE2", sm->getCurrentStateName() );
    // into FINI
    mo();
    mo();
    mo2(3);
    mo2(3);
    BOOST_CHECK( SimulationThread::Instance()->run(1) ); // allow to transition
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "FINI", sm->getCurrentStateName() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "FINI", sm->getCurrentStateName() );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationSignalTransitionAround )
{
    // test event reception hopping from one state to another
    string prog = string("StateMachine X {\n")
    + " initial state INIT {\n"
    + "    transitions { select IDLE }\n"
    + " }\n"
    + " state IDLE {\n"
    + "    transition v1_event() select STATE1\n"
    + "    transition v2_event() select STATE2\n"
    + "    transition v3_event() select STATE3\n"
    + " }\n"
    + " state STATE1 {\n"
    + "    transition v_event()  select IDLE\n"
    + "    transition v2_event() select STATE2\n"
    + "    transition v3_event() select STATE3\n"
    + " }\n"
    + " state STATE2 {\n"
    + "    transition v_event()  select IDLE\n"
    + "    transition v1_event() select STATE1\n"
    + "    transition v3_event() select STATE3\n"
    + " }\n"
    + " state STATE3 {\n"
    + "    transition v_event()  select IDLE\n"
    + "    transition v1_event() select STATE1\n"
    + "    transition v2_event() select STATE2\n"
    + " }\n"
    + " final state FINI {} \n"
    + "}\n"
    + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    this->runState("x", tc);
    // into IDLE
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "IDLE", sm->getCurrentStateName() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    checkState( "x", tc);
    BOOST_CHECK_EQUAL( "IDLE", sm->getCurrentStateName() );

    OperationCaller<void(void)> v( tc->provides()->getOperation("v_event"), tc->engine());
    BOOST_REQUIRE( v.ready() );
    OperationCaller<void(void)> v1( tc->provides()->getOperation("v1_event"), tc->engine());
    BOOST_REQUIRE( v1.ready() );
    OperationCaller<void(void)> v2( tc->provides()->getOperation("v2_event"), tc->engine());
    BOOST_REQUIRE( v2.ready() );
    OperationCaller<void(void)> v3( tc->provides()->getOperation("v3_event"), tc->engine());
    BOOST_REQUIRE( v3.ready() );

#define DO_EVENT(event, name)                               \
    event();                                                \
    BOOST_CHECK( SimulationThread::Instance()->run(1) );  \
    checkState( "x", tc);                                   \
    BOOST_CHECK_EQUAL( name, sm->getCurrentStateName() );   \
    BOOST_CHECK( SimulationThread::Instance()->run(100) );  \
    checkState( "x", tc);                                   \
    BOOST_CHECK_EQUAL( name, sm->getCurrentStateName() )

    DO_EVENT(v2, "STATE2");
    DO_EVENT(v1, "STATE1");
    DO_EVENT(v1, "STATE1");     // no change
    DO_EVENT(v3, "STATE3");
    DO_EVENT(v,  "IDLE");
    DO_EVENT(v3, "STATE3");
    DO_EVENT(v,  "IDLE");
    DO_EVENT(v1, "STATE1");

#undef DO_EVENT

    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationSignalTransitionProgram )
{
    string prog = string("StateMachine X {\n")
        + " var   double et = 0.0\n"
        + " initial state INIT {\n"
        + "    transition o_event(et) { test.i = 5; } select FINI\n" // test program
        + " }\n"
        + " final state FINI {} \n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    //checkState( prog, tc);
    this->runState("x", tc);
    checkState( "x", tc);
    // transition to FINI:
    OperationCaller<void(double)> mo( tc->provides()->getOperation("o_event"), tc->engine());
    mo(3.33);
    mo(6.33); // should be ignored
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(10) );
    BOOST_CHECK_EQUAL( i, 5 );
    checkState( "x", tc);
    BOOST_CHECK( sm->inState("FINI") );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationSignalGuard )
{
    string prog = string("StateMachine X {\n")
        + " var   double et = 0.0\n"
        + " initial state INIT {\n"
        + "    transition o_event(et) if (et == 3.33) then \n"
        + "        select FINI\n" // test guard
        + "        else {log(\"received et != 3.33: \"+et+\": will fail.\") } select FAIL\n" // test guard
        + " }\n"
        + " final state FINI {} \n"
        + " state FAIL {} \n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    //checkState( prog, tc);
    this->runState("x", tc);
    checkState( "x", tc);
    // transition to FINI:
    OperationCaller<void(double)> mo( tc->provides()->getOperation("o_event"), GlobalEngine::Instance() );
    mo(3.33);
    mo(6.33); // should be ignored
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(10) );
    checkState( "x", tc);
    BOOST_CHECK( sm->inState("FINI") );
    this->checkState("x",tc);
    this->finishState("x", tc);
}

BOOST_AUTO_TEST_CASE( testStateOperationCallerSignalGuard )
{
    string prog = string("StateMachine X {\n")
        + " var   double et = 0.0\n"
        + " initial state INIT {\n"
        + "    transition caller.c_event(et) if (et == 3.33) then\n"
        + "       select FINI\n"
        + "       else select FAIL\n" // test guard
        + " }\n"
        + " final state FINI {} \n"
        + " state FAIL {} \n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    //checkState( prog, tc);
    this->runState("x", tc);
    checkState( "x", tc);
    // causes error state when received in INIT:
    OperationCaller<void(double)> mo( caller->provides()->getOperation("c_event") );
    mo(3.33);
    checkState( "x", tc);
    BOOST_CHECK( SimulationThread::Instance()->run(1000) );
    checkState( "x", tc);
    BOOST_CHECK( sm->inState("FINI") );
    this->checkState("x",tc);
    this->finishState("x", tc);
}
#endif

BOOST_AUTO_TEST_CASE( testStateEvents)
{
    // test event reception in sub states.
    string prog = string("StateMachine Y {\n")
        + " var   int t = 0\n"
        + " var   double et_global = 0.0, et_local = 0.0\n"
        + " var   bool eb = false\n"
        + " var   bool eflag = false\n"
        + " transition t_event(t) { do log(\"Global Transition to TESTSELF\");} select TESTSELF\n" // test self transition
        + " transition d_event(et_global)\n"
        + "     if et_global < 0. then { do log(\"Global ISNEGATIVE Transition\");} select ISNEGATIVE\n"
        + "     else { do log(\"Global ISPOSITIVE Transition\");} select ISPOSITIVE\n" // NewData == false !!!
        + " initial state INIT {\n"
        + "   entry { do log(\"INIT\"); set eb = false; }\n"
        + " }\n"
        + " state ISNEGATIVE {\n"
        + "   entry { do log(\"ISNEGATIVE\");}\n"
        + "   transition b_event(eb)\n"
        + "      if (eb) then { do log(\"Local ISNEGATIVE->INIT Transition\");} select INIT\n"
        + " }\n"
        + " state ISPOSITIVE {\n"
        + "   entry { do log(\"ISPOSITIVE\");}\n"
        + "   transition b_event(eb)\n" // 20
        + "      if (eb == true) then { do log(\"Local ISPOSITIVE->INIT Transition for b_event\");} select INIT\n"
#ifdef ORO_SIGNALLING_OPERATIONS
        + "   transition o_event(et_local)\n"
        + "      if ( et_local == 3.0 ) then { do log(\"Local ISPOSITIVE->INIT Transition for o_event == \" + et_local);} select INIT\n"
        + "         else { do log(\"Invalid et_local: \"+et_local); } \n"
#endif
        + " }\n"
        + " state TESTSELF {\n"
        + "   entry {\n"
        + "      do log(\"TESTSELF\");\n"
        + "      set eflag = !eflag\n"
        + "   }\n"
        + "   transition t_event(t) { do log(\"Self Transition in TESTSELF\");} select TESTSELF\n"     // does not execute entry {}, overrides global t_event()
        + "   transition b_event(eb)\n"
        + "      if (eb == true) then { do log(\"Local TESTSELF->INIT Transition\");} select INIT\n"
        + "      else { log(\"Failed to select INIT upon event.\");}\n"
        + " }\n"
        + " final state FINI {\n"
        + "   entry { do log(\"FINI\");}\n"
        + " }\n"
        + " }\n" // 40
        + string("StateMachine X {\n") // 1
        + " SubMachine Y y1()\n"
        + " initial state XINIT {\n"
        + " entry {\n"
        + "     do y1.trace(true)\n"
        + "     do y1.activate()\n"
        + "     do y1.start()\n"
        + "     do yield\n"
        + " }\n"
        + " run {\n"

        + "     do d_event_source.write(-1.0)\n" // 11
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( y1.inState(\"ISNEGATIVE\") )\n"
        + "     do b_event_source.write( true )\n" // go to INIT.
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do d_event_source.write(+1.0)\n" // 21

        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     if ( !y1.inState(\"ISPOSITIVE\") ) then\n"
        + "          do test.assertMsg( false, \"Not ISNEGATIVE but \" + y1.getState() )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do b_event_source.write( true )\n" // go to INIT.
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n" // 31
#ifdef ORO_SIGNALLING_OPERATIONS
        // test operation
        + "     do d_event_source.write(+1.0)\n"

        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     if ( !y1.inState(\"ISPOSITIVE\") ) then\n"
        + "          do test.assertMsg( false, \"Not ISPOSITIVE but \" + y1.getState() )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do o_event( 3.0 )\n" // go to INIT.
        + "     do yield\n"
        + "     do yield\n"
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
#endif
        // test self transitions
        + "     set y1.eflag = true;\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eflag == false ) /* first */\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eflag == false ) /* second */\n" // no entry
        + "     do log(\"Trigger b_event.\");\n"
        + "     do b_event_source.write(true);\n"
        + "     yield;\n"
        + "     do test.assert( y1.inState(\"INIT\") ) /* last */\n"
        + " }\n"
        + " transitions {\n"
        + "     select XFINI\n"
        + " }\n"
        + " }\n"
        + " final state XFINI {\n"
        + " entry {\n"
        + "     do y1.deactivate()\n"
        //+ "     do test.assert(false)\n"
        + " }\n"
        + " transitions {\n"
        + "     select XINIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x() \n" // instantiate a hierarchical SC
        ;

     this->doState("x", prog, tc );
     //BOOST_CHECK( tc->engine()->states()->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}


BOOST_AUTO_TEST_CASE( testStateLevelEvents)
{
    // test event reception in sub states.
    string prog = string("StateMachine Y {\n")
        + " var   int t = 0\n"
        + " var   double et_global = 0.0, et_local = 0.0\n"
        + " var   bool eb = false\n"
        + " var   bool eflag = false\n"
        + " transition if ( t_event.read(t) == NewData && t == 1 ) then { do log(\"Global Transition to TESTSELF\");} select TESTSELF\n" // test self transition
        + " transition d_event(et_global) if ( et_global < 0.) then \n"
        + "     { do log(\"Global ISNEGATIVE Transition\");} select ISNEGATIVE\n"
        + "     else { do log(\"Global ISPOSITIVE Transition\");} select ISPOSITIVE\n"
        + " initial state INIT {\n"
        + "   entry { do log(\"INIT\"); set eb = false; }\n"
        + " }\n"
        + " state ISNEGATIVE {\n"
        + "   entry { do log(\"ISNEGATIVE\");}\n"
        + "   transition if ( b_event.read(eb) != NoData && eb )\n" // once eb is true (or was true already), transition
        + "      then { do log(\"Local ISNEGATIVE->INIT Transition\");} select INIT\n"
        + " }\n"
        + " state ISPOSITIVE {\n"
        + "   entry { do log(\"ISPOSITIVE\");}\n"
        + "   transition if ( b_event.read(eb) != NoData && eb == true) \n" // 20
        + "      then { do log(\"Local ISPOSITIVE->INIT Transition for b_event\");} select INIT\n"
#ifdef ORO_SIGNALLING_OPERATIONS
        + "   transition o_event(et_local) if ( et_local == 3.0)\n"
        + "      then { do log(\"Local ISPOSITIVE->INIT Transition for o_event == \" + et_local);} select INIT\n"
        + "      else { do log(\"Local ISPOSITIVE->INIT Transition FAILED for o_event == \" + et_local);}\n"
#endif
        + " }\n"
        + " state TESTSELF {\n"
        + "   entry {\n"
        + "      do log(\"TESTSELF\");\n"
        + "      set eflag = !eflag\n"
        + "   }\n"
        + "   transition if ( t_event.read(t) == NewData ) { do log(\"Self Transition in TESTSELF\");} select TESTSELF\n"     // does not execute entry {}, overrides global t_event()
        + "   transition if ( b_event.read(eb) == NewData && eb == true )\n"
        + "      then { do log(\"Local TESTSELF->INIT Transition\");} select INIT\n"
        + "      else { log(\"Failed to select INIT upon event.\");}\n"
        + " }\n"
        + " final state FINI {\n"
        + "   entry { do log(\"FINI\");}\n"
        + " }\n"
        + " }\n" // 40
        + string("StateMachine X {\n") // 1
        + " SubMachine Y y1()\n"
        + " initial state INIT {\n"
        + " entry {\n"
        + "     do y1.trace(true)\n"
        + "     do y1.activate()\n"
        + "     do y1.start()\n"
        + "     do yield\n"
        + " }"
        + " run {\n"

        + "     do d_event_source.write(-1.0)\n" // 11
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( y1.inState(\"ISNEGATIVE\") )\n"
        + "     do b_event_source.write( true )\n" // go to INIT.
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do b_event_source.write( false )\n" // clear the b_event for level sake

        + "     do d_event_source.write(+1.0)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"//21
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     if ( !y1.inState(\"ISPOSITIVE\") ) then\n"
        + "          do test.assertMsg( false, \"Not ISNEGATIVE but \" + y1.getState() )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do b_event_source.write( true )\n" // go to INIT.
        + "     do yield\n"

        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do b_event_source.write( false )\n" // clear the b_event for level sake
#ifdef ORO_SIGNALLING_OPERATIONS
        // test operation
        + "     do d_event_source.write(+1.0)\n"
        + "     do nothing\n" // 31
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     if ( !y1.inState(\"ISPOSITIVE\") ) then\n"
        + "          do test.assertMsg( false, \"Not ISPOSITIVE but \" + y1.getState() )\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do o_event( 3.0 )\n" // go to INIT.
        + "     do yield\n"
        + "     do yield\n" // 40
        + "     do yield\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
#endif
        // test self transitions
        + "     set y1.eflag = true;\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eflag == false ) /* first */\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eflag == false ) /* second */\n" // no entry
        + "     do log(\"Trigger b_event.\");\n"
        + "     do b_event_source.write(true);\n"
        + "     yield;\n"
        + "     do test.assert( y1.inState(\"INIT\") ) /* last */\n"
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do y1.deactivate()\n"
        //+ "     do test.assert(false)\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x() \n" // instantiate a hierarchical SC
        ;

     this->doState("x", prog, tc, true, 100 );
     //BOOST_CHECK( tc->engine()->states()->getStateMachine( "x" )->inState("FINI") );
     this->finishState( "x", tc);
}

BOOST_AUTO_TEST_CASE( testSelfDeactivatingStateMachineinEntry )
{
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " transitions {select FINI} \n"
        + " }\n"
        + " final state FINI {\n"
        + "  entry{\n"
        + "   try scripting.deactivateStateMachine(\"x\")\n"
        + "  }\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    // causes deactivation of SM:
    runState( "x", tc, false);//without trace
    BOOST_CHECK( !sm->isActive() );
    // causes deactivation of SM:
    runState( "x", tc, true);//with trace
    BOOST_CHECK( !sm->isActive() );

}

BOOST_AUTO_TEST_CASE( testSelfDeactivatingStateMachineinRun )
{
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " transitions {select FINI} \n"
        + " }\n"
        + " final state FINI {\n"
        + "  run{\n"
        + "   try scripting.deactivateStateMachine(\"x\")\n"
        + "  }\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    // causes deactivation of SM:
    runState( "x", tc, false);//without trace
    BOOST_CHECK( !sm->isActive() );
    // causes deactivation of SM:
    runState( "x", tc, true);//with trace
    BOOST_CHECK( !sm->isActive() );
}

BOOST_AUTO_TEST_CASE( testSelfDeactivatingStateMachineinHandle )
{
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + "  handle{\n"
        + "   try scripting.deactivateStateMachine(\"x\")\n"
        + "  }\n"
        + " transition if false == true then select FINI \n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    // causes deactivation of SM:
    runState( "x", tc, false);//without trace
    BOOST_CHECK( !sm->isActive() );
    // causes deactivation of SM:
    runState( "x", tc, true);//with trace
    BOOST_CHECK( !sm->isActive() );
}

BOOST_AUTO_TEST_CASE( testSelfDeactivatingStateMachineinExit )
{
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + "  exit{\n"
        + "   try scripting.deactivateStateMachine(\"x\")\n"
        + "  }\n"
        + " transitions {select FINI} \n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    // causes deactivation of SM:
    runState( "x", tc, false);//without trace
    BOOST_CHECK( !sm->isActive() );
    // causes deactivation of SM:
    runState( "x", tc, true);//with trace
    BOOST_CHECK( !sm->isActive() );
}

BOOST_AUTO_TEST_CASE( testSelfDeactivatingStateMachineinTransition )
{
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + "  transition if true then {\n"
        + "   scripting.deactivateStateMachine(\"x\")\n"
        + "  }select FINI \n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + "}\n"
        + "RootMachine X x()\n";
    this->parseState( prog, tc );
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    // causes deactivation of SM:
    runState( "x", tc, false);//without trace
    BOOST_CHECK( !sm->isActive() );
    // causes deactivation of SM:
    runState( "x", tc, true);//with trace
    BOOST_CHECK( !sm->isActive() );
}

BOOST_AUTO_TEST_SUITE_END()

void StateTest::doState(  const std::string& name, const std::string& prog, TaskContext* tc, bool test, int runs )
{
    BOOST_CHECK( tc->engine() );

    parseState( prog, tc, test);
    runState(name, tc, true, test,runs);
    checkState(name, tc, test);
}

void StateTest::parseState(const std::string& prog, TaskContext* tc, bool test )
{
    // Alternative way: test ScriptingService as well.
    try {
        sa->loadStateMachines( prog, std::string("state_test.cpp"), true );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( !test, exc.what() );
        }
    catch( const parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( !test, exc.what() );
        }
    catch( const program_load_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( !test, e.what() );
        }
    catch( const std::exception& e ) {
            BOOST_CHECK_MESSAGE( !test , e.what());
            BOOST_REQUIRE_MESSAGE( !test, "Uncaught Processor load exception" );
    }
}

void StateTest::runState(const std::string& name, TaskContext* tc, bool trace, bool test, int runs )
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    sm->trace(trace);
    StateMachine::ChildList children = sm->getChildren();
    for( StateMachine::ChildList::iterator it = children.begin(); it != children.end(); ++it)
        (*it)->trace(trace);
    OperationCaller<bool(StateMachinePtr)> act = tc->provides(name)->getOperation("activate");
    OperationCaller<bool(StateMachinePtr)> autom = tc->provides(name)->getOperation("automatic");
    BOOST_CHECK( act(sm) );
    BOOST_CHECK( SimulationThread::Instance()->run(1) );
    BOOST_CHECK_MESSAGE( sm->isActive(), "Error : Activate Command for '"+sm->getName()+"' did not have effect." );
    BOOST_CHECK( autom(sm) || !test  );

    BOOST_CHECK( SimulationThread::Instance()->run(runs) );
}

void StateTest::checkState(const std::string& name, TaskContext* tc, bool test )
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    if ( test ) {
        // check error status of parent :
        BOOST_CHECK_MESSAGE( sm->isActive(), "Error : State Context '"+sm->getName()+"' did not get activated." );
        stringstream errormsg;
        int line = sm->getLineNumber();
        errormsg <<" in StateMachine "+sm->getName()
                 <<" in state "<< (sm->currentState() ? sm->currentState()->getName() : "(null)")
                 <<" on line " << line <<" of that StateMachine:"<<endl;
        {
            stringstream sctext( sm->getText() );
            int cnt = 1;
            while ( cnt++ <line && sctext ) {
                string garbage;
                getline( sctext, garbage, '\n' );
            }
            getline( sctext, sline, '\n' );
        }
        errormsg <<"here  > " << sline << endl;
        if ( sm->inError() ) {
            RTT::scripting::DumpObject( tc->provides() );
            RTT::scripting::DumpObject( tc->provides(name) );
        }
        BOOST_CHECK_MESSAGE( sm->inError() == false, "Runtime error (inError() == true) encountered" + errormsg.str() );
        // check error status of all children:
        StateMachine::ChildList cl = sm->getChildren();
        StateMachine::ChildList::iterator clit = cl.begin();
        while( clit != cl.end() ) {
            line = (*clit)->getLineNumber();
            {
                stringstream sctext( (*clit)->getText() );
                int cnt = 1;
                while ( cnt++ <line && sctext ) {
                    string garbage;
                    getline( sctext, garbage, '\n' );
                }
                getline( sctext, sline, '\n' );
            }
            stringstream cerrormsg;
            if ( (*clit)->currentState() )
                cerrormsg <<" in child "<< (*clit)->getName() <<" in state "<<(*clit)->currentState()->getName()<< " on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl <<"here  > " << sline << endl;
            else
                cerrormsg <<" child "<< (*clit)->getName() << " (deactivated) on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl<<"here  > " << sline << endl;

            BOOST_CHECK_MESSAGE( (*clit)->inError() == false, "Runtime error (inError() == true) encountered" + cerrormsg.str() );
            if ( (*clit)->inError() == false && sm->inError() == true) {
                cout << "Child Status:" << cerrormsg.str() << endl;
            }
            ++clit;
        }
    }
}

void StateTest::finishState(std::string const& name, TaskContext* tc, bool test)
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    BOOST_CHECK( sa->getStateMachine( name )->stop() );
    BOOST_CHECK( SimulationThread::Instance()->run(10) );
    if (test) {
        stringstream errormsg;
        errormsg << " on line " << sm->getLineNumber() <<", status is "<< sa->getStateMachineStatusStr(name) <<endl <<"here  > " << sline << endl;;
        BOOST_CHECK_MESSAGE( sm->isStopped(), "StateMachine stalled " + errormsg.str() );
    }
    // you can call deactivate even when the proc is not running.
    // but deactivation may be 'in progress if exit state has commands in it.
    BOOST_CHECK( sa->getStateMachine( name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(10) );
    if ( sm->isActive() )
        BOOST_CHECK( sa->getStateMachine( name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(10) );
    BOOST_CHECK( sa->getStateMachine( name )->isActive() == false );

    // only stop now, since deactivate won't work if simtask not running.
    tc->stop();

    try {
        BOOST_CHECK( sa->unloadStateMachine( name ) );
    }
    catch( const program_unload_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( false, e.what() );
        }
    catch( ... ) {
            BOOST_REQUIRE_MESSAGE( false, "Uncaught Processor unload exception" );
    }

}

