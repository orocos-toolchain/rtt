/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  program_test.hpp 

                        program_test.hpp -  description
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
 
 
#ifndef PROGRAMTEST_H
#define PROGRAMTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <Parser.hpp>
#include <TaskContext.hpp>
#include <string>
#include <SimulationActivity.hpp>

using namespace RTT;

class ProgramTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ProgramTest );
    CPPUNIT_TEST( testEmptyProgram );
    CPPUNIT_TEST( testReturnProgram );
    CPPUNIT_TEST( testParseProgram );
    CPPUNIT_TEST( testProgramFailure );
    CPPUNIT_TEST( testProgramCondition );
    CPPUNIT_TEST( testProgramAnd );
    CPPUNIT_TEST( testProgramTry );
    CPPUNIT_TEST( testProgramUntil );
    CPPUNIT_TEST( testProgramBreak );
    CPPUNIT_TEST( testProgramUntilFail );
    CPPUNIT_TEST( testProgramTask);
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext gtc;
    SimulationActivity gtask;
    OperationInterface* createObject( OperationInterface*, CommandProcessor* cp );
    Attribute<int> var_i;
    Constant<int>* const_i;
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    int increase();
    void reset();
    void doProgram( const std::string& prog, TaskContext*, bool test=true );
    void finishProgram( TaskContext* , std::string );
    void loopProgram( ProgramInterfacePtr );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const { return i; }
    
    int i;
public:
    ProgramTest();

    void setUp();
    void tearDown();

    void testEmptyProgram();
    void testReturnProgram();
    void testParseProgram();
    void testProgramTask();
    void testProgramCondition();
    void testProgramFailure();
    void testProgramAnd();
    void testProgramTry();
    void testProgramBreak();
    void testProgramUntil();
    void testProgramUntilFail();

};

#endif
