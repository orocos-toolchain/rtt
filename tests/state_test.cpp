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
 
 

#include "state_test.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#ifndef NOPARSER
#include <execution/ParsedStateMachine.hpp>
#endif
#include <corelib/SimulationThread.hpp>
#include <corelib/CommandFunctor.hpp>
#include <execution/TemplateFactories.hpp>
#include <execution/StateMachine.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StateTest );


    StateTest::StateTest()
        :gtc("root"),
         gtask( 0.01, gtc.getExecutionEngine() )
    {}


void 
StateTest::setUp()
{
    // ltc has a test object
    gtc.methodFactory.registerObject("test", this->createMethodFactory() );
    gtc.commandFactory.registerObject("test", this->createCommandFactory() );
    gtc.dataFactory.registerObject("test", this->createDataSourceFactory() );

    gtc.eventService.addEvent( "d_event", &d_event );
    gtc.eventService.addEvent( "b_event", &b_event );
    i = 0;
}


void 
StateTest::tearDown()
{
    // if a test failed, we must still stop :
    SimulationThread::Instance()->stop();
    gtask.stop();
    gtc.methodFactory.unregisterObject( "test" );
    gtc.commandFactory.unregisterObject( "test" );
    gtc.dataFactory.unregisterObject( "test" );
}


bool StateTest::assertBool( bool b) { 
    return b;
}
bool StateTest::assertMsg( bool b, const std::string& msg) {
    if ( b == false )
        cout << "Asserted :" << msg << endl;
    return true; // allow to continue to check other commands.
}

int StateTest::increase() {
    return ++i;
}

void StateTest::reset() {
    i = 0;
}


MethodFactoryInterface* StateTest::createMethodFactory()
{
    // Add the data of the EE:
    TemplateMethodFactory< StateTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &StateTest::assertBool,
                              "Assert", "bool", "") );
    dat->add( "increase", method( &StateTest::increase,
                                "Return increasing i" ) );
    dat->add( "reset", method( &StateTest::reset,
                              "Reset i") );
    dat->add( "assertMsg", method( &StateTest::assertMsg,
                                 "Assert message", "bool", "", "text", "text" ) );
    return dat;
}

DataSourceFactoryInterface* StateTest::createDataSourceFactory()
{
    // Add the data of the EE:
    TemplateDataSourceFactory< StateTest >* dat =
        newDataSourceFactory( this );

    dat->add( "isTrue", data( &StateTest::assertBool,
                              "Identity function", "bool", "") );
    dat->add( "i", data( &StateTest::i,
                         "Return the current number" ) );
    return dat;
}

CommandFactoryInterface* StateTest::createCommandFactory()
{
    // Add the data of the EE:
    TemplateCommandFactory< StateTest >* dat =
        newCommandFactory( this );

    dat->add( "instantDone", command( &StateTest::true_genCom,
                                      &StateTest::true_gen,
                                      "returns immediately") );
    dat->add( "neverDone", command( &StateTest::true_genCom,
                                    &StateTest::false_gen,
                                    "returns never") );
    dat->add( "instantNotDone", command( &StateTest::true_genCom,
                                         &StateTest::true_gen,
                                         "returns never", false ) );
    dat->add( "instantFail", command( &StateTest::false_genCom,
                                      &StateTest::true_gen,
                                      "fails immediately") );
    dat->add( "totalFail", command( &StateTest::false_genCom,
                                    &StateTest::false_gen,
                                    "fails in command and condition") );
    return dat;
}


void StateTest::testParseState()
{
    // a state which should never fail
    string prog = string("StateMachine X {\n")
        + " param int isten\n"
        + " var bool istrue = true\n"
        + " param bool isfalse\n"
        + " param bool isok\n"
        + " param double isnegative\n"
        + " var double d_dummy = -1.0\n"
        + " var int    i_dummy = -1\n"
        + " var bool   varinit = false\n"
        + " initial state INIT {\n"
        // XXX bug : preconditions are not checked in the initial state.
//         + " preconditions {\n"
//         + "     if (istrue == false) || (isfalse == true) || (isten != 10) ||( isnegative >= 0. )  then select PRE_PARAMFAIL\n"
//         + "     if false then select PRE_ERROR\n"
//         + "     if (isnegative != -1.) then select PRE_PARAMFAIL\n"
//         + "     if (istrue != true) then select PRE_PARAMFAIL\n"
//         + "     if (isfalse != false) then select PRE_PARAMFAIL\n"
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
        + "     do test.instantDone()\n"
        + " }\n"
        + " exit {\n"
        + "     do test.instantDone()\n"
        + "     set d_dummy = 0.0\n"
        + "     set i_dummy = 0\n"
        + " }\n"
        + " transitions {\n"
        + "     if false then select ERROR\n"
        + "     if (d_dummy != 1.234) || (i_dummy != -2)  then select ENTRYFAIL\n"
        + "     if (istrue == false) || (isfalse == true) || (isten != 10) ||( isnegative >= 0. )  then select PARAMFAIL\n"
        + "     if isok == false then select PARAMFAIL\n"
        + "     select FINI\n"
        + "     select ERROR\n" // do not reach
        + " }\n"
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
        + "      do test.assertMsg( istrue == true, \"istrue parameter not correctly initialised\")\n"
        + "      do test.assertMsg( isfalse == false, \"isfalse parameter not correctly initialised\")\n"
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
        //        + " RootMachine X x( isten = 10, isfalse = false, isnegative = -1.0) \n" // instantiate a non hierarchical SC
        + " RootMachine X x( isten = 10, isok = true, isfalse = false, isnegative = -1.0) \n" // instantiate a non hierarchical SC
        ;

    this->doState( prog, &gtc );
    this->finishState( &gtc, "x");
}

void StateTest::testStateFailure()
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
    while ( i < 6 ) {
        this->doState( prog, &gtc, false );

        // assert that an error happened :
        CPPUNIT_ASSERT( gtc.getExecutionEngine()->getStateMachineProcessor()->getStateMachineStatus("x") == Processor::StateMachineStatus::error );
        
        this->finishState( &gtc, "x");
    }
}
void StateTest::testStateChildren()
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

void StateTest::testStateEmpty()
{
    // test processing of completely empty states
    string prog = string("StateMachine X {\n")
        + " initial state INIT {\n"
        + " transitions {\n"
        + "     select TEST\n" // only a transition
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

void StateTest::testStateSubStateVars()
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
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
        + "     do y1.stop()\n"
        + " }\n"
        + " exit {\n"
        + "     set y1.isnegative = -1.0 \n"
        + "     do y1.deactivate()\n"
        + " }\n"
        + " transitions {\n"
        + "     select INIT\n"
        + " }\n"
        + " }\n"
        + " }\n"
        + " RootMachine X x( isnegative = -1.0) \n" // instantiate a hierarchical SC
        ;

     this->doState( prog, &gtc );
     this->finishState( &gtc, "x");
}

void StateTest::testStateSubStateCommands()
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
        + string("StateMachine X {\n")
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
        // 40 :
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

void StateTest::testStateEvents()
{
    // test event reception in sub states.
    string prog = string("StateMachine Y {\n")
        + " var   double t = 0.0\n"
        + " var   double et = 0.0\n"
        + " var   bool eb = false\n"
        + " initial state INIT {\n"
        + " transition d_event(et)\n"
        + "     if et < 0. then select ISNEGATIVE\n"
        + "     else select ISPOSITIVE\n"
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
        + " state DEFAULT {\n"
        + " transitions {\n"
        + "      select FINI\n"
        + "      }\n"
        + " }\n"
        + " final state FINI {\n"
        + " }\n"
        + " }\n" // 40
        + string("StateMachine X {\n")
        + " SubMachine Y y1()\n"
        + " initial state INIT {\n"
        + " run {\n"
        + "     do y1.activate()\n"
        + "     emit d_event(-1.0)\n"
        + "     do nothing\n"
        + "     do test.assert( !y1.inState(\"INIT\") )\n"
        + "     do test.assert( !y1.inState(\"ISPOSITIVE\") )\n"
        + "     do test.assert( y1.inState(\"ISNEGATIVE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     emit d_event(+1.0)\n"
//         + "     do nothing\n"
        + "     do test.assert( y1.inState(\"ISPOSITIVE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     emit b_event(true)\n"
//         + "     do nothing\n"
        + "     do test.assert( y1.inState(\"ISTRUE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + "     emit b_event(false)\n"
//         + "     do nothing\n"
        + "     do test.assert( y1.inState(\"ISFALSE\") )\n"
        + "     do y1.requestState(\"INIT\")\n"
        + "     do test.assert( y1.inState(\"INIT\") )\n"
        + " }\n"
        + " transitions {\n"
        + "     select FINI\n"
        + " }\n"
        + " }\n"
        + " final state FINI {\n"
        + " entry {\n"
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

void StateTest::testStateUntil()
{
//     this->doState( prog, &gtc );
//     this->finishState( &gtc, "x");
}

void StateTest::testStateUntilFail()
{
//     this->doState( prog, &gtc, false );

//     CPPUNIT_ASSERT( gprocessor.getStateMachineStatus("x") == Processor::StateMachineStatus::error );

//     this->finishState( &gtc, "x");
}

void StateTest::doState( const std::string& prog, TaskContext* tc, bool test )
{
    stringstream progs(prog);
#ifndef NOPARSER
    std::vector<ParsedStateMachine*> pg_list;
#else
    std::vector<StateMachine*> pg_list;
    sleep(5);
#endif
    try {
#ifndef NOPARSER
        pg_list = parser.parseStateMachine( progs, tc );
#endif
    }
#ifndef NOPARSER
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE( exc.what(), false );
        }
    catch( const parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE( exc.what(), false );
        }
#endif
    catch( ... ) {
            CPPUNIT_ASSERT_MESSAGE( "Uncaught Parse Exception", false );
    }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    try {
        tc->getProcessor()->loadStateMachine( *pg_list.begin() );
    } 
    catch( const program_load_exception& e)
        {
            CPPUNIT_ASSERT_MESSAGE( e.what(), false );
        }
    catch( ... ) {
            CPPUNIT_ASSERT_MESSAGE( "Uncaught Processor load exception", false );
    }
    CPPUNIT_ASSERT( SimulationThread::Instance()->start() );
    CPPUNIT_ASSERT( gtask.start() );
    CommandInterface* ca = newCommandFunctor(boost::bind(&Processor::activateStateMachine, tc->getProcessor(),(*pg_list.begin())->getName() ));
    CommandInterface* cs = newCommandFunctor(boost::bind(&Processor::startStateMachine,tc->getProcessor(),(*pg_list.begin())->getName() ) );
//      cerr << "Before activate :"<<endl;
//      tc->getPeer("states")->getPeer("x")->debug(true);
    CPPUNIT_ASSERT( ca->execute()  );
    CPPUNIT_ASSERT_MESSAGE( "Error : Activate Command for '"+(*pg_list.begin())->getName()+"' did not have effect.", (*pg_list.begin())->isActive() );
//      cerr << "After activate :"<<endl;
//      tc->getPeer("states")->getPeer("x")->debug(true);
    CPPUNIT_ASSERT( gtc.getExecutionEngine()->getCommandProcessor()->process( cs ) != 0 );
//     while (1)
    sleep(1);
    delete ca;
    delete cs;
//     cerr << "After run :"<<endl;
//     tc->getPeer("states")->getPeer("x")->debug(true);
// //     tc->getPeer("__states")->getPeer("X")->debug(false);
//     if( tc->getPeer("__states")->getPeer("Y"))
//         tc->getPeer("__states")->getPeer("Y")->debug(false);
//     cerr << "               x.y1 running : "<< (gprocessor.getStateMachineStatus("x.y1") == Processor::StateMachineStatus::running) << endl;
//     cerr << "               x running : "<< (gprocessor.getStateMachineStatus("x") == Processor::StateMachineStatus::running) << endl;
    if (test ) {
        // check error status of parent :
        CPPUNIT_ASSERT_MESSAGE( "Error : State Context '"+(*pg_list.begin())->getName()+"' did not get activated.", (*pg_list.begin())->isActive() );
        stringstream errormsg;
        int line = (*pg_list.begin())->getLineNumber();
        errormsg <<" in StateContext "+(*pg_list.begin())->getName()
                 <<" in state "<< (*pg_list.begin())->currentState()->getName()
                 <<" on line " << line <<" of that StateContext:"<<endl;
        stringstream sctext( (*pg_list.begin())->getText() );
        int cnt = 1;
        while ( cnt++ <line && sctext ) {
            string garbage;
            getline( sctext, garbage, '\n' );
        }
        string sline;
        getline( sctext, sline, '\n' );
        errormsg <<"here  > " << sline << endl;
        CPPUNIT_ASSERT_MESSAGE( "Runtime error encountered" + errormsg.str(), (*pg_list.begin())->inError() == false );
        // check error status of all children:
        StateMachine::ChildList cl = (*pg_list.begin())->getChildren();
        StateMachine::ChildList::iterator clit = cl.begin();
        while( clit != cl.end() ) {
            stringstream cerrormsg;
            if ( (*clit)->currentState() )
                cerrormsg <<" in state "<<(*clit)->currentState()->getName()<< " on line " <<  (*clit)->getLineNumber() <<" of that StateContext."<<endl;
            else
                cerrormsg <<" (deactivated) on line " <<  (*clit)->getLineNumber() <<" of that StateContext."<<endl;
            CPPUNIT_ASSERT_MESSAGE( "Runtime error encountered in child "+(*clit)->getName() + cerrormsg.str(), (*clit)->inError() == false );
            ++clit;
        }
    }
    tc->getProcessor()->stopStateMachine( (*pg_list.begin())->getName() );
    sleep(1);
    // after gtask.stop() it must be stopped.
    CPPUNIT_ASSERT( gtask.stop() );
    SimulationThread::Instance()->stop();
    stringstream errormsg;
    errormsg << " on line " << (*pg_list.begin())->getLineNumber() <<", status is "<< int(gtc.getExecutionEngine()->getStateMachineProcessor()->getStateMachineStatus("x")) <<endl;
    CPPUNIT_ASSERT_MESSAGE( "StateMachine stalled " + errormsg.str(), gtc.getExecutionEngine()->getStateMachineProcessor()->getStateMachineStatus("x") == Processor::StateMachineStatus::stopped );
}

void StateTest::finishState(TaskContext* tc, std::string prog_name)
{
    // you can call deactivate even when the proc is not running.
    // but deactivation may be 'in progress if exit state has commands in it.
    CPPUNIT_ASSERT( tc->getProcessor()->deactivateStateMachine( prog_name ) );
    try {
        tc->getProcessor()->deleteStateMachine( prog_name );
    }
    catch( const program_unload_exception& e)
        {
            CPPUNIT_ASSERT_MESSAGE( e.what(), false );
        }
    catch( ... ) {
            CPPUNIT_ASSERT_MESSAGE( "Uncaught Processor unload exception", false );
    }

    TaskContext* ptc = tc->getPeer("states")->getPeer(prog_name);
    tc->getPeer("states")->removePeer(prog_name);
    delete ptc;

    ptc= tc->getPeer("states");
    tc->removePeer("states");
    delete ptc;
}

    

