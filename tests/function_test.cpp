/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  function_test.cpp 

                        function_test.cpp -  description
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
 
 

#include "function_test.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <execution/FunctionGraph.hpp>
#include <corelib/SimulationThread.hpp>
#include <execution/TemplateFactories.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FunctionTest );


    FunctionTest::FunctionTest()
        : gtc("root" ),
          gtask( 0.01, gtc.getExecutionEngine() )
    {}


void 
FunctionTest::setUp()
{
    // ltc has a test object
    gtc.methodFactory.registerObject("test", this->createMethodFactory() );
    gtc.commandFactory.registerObject("test", this->createCommandFactory() );
    gtc.dataFactory.registerObject("test", this->createDataSourceFactory() );

    i = 0;
}


void 
FunctionTest::tearDown()
{
    gtc.methodFactory.unregisterObject( "test" );
    gtc.commandFactory.unregisterObject( "test" );
    gtc.dataFactory.unregisterObject( "test" );
}


bool FunctionTest::assertBool( bool b) { 
    return b;
}
bool FunctionTest::assertMsg( bool b, const std::string& msg) {
    if ( b == false )
        cout << "Asserted :" << b << " with :" << msg << endl;
    return b;
}

int FunctionTest::increase() {
    return ++i;
}

void FunctionTest::reset() {
    i = 0;
}


MethodFactoryInterface* FunctionTest::createMethodFactory()
{
    // Add the data of the EE:
    TemplateMethodFactory< FunctionTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &FunctionTest::assertBool,
                              "Assert", "bool", "") );
    dat->add( "increase", method( &FunctionTest::increase,
                                "Return increasing i" ) );
    dat->add( "reset", method( &FunctionTest::reset,
                              "Reset i") );
    dat->add( "assertMsg", method( &FunctionTest::assertMsg,
                                 "Assert message", "bool", "", "text", "text" ) );
    return dat;
}

DataSourceFactoryInterface* FunctionTest::createDataSourceFactory()
{
    // Add the data of the EE:
    TemplateDataSourceFactory< FunctionTest >* dat =
        newDataSourceFactory( this );

    dat->add( "isTrue", data( &FunctionTest::assertBool,
                              "Identity function", "bool", "") );
    dat->add( "i", data( &FunctionTest::i,
                         "Return the current number" ) );
    return dat;
}

CommandFactoryInterface* FunctionTest::createCommandFactory()
{
    // Add the data of the EE:
    TemplateCommandFactory< FunctionTest >* dat =
        newCommandFactory( this );

    dat->add( "instantDone", command( &FunctionTest::true_genCom,
                                      &FunctionTest::true_gen,
                                      "returns immediately") );
    dat->add( "neverDone", command( &FunctionTest::true_genCom,
                                    &FunctionTest::false_gen,
                                    "returns never") );
    dat->add( "instantNotDone", command( &FunctionTest::true_genCom,
                                         &FunctionTest::true_gen,
                                         "returns never", false ) );
    dat->add( "instantFail", command( &FunctionTest::false_genCom,
                                      &FunctionTest::true_gen,
                                      "fails immediately") );
    dat->add( "totalFail", command( &FunctionTest::false_genCom,
                                    &FunctionTest::false_gen,
                                    "fails in command and condition") );
    return dat;
}

void FunctionTest::testSimpleFunction()
{
    string prog = string("function foo { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "program x { \n"
        //        + "   do test.assert( true )\n"
        + "   call foo\n"
        //+ "   do test.assert( true )\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::testExportFunction()
{
    string prog = string("export function foo { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "program x { \n"
        + "   do this.foo()\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::testRecFunction()
{
    string prog = string("export function foo { \n")
        + " do this.foo()\n" // recursive is forbidden.
        + "}\n"
        + "program x { \n"
        + "   do foo\n"
        + "}";

    try {
        parser.parseProgram( prog, &gtc );
    }
    catch( ... )
        {
            prog = string("function foo { \n")
                + " call foo\n" // recursive is forbidden.
                + "}\n"
                + "program x { \n"
                + "   call foo\n"
                + "}";
            //progs = prog;
            try {
                parser.parseProgram( prog, &gtc );
            }
            catch( ... )
                {
                    return;
                }
            CPPUNIT_ASSERT_MESSAGE( "Recursive 'call' function was accepted, while it is illegal.", false );
        }
    CPPUNIT_ASSERT_MESSAGE( "Recursive 'do' function was accepted, while it is illegal.", false );
}

void FunctionTest::testCallFunction()
{
    string prog = string("function foo(int a, string b, bool c) { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + " if true then\n"
        + "    return\n"
        + " do test.assert(false)\n"  // do not reach
        + "}\n"
        + "program x { \n"
        + "   call foo( 1, \"hello\", true)\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::testFunctionStack()
{
    string prog = string("export function foo { \n")
        +"  var double a = 1.234\n"
        +"  var double b = 4.321\n"
        +"  do test.assert( a == 1.234 )\n"
        +"  do test.assert( b == 4.321 )\n"
        +"  set a = 2.134\n"
        +"  set b = 3.421\n"
        +"  do test.assert( a == 2.134 )\n"
        +"  do test.assert( b == 3.421 )\n"
        + "}\n"
        + "program x { \n"
        +"  var double b = 1.234\n" // we switch val's of a and b here
        +"  var double a = 4.321\n"
        + "   do foo()\n"
        +"  do test.assert( b == 1.234 )\n"
        +"  do test.assert( a == 4.321 )\n"
        + "   do foo()\n"
        + " set a = 0.0\n"
        + " set b = 1.0\n"
        +"  do test.assert( b == 1.0 )\n"
        +"  do test.assert( a == 0.0 )\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}


void FunctionTest::testFunctionExportArgs()
{
    // Test if the foo args are init'ed correctly.
    string prog =
        string("export function fooA(int a, string b, bool c) { \n")
        + " do test.assert( c )\n"
        + " do test.assert( a == 1 )\n"
        + " do test.assert( b == \"A\" )\n"
        + "}\n"
        + "export function fooB(int a, string b, bool c) { \n"
        + " do test.assert( !c )\n"
        + " do test.assert( a == -1 )\n"
        + " do test.assert( b == \"B\" )\n"
        + " do fooA(1, \"A\", true)\n"
        + "}\n"
        + "program x { \n"
        + "   do fooA(1, \"A\", true)\n"
        + "   do fooB(-1, \"B\", false)\n"
//         + "   call fooA(1.0, \"A\", true)\n"
//         + "   call fooB(-1, \"B\", false)\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::testFunctionCallArgs()
{
    // Test if the foo args are init'ed correctly.
    string prog =
        string("function fooA(int a, string b, bool c) { \n")
        + " do test.assert( c )\n"
        + " do test.assert( a == 1 )\n"
        + " do test.assert( b == \"A\" )\n"
        + "}\n"
        + "function fooB(int a, string b, bool c) { \n"
        + " do test.assert( !c )\n"
        + " var int i = 1\n"
        + " var string s = \"A\"\n"
        + " var bool tf = true\n"
        + " call fooA(i, s, tf)\n"
        + " do test.assert( a == -1 )\n"
        + " do test.assert( b == \"B\" )\n"
        + "}\n"
        + "program x { \n"
        + "   call fooA(1, \"A\", true)\n"
        + "   call fooB(-1, \"B\", false)\n"
//         + "   call fooA(1.0, \"A\", true)\n"
//         + "   call fooB(-1, \"B\", false)\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::testFunctionFail()
{
    // Test if obj-function error is propagated correctly to
    // calling program or function
    string prog =
        string("export function fooA { \n")
        + " do test.assert( false )\n" // throws error
        + "}\n"
        + "export function fooB { \n"
        + " do fooA()\n"
        + "}\n"
        + "program x { \n"
        + "   var bool success = false\n"
        + "   try fooA()\n"
        + "   catch \n"
        + "      set success = true\n" // error caught.
        + "   do test.assert(success)\n"
        + "   set success = false\n"
        + "   try fooB()\n"
        + "   catch\n"
        + "      set success = true\n" // error caught.
        + "   do test.assert(success)\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

void FunctionTest::doFunction( const std::string& prog, TaskContext* tc, bool test )
{
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE( exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT_MESSAGE("No program parsed in test.", false );
        }
    ProgramProcessor* pp = tc->getExecutionEngine()->getProgramProcessor();
    pp->loadProgram( *pg_list.begin() );
    pp->getProgram( (*pg_list.begin())->getName() )->start();
    SimulationThread::Instance()->start();
    gtask.start();
//     while (1)
    sleep(1);
    gtask.stop();
    SimulationThread::Instance()->stop();

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << pp->getProgram("x")->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( "Runtime error encountered" + errormsg.str(), pp->getProgramStatus("x") != ProgramInterface::Status::error );
        CPPUNIT_ASSERT_MESSAGE( "Program stalled " + errormsg.str(), pp->getProgramStatus("x") == ProgramInterface::Status::stopped );
    }
}

void FunctionTest::finishFunction(TaskContext* tc, std::string prog_name)
{
    tc->getExecutionEngine()->getProgramProcessor()->getProgram( prog_name )->stop();
    tc->getExecutionEngine()->getProgramProcessor()->unloadProgram( prog_name );

    TaskContext* ptc= tc->getPeer("programs")->getPeer(prog_name);
    tc->getPeer("programs")->removePeer(prog_name);
    delete ptc;

    ptc= tc->getPeer("programs");
    tc->removePeer("programs");
    delete ptc;
}

    

