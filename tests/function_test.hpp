/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  function_test.hpp 

                        function_test.hpp -  description
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
 
 
#ifndef FUNCTIONTEST_H
#define FUNCTIONTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <Parser.hpp>
#include <ExecutionEngine.hpp>
#include <TaskContext.hpp>
#include <string>
#include <SimulationActivity.hpp>

using namespace RTT;
using namespace RTT;

class FunctionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( FunctionTest );
    CPPUNIT_TEST( testSimpleFunction );
   CPPUNIT_TEST( testRecFunction );
    CPPUNIT_TEST( testCallFunction );
    CPPUNIT_TEST( testExportFunction );
   CPPUNIT_TEST( testFunctionStack );
    CPPUNIT_TEST( testFunctionCallArgs );
    CPPUNIT_TEST( testFunctionExportArgs );
   CPPUNIT_TEST( testFunctionFail );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext gtc;
    SimulationActivity gtask;
    OperationInterface* createObject(std::string name, CommandProcessor* cp);
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    int increase();
    void reset();
    void doFunction( const std::string& prog, TaskContext*, bool test=true );
    void finishFunction( TaskContext* , std::string );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const { return i; }
    int i;
public:
    FunctionTest();

    void setUp();
    void tearDown();

    void testSimpleFunction();
    void testRecFunction();
    void testCallFunction();
    void testExportFunction();
    void testFunctionStack();
    void testFunctionCallArgs();
    void testFunctionExportArgs();
    void testFunctionFail();

};

#endif
