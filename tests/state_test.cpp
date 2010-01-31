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



#include <rtt-config.h>
#include <extras/SimulationThread.hpp>
#include <extras/SimulationActivity.hpp>
#include <scripting/StateMachine.hpp>
#include <scripting/ParsedStateMachine.hpp>
#include <scripting/DumpObject.hpp>
#include <scripting/Parser.hpp>

#include <interface/ServiceProvider.hpp>
#include <TaskContext.hpp>
#include <Method.hpp>
#include <Port.hpp>
#include <scripting/ScriptingAccess.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <string>
#include <iostream>
#include <sstream>

using namespace RTT;
using namespace RTT::detail;
using namespace std;

class StateTest
{
public:
    Parser parser;
    TaskContext gtc;
    InputPort<double> d_event;
    InputPort<bool>   b_event;
    InputPort<int>    t_event;
    OutputPort<double> d_event_source;
    OutputPort<bool>   b_event_source;
    OutputPort<int>    t_event_source;
    ScriptingAccess* sa;
    //Event<void(void)> t_event;
    ServiceProvider* createObject(std::string);
    bool assertBool( bool b) { return b; }
    bool assertMsg( bool b, const std::string& msg) {
        if ( b == false )
            cout << "Asserted :" << msg << endl;
        return true; // allow to continue to check other commands.
    }
    int increase() { return ++i;}
    void resetI() { i = 0; }
    void doState( const std::string& prog, TaskContext*, bool test=true );
    void finishState( TaskContext* , std::string, bool test=true );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const {return i;}
    int i;
    std::string sline;
public:
    StateTest()
        :gtc("root"),
         d_event("d_event"), b_event("b_event"), t_event("t_event"),
         d_event_source("d_event_source"), b_event_source("b_event_source"), t_event_source("t_event_source"),
         sa( new ScriptingAccess(&gtc) )
    {
        gtc.setActivity( new SimulationActivity(0.001) );
#if 1
        // ltc has a test object
        gtc.addService(this->createObject("test") );

        gtc.ports()->addPort( &d_event );
        gtc.ports()->addPort( &b_event );
        gtc.ports()->addPort( &t_event );
        gtc.ports()->addPort( &d_event_source );
        gtc.ports()->addPort( &b_event_source );
        gtc.ports()->addPort( &t_event_source );
#endif
        gtc.start();
        i = 0;
        SimulationThread::Instance()->stop();
    }
    ~StateTest(){
    }
};

ServiceProvider* StateTest::createObject(string a)
{
    ServiceProvider* dat = new ServiceProvider(a);

    dat->addOperation("assert", &StateTest::assertBool, this).doc("Assert").arg("bool", "");
    dat->addOperation("increase", &StateTest::increase, this).doc("Return increasing i");
    dat->addOperation("resetI", &StateTest::resetI, this).doc("ResetI i");
    dat->addOperation("assertMsg", &StateTest::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
    dat->addOperation("isTrue", &StateTest::assertBool, this).doc("Identity function").arg("bool", "");
    dat->addOperation("i", &StateTest::getI, this).doc("Return the current number");
    dat->addOperation("instantDone", &StateTest::true_genCom, this).doc("returns immediately");
    dat->addOperation("instantDoneDone", &StateTest::true_gen, this).doc("Returns true when instantDone is done.");
    dat->addOperation("neverDone", &StateTest::true_genCom, this).doc("returns never");
    dat->addOperation("neverDoneDone", &StateTest::false_gen, this).doc("Returns true when neverDone is done.");
    dat->addOperation("instantNotDone", &StateTest::true_genCom,this).doc("returns never.");
    dat->addOperation("instantNotDoneDone", &StateTest::false_gen,this).doc("returns never.");
    dat->addOperation("instantFail", &StateTest::false_genCom, this).doc("fails immediately");
    dat->addOperation("instantFailDone", &StateTest::true_gen, this).doc("Returns true when instantFail is done.");
    dat->addOperation("totalFail", &StateTest::false_genCom, this).doc("fails in command and condition");
    dat->addOperation("totalFailDone", &StateTest::false_gen, this).doc("Returns true when totalFail is done.");
    return dat;
}

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
        + "     do test.instantDone()\n"
        + "     set d_dummy = 1.234\n"
        + "     set i_dummy = -2\n"
        + "     do test.instantDone()\n"
        + " }\n"
        + " handle {\n"
        + "     do test.instantDone()\n" // 20
        + " }\n"
        + " exit {\n"
        + "     do test.instantDone()\n"
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
        + "     do test.instantDone()\n"
        + "     set d_dummy = -1.\n"
        + "     set i_dummy = -1\n"
        + " }\n"
        + " handle {\n"
        + "     do test.instantDone()\n"
        + " }\n"
        + " exit {\n"
        + "     do test.instantDone()\n"
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

    this->doState( prog, &gtc );
    this->finishState( &gtc, "x");
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
        + "     do test.assert( test.i() != 1)\n" // fail if i == 1
        + " }\n"
        + " handle {\n"
        + "     do test.assert( test.i != 2)\n"
        + " }\n"
        + " exit {\n"
        + "     do test.assert( test.i != 3)\n"
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " state ERROR { entry { do test.assert(false) }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do test.assert( test.i != 4)\n"
        + " }\n"
        + " handle {\n"
        + "     do test.assert( test.i != 5)\n"
        + " }\n"
        + " exit {\n"
        + "     do test.assert( test.i != 6)\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    // should fail each time
    int x = 0;
    while ( i < 6 ) {
        this->doState( prog, &gtc, false );

        // assert that an error happened :
        BOOST_CHECK_MESSAGE( sa->getStateMachineStatus("x") == StateMachine::Status::error, "Status is: " + sa->getStateMachineStatusStr("x") );

        this->finishState( &gtc, "x", false);
        ++i;
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
        + "     do test.instantDone()\n"
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
        + "     do test.instantDone()\n"
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
        + "     do test.instantDone()\n"
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
        + "     do test.instantDone()\n"
        + "     do y1.stop()\n"
        + "     do y2.stop()\n"
        + "     do y3.stop()\n"
        + "     do z1.stop()\n"
        + " }\n"
        + " exit {\n"
        + "     do test.instantDone()\n"
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

    this->doState( prog, &gtc );
    this->finishState( &gtc, "x");
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
     this->doState( prog, &gtc );
     this->finishState( &gtc, "x");
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

    this->doState( prog, &gtc );
    this->finishState( &gtc, "x");
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
     this->doState( prog, &gtc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( &gtc, "x");
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
     this->doState( prog, &gtc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("NEXT") );
     this->finishState( &gtc, "x");
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
     this->doState( prog, &gtc );
     BOOST_CHECK( sa->getStateMachine( "x" )->inState("FINI") );
     this->finishState( &gtc, "x");
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
        + "     do y1.activate()\n"
        + "     set y1.t = -1.0 \n"
        + " }\n"
        + " exit {\n"
        + "     do y1.start()\n"
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

     this->doState( prog, &gtc );
     this->finishState( &gtc, "x");
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

     this->doState( prog, &gtc );
     this->finishState( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testStateEvents)
{
    // test event reception in sub states.
    string prog = string("StateMachine Y {\n")
        + " var   int t = 0\n"
        + " var   double et = 0.0\n"
        + " var   bool eb = false\n"
        + " transition t_event(t) select TESTSELF\n" // test self transition
        + " transition d_event(et)\n"
        + "     if et < 0. then  select ISNEGATIVE\n"
        + "     else select ISPOSITIVE\n"
        + " initial state INIT {\n"
        + " transition b_event(eb)\n"
        + "     if eb == false then select ISFALSE\n"
        + "     else select ISTRUE\n"
        + " }\n"
        + " state ISNEGATIVE {\n"
        + " transitions {\n"
        + "      select INIT\n"
        + " }\n"
        + " }\n"
        + " state ISPOSITIVE {\n"
        + " transitions {\n"
        + "      select INIT\n" // 20
        + "      }\n"
        + " }\n"
        + " state ISTRUE {\n"
        + " transitions {\n"
        + "      select INIT\n"
        + "      }\n"
        + " }\n"
        + " state ISFALSE {\n"
        + " transitions {\n"
        + "      select INIT\n"
        + " }\n"
        + " }\n"
        + " state TESTSELF {\n"
        + " entry {\n"
        + "      set eb = !eb\n"
        + " }\n"
        + " transition t_event(t) select TESTSELF\n"     // does not execute entry {}
        + " transition b_event(eb) { set eb = true; }\n"
        + " transition select INIT\n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + " }\n" // 40
        + string("StateMachine X {\n") // 1
        + " SubMachine Y y1()\n"
        + " initial state INIT {\n"
        + " run {\n"
        //        + "     do test.assert(false)\n"
        + "     do y1.activate()\n"
        + "     do d_event_source.write(-1.0)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( y1.inState(\"ISNEGATIVE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do d_event_source.write(+1.0)\n"
        + "     do nothing\n"
        + "     if ( !y1.inState(\"ISPOSITIVE\") ) then\n"
        + "          do test.assertMsg( false, y1.getState() )\n" // 15
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do b_event_source.write(true)\n"
        + "     do nothing\n"
        + "     if ( !y1.inState(\"ISTRUE\") ) then\n"
        + "          do test.assertMsg(false, y1.getState() )\n"
        + "     do test.assertMsg( y1.inState(\"ISTRUE\"), y1.getState() )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     do b_event_source.write(false)\n"
        + "     do nothing\n"
        + "     if ( !y1.inState(\"ISFALSE\") ) then\n"
        + "          do test.assertMsg(false, y1.getState() )\n"
        + "     do test.assert( y1.inState(\"ISFALSE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        // test self transitions
        + "     set y1.eb = true;\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( !y1.inState(\"ISNEGATIVE\") )\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eb == false )\n"
        + "     do t_event_source.write(1)\n"
        + "     do nothing\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eb == false )\n" // no entry
        + "     do b_event_source.write(true)\n"
        + "     do nothing\n"
        + "     do test.assert( y1.inState(\"TESTSELF\") )\n"
        + "     do test.assert( y1.eb == true )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        //+ "     do y1.deactivate()\n"
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

     this->doState( prog, &gtc );
     //BOOST_CHECK( gtc.engine()->states()->getStateMachine( "x" )->inState("FINI") );
     this->finishState( &gtc, "x");
}

BOOST_AUTO_TEST_SUITE_END()

void StateTest::doState( const std::string& prog, TaskContext* tc, bool test )
{
    BOOST_CHECK( tc->engine() );

#if 0
    // Classical way: use parser directly.
    Parser::ParsedStateMachines pg_list;
    try {
        pg_list = parser.parseStateMachine( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_CHECK_MESSAGE( false, exc.what() );
        }
    catch( const parse_exception& exc )
        {
            BOOST_CHECK_MESSAGE( false ,exc.what());
        }
    catch( ... ) {
            BOOST_CHECK_MESSAGE( false, "Uncaught Parse Exception" );
    }
    if ( pg_list.empty() )
        {
            BOOST_CHECK( false );
        }
#endif
    // Alternative way: test ScriptingAccess as well.
    try {
        sa->loadStateMachines( prog, std::string("state_test.cpp"), true );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( false, exc.what() );
        }
    catch( const parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( false, exc.what() );
        }
    catch( const program_load_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( false, e.what() );
        }
    catch( const std::exception& e ) {
            BOOST_CHECK_MESSAGE( false , e.what());
            BOOST_REQUIRE_MESSAGE( false, "Uncaught Processor load exception" );
    }
    StateMachinePtr sm = sa->getStateMachine("x");
    BOOST_REQUIRE( sm );
    Method<bool(StateMachine*)> act = tc->provides("x")->getOperation<bool(StateMachine*)>("activate");
    Method<bool(StateMachine*)> autom = tc->provides("x")->getOperation<bool(StateMachine*)>("automatic");
//      cerr << "Before activate :"<<endl;
//      tc->getPeer("states")->getPeer("x")->debug(true);
    //BOOST_CHECK( sm->activate() );
    BOOST_CHECK( act(sm.get()) );
    BOOST_CHECK( SimulationThread::Instance()->run(1) );
    BOOST_CHECK_MESSAGE( sm->isActive(), "Error : Activate Command for '"+sm->getName()+"' did not have effect." );
//      cerr << "After activate :"<<endl;
//      tc->getPeer("states")->getPeer("x")->debug(true);
    //BOOST_CHECK( sm->automatic() );
    BOOST_CHECK( autom(sm.get()) || !test  );
//     while (1)
    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

//     cerr << "After run :"<<endl;
//     tc->getPeer("states")->getPeer("x")->debug(true);
// //     tc->getPeer("__states")->getPeer("X")->debug(false);
//     if( tc->getPeer("__states")->getPeer("Y"))
//         tc->getPeer("__states")->getPeer("Y")->debug(false);
//     cerr << "               x.y1 running : "<< (gprocessor.getStateMachineStatus("x.y1") == Processor::StateMachineStatus::running) << endl;
//     cerr << "               x running : "<< (gprocessor.getStateMachineStatus("x") == Processor::StateMachineStatus::running) << endl;

    if (test ) {
        // check error status of parent :
        BOOST_CHECK_MESSAGE( sm->isActive(), "Error : State Context '"+sm->getName()+"' did not get activated." );
        stringstream errormsg;
        int line = sm->getLineNumber();
        errormsg <<" in StateMachine "+sm->getName()
                 <<" in state "<< sm->currentState()->getName()
                 <<" on line " << line <<" of that StateMachine:"<<endl;
        stringstream sctext( sm->getText() );
        int cnt = 1;
        while ( cnt++ <line && sctext ) {
            string garbage;
            getline( sctext, garbage, '\n' );
        }
        getline( sctext, sline, '\n' );
        errormsg <<"here  > " << sline << endl;
        if ( sm->inError() ) {
            RTT::detail::DumpObject( tc );
        }
        BOOST_CHECK_MESSAGE( sm->inError() == false, "Runtime error (inError() == true) encountered" + errormsg.str() );
        // check error status of all children:
        StateMachine::ChildList cl = sm->getChildren();
        StateMachine::ChildList::iterator clit = cl.begin();
        while( clit != cl.end() ) {
            stringstream cerrormsg;
            if ( (*clit)->currentState() )
                cerrormsg <<" in state "<<(*clit)->currentState()->getName()<< " on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl <<"here  > " << sline << endl;
            else
                cerrormsg <<" (deactivated) on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl<<"here  > " << sline << endl;
            BOOST_CHECK_MESSAGE( (*clit)->inError() == false, "Runtime error (inError() == true) encountered in child "+(*clit)->getName() + cerrormsg.str() );
            ++clit;
        }
    }
}

void StateTest::finishState(TaskContext* tc, std::string prog_name, bool test)
{
    StateMachinePtr sm = sa->getStateMachine(prog_name);
    BOOST_REQUIRE( sm );
    BOOST_CHECK( sa->getStateMachine( prog_name )->stop() );
    BOOST_CHECK( SimulationThread::Instance()->run(500) );
    if (test) {
        stringstream errormsg;
        errormsg << " on line " << sm->getLineNumber() <<", status is "<< sa->getStateMachineStatusStr(prog_name) <<endl <<"here  > " << sline << endl;;
        BOOST_CHECK_MESSAGE( sm->isStopped(), "StateMachine stalled " + errormsg.str() );
    }
    // you can call deactivate even when the proc is not running.
    // but deactivation may be 'in progress if exit state has commands in it.
    BOOST_CHECK( sa->getStateMachine( prog_name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(200) );
    if ( sm->isActive() )
        BOOST_CHECK( sa->getStateMachine( prog_name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(200) );
    BOOST_CHECK( sa->getStateMachine( prog_name )->isActive() == false );

    // only stop now, since deactivate won't work if simtask not running.
    tc->stop();

    try {
        BOOST_CHECK( sa->unloadStateMachine( prog_name ) );
    }
    catch( const program_unload_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( false, e.what() );
        }
    catch( ... ) {
            BOOST_REQUIRE_MESSAGE( false, "Uncaught Processor unload exception" );
    }

}

