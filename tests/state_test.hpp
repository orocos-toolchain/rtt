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

#include <cppunit/extensions/HelperMacros.h>

//#define NOPARSER
#ifndef NOPARSER
#include <execution/Parser.hpp>
#endif

#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>
#include <corelib/TaskSimulation.hpp>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class StateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( StateTest );
    CPPUNIT_TEST( testParseState );
    //CPPUNIT_TEST( testStateFailure );
    CPPUNIT_TEST( testStateChildren );
    CPPUNIT_TEST( testStateEmpty );
    CPPUNIT_TEST( testStateSubStateVars );
    CPPUNIT_TEST( testStateSubStateCommands );
//     CPPUNIT_TEST( testStateUntil );
//     CPPUNIT_TEST( testStateUntilFail );
    CPPUNIT_TEST_SUITE_END();

#ifndef NOPARSER
    Parser parser;
#endif
    Processor gprocessor;
    TaskContext gtc;
    TaskSimulation gtask;
    MethodFactoryInterface* createMethodFactory();
    CommandFactoryInterface* createCommandFactory();
    DataSourceFactoryInterface* createDataSourceFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    int increase();
    void reset();
    void doState( const std::string& prog, TaskContext*, bool test=true );
    void finishState( TaskContext* , std::string );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int i;
public:
    StateTest();

    void setUp();
    void tearDown();

    void testParseState();
    void testStateFailure();
    void testStateChildren();
    void testStateEmpty();
    void testStateSubStateVars();
    void testStateSubStateCommands();
    void testStateUntil();
    void testStateUntilFail();

};

#endif
