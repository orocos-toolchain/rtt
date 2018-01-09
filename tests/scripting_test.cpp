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

// note: Does not preserve newlines. Add them explicitly with \n or add semicolons after each line.
#define MULTILINE_STRING(...) #__VA_ARGS__

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ScriptingTestSuite,  OperationsFixture )

//! Tests the scripting service's functions
BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //ScriptingService* sa = new ScriptingService( tc ); // done by TC or plugin.

    PluginLoader::Instance()->loadService("scripting",tc);

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );
    BOOST_REQUIRE( sc->loadProgramText( MULTILINE_STRING(
        program Foo {
            do test.assert(true);
            set ret = 10.0;
        })));
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

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );

    // test plain statements:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        ;;test.increase()\n
        ;;;\n
        test.increase())));  // trailing newline is optional
    BOOST_CHECK_EQUAL( i, 1);

    // test variable decls:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int i = 0;
        var int y, z = 10;
        test.i = z;
        )));
    BOOST_CHECK_EQUAL( i, 10);

    // test if statement:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int x = 1, y = 2;
        if 3 == 8 then
            test.i = x;
        else
            test.i = y;
        )));
    BOOST_CHECK_EQUAL( i, 2);

    // test while statement:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int x = 1, y = 2;
        while x != y {
            test.i = 3;
            x = y;
        })));
    BOOST_CHECK_EQUAL( i, 3);

    // test while name clash:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int whilex, whiley\n
        whilex = 1;
        whiley = 2\n
        while whilex != whiley {
            test.i = 3;
            whilex = whiley;
        })));
    BOOST_CHECK_EQUAL( i, 3);

    // test for statement:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int x = 10, y = 20;
        for( x = 0; x != y; x = x + 1) {
            test.i = x;
        })));
    BOOST_CHECK_EQUAL( i, 19);

    // test for name clash:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        var int forx, fory\n
        forx = 10; fory = 20;
        for( forx = 0; forx != fory; forx = forx + 1) {
            test.i = forx;
        })));
    BOOST_CHECK_EQUAL( i, 19);

    // test function +  a statement that uses that function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        export function adder(int a, int b) {
            test.i = a + b;
        }\n
        adder(5,6)\n
        )));
    BOOST_CHECK_EQUAL( i, 11);
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        export void adder2(int a, int b) {
            test.i = a + b;
        }\n
        adder2(7,8)\n
        )));
    BOOST_CHECK_EQUAL( i, 15);
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        export int adder3(int a, int b) {
            return a + b;
        }\n
        test.i = adder3(6,10)\n
        )));
    BOOST_CHECK_EQUAL( i, 16);

    // test program +  a statement that starts that program and waits for the result.
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        program rt_script {
            test.i = 3-9;
        }\n
        rt_script.start();;;;
        while( rt_script.isRunning() ) {
            trigger();
            yield;
        })));
    BOOST_CHECK_EQUAL( sc->getProgramStatus("rt_script"), ProgramInterface::Status::stopped );
    BOOST_CHECK_EQUAL( i, -6);

    // test state machine +  a statement that starts that SM and waits for the result.
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        StateMachine RTState {
            initial state init {
                entry {
                    test.i = 0;
                }
                transitions {
                    select fini;
                }
            }

            final state fini {
                entry {
                   test.i = test.i - 2;
                }
            }
        }
        RootMachine RTState rt_state;
        rt_state.activate();
        rt_state.start();
        while( !rt_state.inFinalState() ) {
            trigger();
            yield; // ...has no effect here other than incrementing the step counter! See ScriptParser::seenstatement().
        })));
    BOOST_CHECK_EQUAL( sc->getStateMachineState("rt_state"), "fini" );
    BOOST_CHECK_EQUAL( i, -2);
}

BOOST_AUTO_TEST_CASE(TestScriptingFunction)
{
    PluginLoader::Instance()->loadService("scripting",tc);

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );

    // set test counter to zero:
    i = 0;

    // define a function (added to scripting interface):
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void func1(void) {
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func1"));

    // export a function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        export void efunc1(void) {
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides()->hasMember("efunc1"));

    // local function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void lfunc1(void) {
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("lfunc1"));

    // global function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        global void gfunc1(void) {
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( GlobalService::Instance()->provides()->hasMember("gfunc1"));

    // nested function call:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void func2(void) {
            func1();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func2"));

    // nested exported function call:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void efunc2(void) {
            efunc1();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("efunc2"));

    // nested global function call:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void gfunc2(void) {
            gfunc1();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("gfunc2"));

    // nested local function call:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void lfunc2(void) {
            lfunc1();
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("lfunc2"));

    // invoke a function:
    BOOST_REQUIRE( sc->eval("func1()") );
    BOOST_CHECK_EQUAL( i, 1);

    // invoke an exported function:
    BOOST_REQUIRE( sc->eval("efunc1()") );
    BOOST_CHECK_EQUAL( i, 2);

    // invoke a global function:
    BOOST_REQUIRE( sc->eval("gfunc1()") );
    BOOST_CHECK_EQUAL( i, 3);

    // invoke a local function:
    BOOST_REQUIRE( sc->eval("lfunc1()") );
    BOOST_CHECK_EQUAL( i, 4);

    // invoke a function with a nested function call:
    BOOST_REQUIRE( sc->eval("func2()") );
    BOOST_CHECK_EQUAL( i, 5);

    // invoke a function with a nested exported function call:
    BOOST_REQUIRE( sc->eval("efunc2()") );
    BOOST_CHECK_EQUAL( i, 6);

    // invoke a function with a nested global function call:
    BOOST_REQUIRE( sc->eval("gfunc2()") );
    BOOST_CHECK_EQUAL( i, 7);

    // invoke a function with a nested local function call:
    BOOST_REQUIRE( sc->eval("lfunc2()") );
    BOOST_CHECK_EQUAL( i, 8);

    // call a function:
    BOOST_CHECK( !sc->eval("call func1()") );
    BOOST_CHECK_EQUAL( i, 8);

    // call an exported function:
    BOOST_CHECK( !sc->eval("call efunc1()") );
    BOOST_CHECK_EQUAL( i, 8);

    // RE-define a function (added to scripting interface):
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void func1(void) {
            test.increase();
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 8);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func1"));

    BOOST_REQUIRE( sc->eval("func1()") );
    BOOST_CHECK_EQUAL( i, 10);

    // RE-export a function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        export void efunc1(void) {
            test.increase();
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 10);
    BOOST_CHECK( tc->provides()->hasMember("efunc1"));

    BOOST_REQUIRE( sc->eval("efunc1()") );
    BOOST_CHECK_EQUAL( i, 12);

    // RE-global a function:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        global void gfunc1(void) {
            test.increase();
            test.increase();
        })));
    BOOST_CHECK_EQUAL( i, 12);
    BOOST_CHECK( GlobalService::Instance()->provides()->hasMember("gfunc1"));

    BOOST_REQUIRE( sc->eval("gfunc1()") );
    BOOST_CHECK_EQUAL( i, 14);
}

BOOST_AUTO_TEST_CASE(TestScriptingFunctionWithYield)
{
    PluginLoader::Instance()->loadService("scripting",tc);

    // We need a periodic activity for this test case so that yielded functions
    // will be executed again while we are waiting.
    tc->setPeriod(0.1);

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );

    // set test counter to zero:
    i = 0;

    // define a function that yields:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
          void func1(void) {
              test.printNumber("[ENTER func1()] CycleCounter = ", CycleCounter);
              test.increase();
              yield;
              test.increase();
              test.printNumber("[EXIT func1()] CycleCounter = ", CycleCounter);
          })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func1"));

    // define a function that calls func1, yields and calls func1 again:
    BOOST_REQUIRE( sc->eval( MULTILINE_STRING(
        void func2(void) {
            test.printNumber("[ENTER func2()] CycleCounter = ", CycleCounter);
            func1();
            yield;
            func1();
            test.printNumber("[EXIT func2()] CycleCounter = ", CycleCounter);
        })));
    BOOST_CHECK_EQUAL( i, 0);
    BOOST_CHECK( tc->provides("scripting")->hasMember("func2"));

    // invoke func1()
    BOOST_REQUIRE( sc->eval("func1()") );
    BOOST_CHECK_EQUAL( i, 2);

    // invoke func2()
    BOOST_REQUIRE( sc->eval("func2()") );
    BOOST_CHECK_EQUAL( i, 6);
}

BOOST_AUTO_TEST_SUITE_END()
