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

#include <iostream>
#include <sstream>
#include <FunctionGraph.hpp>
#include <SimulationThread.hpp>
#include <Method.hpp>
#include <Command.hpp>
#include <TaskObject.hpp>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

    FunctionTest::FunctionTest()
        : gtc("root" ),
          gtask( 0.01, gtc.engine() )
    {
    	setUp();
    }


void
FunctionTest::setUp()
{
    SimulationThread::Instance()->stop();
    // ltc has a test object
    gtc.addObject(this->createObject("test", gtc.engine()->commands() ) );
    gtask.start();
    i = 0;
}


void
FunctionTest::tearDown()
{
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


OperationInterface* FunctionTest::createObject(string a, CommandProcessor* cp)
{
    TaskObject* dat = new TaskObject(a);
    dat->methods()->addMethod( method( "assert", &FunctionTest::assertBool, this),
                              "Assert", "bool", "" );
    dat->methods()->addMethod( method( "increase", &FunctionTest::increase, this),
                                "Return increasing i"  );
    dat->methods()->addMethod( method( "reset", &FunctionTest::reset, this),
                              "Reset i" );
    dat->methods()->addMethod( method( "assertMsg", &FunctionTest::assertMsg, this),
                                 "Assert message", "bool", "", "text", "text"  );

    dat->methods()->addMethod( method( "isTrue", &FunctionTest::assertBool, this),
                              "Identity function", "bool", "" );
    dat->methods()->addMethod( method( "i", &FunctionTest::getI, this),
                         "Return the current number"  );

    dat->commands()->addCommand( command( "instantDone", &FunctionTest::true_genCom,
                                      &FunctionTest::true_gen, this, cp),
                                      "returns immediately" );
    dat->commands()->addCommand( command( "neverDone", &FunctionTest::true_genCom,
                                    &FunctionTest::false_gen, this, cp),
                                    "returns never" );
    dat->commands()->addCommand( command( "instantNotDone", &FunctionTest::true_genCom,
                                         &FunctionTest::true_gen, this, cp, false),
                                         "returns never");
    dat->commands()->addCommand( command( "instantFail", &FunctionTest::false_genCom,
                                      &FunctionTest::true_gen, this, cp),
                                      "fails immediately" );
    dat->commands()->addCommand( command( "totalFail", &FunctionTest::false_genCom,
                                    &FunctionTest::false_gen, this, cp),
                                    "fails in command and condition" );
    return dat;
}

BOOST_FIXTURE_TEST_SUITE( FunctionTestSuite, FunctionTest )
// Registers the fixture into the 'registry'

BOOST_AUTO_TEST_CASE( testSimpleFunction)
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

BOOST_AUTO_TEST_CASE( testExportFunction)
{
    string prog = string("export function foo { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "export function foo_args() { \n"
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "program x { \n"
        + "   do this.foo()\n"
        + "   do this.foo_args()\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testRecFunction)
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
            BOOST_CHECK_MESSAGE( false, "Recursive 'call' function was accepted, while it is illegal." );
        }
    BOOST_CHECK_MESSAGE( false , "Recursive 'do' function was accepted, while it is illegal.");
}

BOOST_AUTO_TEST_CASE( testCallFunction)
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

BOOST_AUTO_TEST_CASE( testFunctionStack)
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


BOOST_AUTO_TEST_CASE( testFunctionExportArgs)
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

BOOST_AUTO_TEST_CASE( testFunctionCallArgs)
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

BOOST_AUTO_TEST_CASE( testFunctionFail)
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
        + "   do test.assertMsg(success,\"Program script did not detect exported function failure.\")\n"
        + "   set success = false\n"
        + "   try fooB()\n"
        + "   catch\n"
        + "      set success = true\n" // error caught.
        + "   do test.assertMsg(success,\"Program script did not detect exported function failure.\")\n"
        + "}";

    this->doFunction( prog, &gtc );
    this->finishFunction( &gtc, "x");
}

BOOST_AUTO_TEST_SUITE_END()

void FunctionTest::doFunction( const std::string& prog, TaskContext* tc, bool test )
{
    BOOST_CHECK( tc->engine() );
    BOOST_CHECK( tc->engine()->programs());
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( false , exc.what() );
        }
    if ( pg_list.empty() )
        {
            BOOST_REQUIRE_MESSAGE(false , "No program parsed in test.");
        }
    ProgramProcessor* pp = tc->engine()->programs();
    BOOST_CHECK( pp->loadProgram( *pg_list.begin() ) );
    BOOST_CHECK( pp->getProgram( (*pg_list.begin())->getName() )->start() );

    BOOST_CHECK (SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << pp->getProgram("x")->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( pp->getProgramStatus("x") != ProgramInterface::Status::error , "Runtime error encountered" + errormsg.str());
        BOOST_CHECK_MESSAGE( pp->getProgramStatus("x") == ProgramInterface::Status::stopped, "Program stalled" + errormsg.str() );
    }
    tc->stop();
}

void FunctionTest::finishFunction(TaskContext* tc, std::string prog_name)
{
    tc->engine()->programs()->getProgram( prog_name )->stop();
    tc->engine()->programs()->unloadProgram( prog_name );
}


