/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  state_test.hpp

                        state_test.hpp -  description
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


#ifndef STATETEST_H
#define STATETEST_H


//#define NOPARSER
#ifndef NOPARSER
#include <scripting/Parser.hpp>
#endif

#include <TaskContext.hpp>
#include <string>
#include <SimulationActivity.hpp>

using namespace RTT;

class StateTest
{
	/*
    CPPUNIT_TEST_SUITE( StateTest );
    CPPUNIT_TEST( testParseState );
    //CPPUNIT_TEST( testStateFailure );
    CPPUNIT_TEST( testStateChildren );
    CPPUNIT_TEST( testStateEmpty );
    CPPUNIT_TEST( testStateSubStateVars );
    CPPUNIT_TEST( testStateSubStateCommands );
    CPPUNIT_TEST( testStateEvents );
    CPPUNIT_TEST( testStateTransitions );
    CPPUNIT_TEST( testStateGlobalTransitions );
//     CPPUNIT_TEST( testStateUntilFail );
    CPPUNIT_TEST_SUITE_END();
*/
public:
#ifndef NOPARSER
    Parser parser;
#endif
    TaskContext gtc;
    SimulationActivity gtask;
    Event<void(double)> d_event;
    Event<void(bool)> b_event;
    Event<void(void)> t_event;
    TaskObject* createObject(std::string);
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    int increase();
    void resetI();
    void doState( const std::string& prog, TaskContext*, bool test=true );
    void finishState( TaskContext* , std::string );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const {return i;}
    int i;
    std::string sline;
public:
    StateTest();
    ~StateTest(){ tearDown();};

    void setUp();
    void tearDown();

    void testParseState();
    void testStateFailure();
    void testStateChildren();
    void testStateEmpty();
    void testStateSubStateVars();
    void testStateSubStateCommands();
    void testStateEvents();
    void testStateUntil();
    void testStateUntilFail();
    void testStateTransitions();
    void testStateGlobalTransitions();

};

#endif
