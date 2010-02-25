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


#include <iostream>
#include <sstream>
#include <string>

#include <scripting/Parser.hpp>
#include <scripting/FunctionGraph.hpp>
#include <scripting/ScriptingAccess.hpp>
#include <extras/SimulationThread.hpp>
#include <extras/SimulationActivity.hpp>
#include <interface/ServiceProvider.hpp>

#include <TaskContext.hpp>
#include <Method.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace RTT;
using namespace RTT::detail;
using namespace std;



class FunctionTest
{
public:
    Parser parser;
    TaskContext gtc;
    ScriptingAccess* sa;
    ServiceProvider::shared_ptr createObject( ServiceProvider::shared_ptr );
    Attribute<int> var_i;
    Constant<int>* const_i;

    bool assertBool( bool b) { return b; }
    bool assertMsg( bool b, const std::string& msg) {
        if ( b == false )
            cout << "Asserted :" << msg << endl;
        return true; // allow to continue to check other commands.
    }
    int increase() { return ++i;}
    void resetI() { i = 0; }

    void doFunction( const std::string& prog, TaskContext*, bool test=true );
    void finishFunction( TaskContext* , std::string );
    void loopProgram( ProgramInterfacePtr );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const { return i; }

    int i;

    FunctionTest()
    : gtc("root"),
      sa( new ScriptingAccess(&gtc) )
    {
        gtc.setActivity(new SimulationActivity(0.01));
        Attribute<int> init_var("tvar_i");
        var_i = &init_var;
        const_i = new Constant<int>("tconst_i",-1);
        // ltc has a test object
        gtc.provides()->addService( this->createObject( ServiceProvider::Create("test") ) );

        // also this functions
        this->createObject( gtc.provides() );

        gtc.provides()->addAttribute( var_i );
        gtc.provides()->addConstant( *const_i );
        var_i.set(-1);
        i = 0;
        SimulationThread::Instance()->stop();
    }

    ~FunctionTest()
    {
        delete const_i;
    }

};

ServiceProvider::shared_ptr FunctionTest::createObject(ServiceProvider::shared_ptr dat)
{
    // Add the data of the EE:
    dat->addOperation("assert", &FunctionTest::assertBool, this).doc("Assert").arg("bool", "");
    dat->addOperation("increase", &FunctionTest::increase, this).doc("Return increasing i");
    dat->addOperation("resetI", &FunctionTest::resetI, this).doc("ResetI i");
    dat->addOperation("assertMsg", &FunctionTest::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
    dat->addOperation("isTrue", &FunctionTest::assertBool, this).doc("Identity function").arg("bool", "");
    dat->addOperation("i", &FunctionTest::getI, this).doc("Return the current number");
    dat->addOperation("instantDone", &FunctionTest::true_genCom, this).doc("returns immediately");
    dat->addOperation("instantDoneDone", &FunctionTest::true_gen, this).doc("Returns true when instantDone is done.");
    dat->addOperation("neverDone", &FunctionTest::true_genCom, this).doc("returns never");
    dat->addOperation("neverDoneDone", &FunctionTest::false_gen, this).doc("Returns true when neverDone is done.");
    dat->addOperation("instantFail", &FunctionTest::false_genCom, this).doc("fails immediately");
    dat->addOperation("instantFailDone", &FunctionTest::true_gen, this).doc("Returns true when instantFail is done.");
    dat->addOperation("totalFail", &FunctionTest::false_genCom, this).doc("fails in command and condition");
    dat->addOperation("totalFailDone", &FunctionTest::false_gen, this).doc("Returns true when totalFail is done.");
    return ServiceProvider::shared_ptr(dat);
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
    BOOST_CHECK( gtc.getOperation("foo") );
    this->finishFunction( &gtc, "x");
}

// Test removing exported function in infinite loop.
BOOST_AUTO_TEST_CASE( testRemoveFunction)
{
    string prog = string("export function foo { \n")
        + " while (true) { do nothing }\n"
        + "}\n"
        + "program x { \n"
        + "   do this.foo()\n"
        + "}";

    this->doFunction( prog, &gtc, false );
    BOOST_CHECK( gtc.getOperation("foo") );
    // removing the program should lead to removal of the function from the PP.
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
    BOOST_REQUIRE( tc->engine() );
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

    BOOST_REQUIRE( sa->loadProgram( *pg_list.begin() ) );
    BOOST_CHECK( sa->getProgram( (*pg_list.begin())->getName() )->start() );

    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << sa->getProgram("x")->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( sa->getProgramStatus("x") != ProgramInterface::Status::error , "Runtime error encountered" + errormsg.str());
        BOOST_CHECK_MESSAGE( sa->getProgramStatus("x") == ProgramInterface::Status::stopped, "Program stalled" + errormsg.str() );
    }
}

void FunctionTest::finishFunction(TaskContext* tc, std::string prog_name)
{
    BOOST_REQUIRE( sa->getProgram(prog_name) );
    sa->getProgram( prog_name )->stop();
    sa->unloadProgram( prog_name );
}


