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



#include "operations_fixture.hpp"
#include <scripting/Scripting.hpp>
#include <scripting/ScriptingService.hpp>
#include <extras/SequentialActivity.hpp>
#include <plugin/PluginLoader.hpp>
#include <scripting/Parser.hpp>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace RTT::detail;

#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  ScriptingTestSuite,  OperationsFixture )

//! Tests the scripting service's functions
BOOST_AUTO_TEST_CASE(TestGetProvider)
{
    //ScriptingService* sa = new ScriptingService( tc ); // done by TC or plugin.

    PluginLoader::Instance()->loadService("scripting",tc);

    // We use a sequential activity in order to force execution on trigger().
    tc->stop();
    BOOST_CHECK( tc->setActivity( new SequentialActivity() ) );
    tc->start();

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
    tc->getActivity()->trigger();

    // test results:
    BOOST_CHECK( sc->isProgramRunning("Foo") == false );
    BOOST_CHECK( sc->inProgramError("Foo") == false );
    BOOST_CHECK( ret == 10.0 );

}

BOOST_AUTO_TEST_CASE(TestScriptingParser)
{
    PluginLoader::Instance()->loadService("scripting",tc);

    // We use a sequential activity in order to force execution on trigger().
    tc->stop();
    BOOST_CHECK( tc->setActivity( new SequentialActivity() ) );
    tc->start();

    boost::shared_ptr<Scripting> sc = tc->getProvider<Scripting>("scripting");
    BOOST_REQUIRE( sc );
    BOOST_CHECK ( sc->ready() );
    bool r;

    // test plain statements:
    string statements=";;test.increase()\n;;;\ntest.increase()"; // trailing newline is optional
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 1);

    // test function +  a statement that uses that function:
    statements = "export function adder(int a, int b) { test.i = a + b; }\n adder(5,6)\n";
    r = sc->eval(statements);
    BOOST_CHECK( r );
    BOOST_CHECK_EQUAL( i, 11);

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


BOOST_AUTO_TEST_SUITE_END()
