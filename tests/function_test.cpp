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


#include "unit.hpp"
#include "operations_fixture.hpp"


#include <scripting/Parser.hpp>
#include <scripting/FunctionGraph.hpp>
#include <scripting/ScriptingService.hpp>
#include <Service.hpp>
#include <OperationCaller.hpp>

class FunctionsFixture : public OperationsFixture
{
public:
    FunctionsFixture()
    : sa( new ScriptingService(tc) )
    {
        tc->stop();
        tc->setActivity(new SimulationActivity(0.01));
        SimulationThread::Instance()->stop();
        tc->start();
    }
    Parser parser;
    ScriptingService* sa;

    void doFunction( const std::string& prog, TaskContext*, bool test=true );
    void finishFunction( TaskContext* , std::string );
    void loopProgram( ProgramInterfacePtr );
};

BOOST_FIXTURE_TEST_SUITE( FunctionsFixtureSuite, FunctionsFixture )
// Registers the fixture into the 'registry'

BOOST_AUTO_TEST_CASE( testSimpleFunction)
{
    string prog = string("function foo { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "program x { \n"
        + "   call foo\n"
        + "}";

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
}

BOOST_AUTO_TEST_CASE( testSimpleReturnFunction)
{
    string prog = string("int foo { \n")
        + " test.assert( test.isTrue( true ) )\n"
        + " return 3\n"
        + "}\n"
        + "\n"
        + "program x { \n"
        + "   test.assert(true);\n"
        + "   call foo\n"
        + "   test.assert(true);\n"
        + "}";

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
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

    this->doFunction( prog, tc );
    BOOST_CHECK( tc->getOperation("foo") );
    this->finishFunction( tc, "x");
}

#ifdef ORO_REMOTING
/**
 * Exports foo and foo_args and calls them from C++
 * Compare with testReturnExportFunction below that does
 * the same, but in scripting.
 * This code relies on RemoteOperationCaller and OperationCallerC
 */
BOOST_AUTO_TEST_CASE( testOnlyExportFunction)
{
    string prog = string("export function foo { \n")
//                + "   test.print(\"foo()\")\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "export int foo_ret(double d) { \n"
//        + "   test.printNumber(\"foo_ret(double d), d is: \", d)\n"
        + "   if (true) then\n"
        + "     return 3\n"
        + "   else\n"
        + "     return 5\n"
        + "}\n"
        + "export int foo_args(double d, int v) { \n"
//        + " test.printNumber(\"foo_args(double d, int v) v is: \", v)\n"
        + " var double r = 10\n"
        + " if ( d == 3.0 && v == 6) then\n"
        + "     set r = +1\n"
        + " else\n"
        + "     set r = -1\n"
        + " return r;\n"
        + "}\n";

    this->doFunction( prog, tc );
    BOOST_CHECK( tc->getOperation("foo") );

    // Test call:
    {
    	OperationCaller<void(void)> foo( tc->getOperation("foo"), caller->engine());
    	BOOST_CHECK( foo.ready() );

    	foo();
    }
#if 0
    // Test pure send:
    {
    	OperationCaller<void(void)> foo( tc->getOperation("foo"), caller->engine());
    	BOOST_CHECK( foo.ready() );

    	foo.send();
    }

    // Test send + collect:
    {
    	OperationCaller<void(void)> foo( tc->getOperation("foo"), caller->engine());
    	BOOST_CHECK( foo.ready() );

    	SendHandle<void(void)> sh = foo.send();
    	BOOST_CHECK_EQUAL( sh.collect(), SendSuccess);
    }
#endif
    // Test call:
    {
    	OperationCaller<int(double)> foo_ret( tc->getOperation("foo_ret"), caller->engine());
    	BOOST_CHECK( foo_ret.ready() );

    	int i = 0;
    	i = foo_ret(3.0);
    	BOOST_CHECK_EQUAL( i, 3 );
    }
#if 0
    // Test pure send:
    {
    	OperationCaller<int(double)> foo_ret( tc->getOperation("foo_ret"), caller->engine());
    	BOOST_CHECK( foo_ret.ready() );

    	int i = 0;
    	foo_ret.send(3.0);
    }

    // Test send + collect:
    {
    	OperationCaller<int(double)> foo_ret( tc->getOperation("foo_ret"), caller->engine());
    	BOOST_CHECK( foo_ret.ready() );

    	int i = 0;
    	SendHandle<int(double)> sh = foo_ret.send(3.0);
    	BOOST_CHECK_EQUAL( sh.collect(i), SendSuccess);
    	BOOST_CHECK_EQUAL( i, 3 );
    }
#endif
    // Test call:
    {
    	BOOST_CHECK( tc->getOperation("foo_args") );
    	OperationCaller<int(double,int)> foo_args( tc->getOperation("foo_args"), caller->engine());
    	BOOST_CHECK( foo_args.ready() );

    	int i = 0;
    	i = foo_args(-3.0, -6);
    	BOOST_CHECK_EQUAL( i, -1);

    	i = 0;
    	i = foo_args(3.0, 6);
    	BOOST_CHECK_EQUAL( i, +1);
    }
#if 0
    // Test pure send:
    {
    	BOOST_CHECK( tc->getOperation("foo_args") );
    	OperationCaller<int(double,int)> foo_args( tc->getOperation("foo_args"), caller->engine());
    	BOOST_CHECK( foo_args.ready() );

    	foo_args.send(-3.0, -6);
    	foo_args.send(3.0, 6);
    }

    // Test send + collect:
    {
    	BOOST_CHECK( tc->getOperation("foo_args") );
    	OperationCaller<int(double,int)> foo_args( tc->getOperation("foo_args"), caller->engine());
    	BOOST_CHECK( foo_args.ready() );

    	int i = 0;
    	SendHandle<int(double,int)> sh = foo_args.send(-3.0, -6);
    	BOOST_CHECK_EQUAL( sh.collect(i), SendSuccess);
    	BOOST_CHECK_EQUAL( i, -1);

    	i = 0;
    	sh = foo_args.send(3.0, 6);
    	BOOST_CHECK_EQUAL( sh.collect(i), SendSuccess);
    	BOOST_CHECK_EQUAL( i, +1);
    }
#endif
}
#endif
/**
 * Compare to use case above that does the same,
 * but in C++
 */
BOOST_AUTO_TEST_CASE( testReturnExportFunction)
{
    string prog = string("export function foo { \n")
        + " do test.assert( test.isTrue( true ) )\n"
        + "}\n"
        + "export int foo_ret() { \n"
        + "   if (true) then\n"
        + "     return 3\n"
        + "   else\n"
        + "     return 5\n"
        + "}\n"
        + "export int foo_args(double d, int v) { \n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " if ( d == 3.0 && v == 6) then\n"
        + "     return +1\n" // 10
        + " else\n"
        + "     return -1\n"
        + " return 4\n"
        + "}\n"
        + "program x { \n"
        + "   this.foo()\n" // a void function
        + "   this.foo_ret()\n"
        + "   this.foo_args(3.0,6)\n"
        + "   test.assertEqual(this.foo_ret(), 3 )\n"
        + "   test.assertEqual(this.foo_args(3.0,6), 1)\n" // 20
        + "   test.assertEqual(this.foo_args(0.0,0), -1)\n"
        + "}";

    this->doFunction( prog, tc );
    BOOST_CHECK( tc->getOperation("foo") );
    this->finishFunction( tc, "x");
}

#if 0
// Test removing exported function in infinite loop.
BOOST_AUTO_TEST_CASE( testRemoveFunction)
{
    string prog = string("export function foo { \n")
        + " while (true) { do nothing }\n" // this one yiels politely
        + "}\n"
        + "program x { \n"
        + "   this.foo()\n" // this will hang the program's execution being blocked in waitForMessages for foo() to return... should we use a yield point ? Was so before because it were commands...
        //+ "   this.foo.send()\n" // send/collect not yet supported.
        + "}";

    this->doFunction( prog, tc, false );
    BOOST_CHECK( tc->getOperation("foo") );
    // removing the program should lead to removal of the function from the PP.
    this->finishFunction( tc, "x");
}
#endif
BOOST_AUTO_TEST_CASE( testRecFunction)
{
    string prog = string("export function foo { \n")
        + " do this.foo()\n" // recursive is forbidden.
        + "}\n"
        + "program x { \n"
        + "   do foo\n"
        + "}";

    try {
        parser.parseProgram( prog, tc );
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
                parser.parseProgram( prog, tc );
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

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
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

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
}


BOOST_AUTO_TEST_CASE( testFunctionExportArgs)
{
    // Test if the foo args are init'ed correctly.
    string prog =
        string("export function fooA(int a, string b, bool c) { \n")
        + " do test.assertMsg( c, \"c not true\" )\n"
        + " do test.assertMsg( a == 1, \"a not 1\" )\n"
        + " do test.assertMsg( b == \"A\", \"b not A\" )\n"
        + "}\n"
        + "export function fooB(int a, string b, bool c) { \n"
        + " do test.assertMsg( !c, \"c not false\" )\n"
        + " do test.assertMsg( a == -1, \"a not -1\" )\n"
        + " do test.assertMsg( b == \"B\", \"b not B\"   )\n"
        + " do fooA(1, \"A\", true)\n"
        + "}\n"
        + "program x { \n"
        + "   do fooA(1, \"A\", true)\n"
        + "   do fooB(-1, \"B\", false)\n"
//         + "   call fooA(1.0, \"A\", true)\n"
//         + "   call fooB(-1, \"B\", false)\n"
        + "}";

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
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

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
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

    this->doFunction( prog, tc );
    this->finishFunction( tc, "x");
}

BOOST_AUTO_TEST_SUITE_END()

void FunctionsFixture::doFunction( const std::string& prog, TaskContext* tc, bool test )
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
    catch( ... ) {
        BOOST_REQUIRE_MESSAGE( false, "Unknown exception thrown by Parser.");
    }
    if ( pg_list.empty() )
    {
        // no program necessary here.
        //BOOST_REQUIRE_MESSAGE(false , "No program parsed in test.");
        return;
    }

    BOOST_REQUIRE( sa->loadProgram( *pg_list.begin() ) );
    BOOST_CHECK( sa->getProgram( (*pg_list.begin())->getName() )->start() );

    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << sa->getProgram("x")->getLineNumber() <<" of program 'x' (or of a function 'called' by x)."<<endl;
        BOOST_REQUIRE_MESSAGE( sa->getProgramStatus("x") != ProgramInterface::Status::error , "Runtime Error Status encountered" + errormsg.str());
        BOOST_REQUIRE_MESSAGE( sa->getProgramStatus("x") == ProgramInterface::Status::stopped, "Program stalled" + errormsg.str() );
    }
}

void FunctionsFixture::finishFunction(TaskContext* tc, std::string prog_name)
{
    BOOST_REQUIRE( sa->getProgram(prog_name) );
    sa->getProgram( prog_name )->stop();
    sa->unloadProgram( prog_name );
}


