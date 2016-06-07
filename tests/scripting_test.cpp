/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  scripting_test.cpp

                        scripting_test.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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
#include <scripting/Scripting.hpp>
#include <scripting/ScriptingService.hpp>
#include <extras/SequentialActivity.hpp>
#include <plugin/PluginLoader.hpp>
#include <scripting/Parser.hpp>
#include <internal/GlobalService.hpp>


using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ScriptingTestSuite,  OperationsFixture )

//! Tests the scripting service's functions
BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //ScriptingService* sa = new ScriptingService( tc ); // done by TC or plugin.

    PluginLoader::Instance()->loadService("scripting",tc);

    // We use a sequential activity in order to force execution on trigger().
//    tc->stop();
//    BOOST_CHECK( tc->setActivity( new SequentialActivity() ) );
//    tc->start();

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );
    bool r;
    r = sc->loadProgramText("program Foo { do test.assert(true); set ret = 10.0; }");
    BOOST_REQUIRE( r );
    BOOST_CHECK( sc->hasProgram("Foo") );
    BOOST_REQUIRE( sc->startProgram("Foo") );
    BOOST_CHECK( sc->isProgramRunning("Foo") );

    // executes our script in the EE:
    while(sc->isProgramRunning("Foo")) {
        tc->trigger();
        usleep(100);
    }

    // test results:
    BOOST_CHECK( sc->isProgramRunning("Foo") == false );
    BOOST_CHECK( sc->inProgramError("Foo") == false );
    BOOST_CHECK( ret == 10.0 );

}

BOOST_AUTO_TEST_CASE(TestScriptingParser)
{
    PluginLoader::Instance()->loadService("scripting",tc);

    // We use a sequential activity in order to force execution on trigger().
//    tc->stop();
//    BOOST_CHECK( tc->setActivity( new SequentialActivity() ) );
//    tc->start();

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );
    bool r;

    // test plain statements:
    string statements=";;test.increase()\n;;;\ntest.increase()"; // trailing newline is optional
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 1);

    // test variable decls:
    statements="var int i = 0; var int y,z=10; test.i = z";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 10);

    // test if statement:
    statements="var int x=1,y=2; if  3 == 8 then test.i = x else test.i = y";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 2);

    // test while statement:
    statements="var int x=1,y=2; while x != y  { test.i = 3; x = y; }";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 3);

    // test while name clash:
    statements="var int whilex, whiley\n whilex = 1; whiley=2\n while whilex != whiley  { test.i = 3; whilex = whiley; }";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 3);

    // test for statement:
    statements="var int x=10,y=20; for(  x = 0; x != y; x = x + 1) { test.i = x; }";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 19);

    // test for name clash:
    statements="var int forx,fory\n forx=10; fory=20; for(  forx = 0; forx != fory; forx = forx + 1) { test.i = forx; }";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 19);

    // test function +  a statement that uses that function:
    statements = "export function adder(int a, int b) { test.i = a + b; }\n adder(5,6)\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 11);
    statements = "export void adder2(int a, int b) { test.i = a + b; }\n adder2(7,8)\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 15);
    statements = "export int adder3(int a, int b) { return a + b; }\n test.i = adder3(6,10)\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 16);

    // test program +  a statement that starts that program and waits for the result.
    statements = "program rt_script { test.i = 3-9; }\n rt_script.start();;;; while( rt_script.isRunning() ) { trigger(); yield; }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( sc->getProgramStatus("rt_script"), ProgramInterface::Status::stopped );
    BOOST_CHECK_EQUAL( i, -6);

    // test state machine +  a statement that starts that SM and waits for the result.
    statements = "StateMachine RTState { initial state init { entry { test.i = 0;} transitions { select fini; } } final state fini { entry { test.i = test.i - 2; } } }; RootMachine RTState rt_state; rt_state.activate(); rt_state.start(); while( !rt_state.inFinalState() ) { trigger(); yield; }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( sc->getStateMachineState("rt_state"), "fini" );
    BOOST_CHECK_EQUAL( i, -2);
}

BOOST_AUTO_TEST_CASE(TestScriptingFunction)
{
    PluginLoader::Instance()->loadService("scripting",tc);

    // We use a sequential activity in order to force execution on trigger().
//    tc->stop();
//    BOOST_CHECK( tc->setActivity( new SequentialActivity() ) );
//    tc->start();

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );
    bool r;

    // set test counter to zero:
    i = 0;

    // define a function (added to scripting interface):
    string statements="void func1(void) { test.increase(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func1"));

    // export a function:
    statements="export void efunc1(void) { test.increase(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides()->hasMember("efunc1"));

    // local function:
    statements="void lfunc1(void) { test.increase(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("lfunc1"));

    // global function:
    statements="global void gfunc1(void) { test.increase(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( GlobalService::Instance()->provides()->hasMember("gfunc1"));

    // nested function:
    statements="void func2(void) { func1(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func2"));

    // nested exported function:
    statements="void efunc2(void) { efunc1(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("efunc2"));

    // nested global function:
    statements="void gfunc2(void) { gfunc1(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("gfunc2"));

    // nested local function:
    statements="void lfunc2(void) { lfunc1(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("lfunc2"));

    // invoke a function:
    statements="func1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 1);

    // invoke an exported function:
    statements="efunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 2);

    // invoke a global function:
    statements="gfunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 3);

    // invoke a local function:
    statements="lfunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 4);

    // invoke a nested function:
    statements="func2()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 5);

    // invoke a nested exported function:
    statements="efunc2()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 6);

    // invoke a nested global function:
    statements="gfunc2()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 7);

    // invoke a nested local function:
    statements="lfunc2()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 8);

    // call a function:
    statements="call func1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( !r );
    BOOST_CHECK_EQUAL( i, 8);

    // call an exported function:
    statements="call efunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( !r );
    BOOST_CHECK_EQUAL( i, 8);

    // RE-define a function (added to scripting interface):
    statements="void func1(void) { test.increase(); test.increase(); }\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 8);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func1"));

    statements="func1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 10);

    // RE-export a function:
    statements="export void efunc1(void) { test.increase(); test.increase();}\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 10);
    BOOST_CHECK( tc->provides()->hasMember("efunc1"));

    statements="efunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 12);

    // RE-global a function:
    statements="global void gfunc1(void) { test.increase(); test.increase();}\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 12);
    BOOST_CHECK( GlobalService::Instance()->provides()->hasMember("gfunc1"));

    statements="gfunc1()\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 14);
}

BOOST_AUTO_TEST_SUITE_END()
