/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  program_test.cpp

                        program_test.cpp -  description
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

#include <iostream>
#include <sstream>
#include <string>

#include <scripting/Parser.hpp>
#include <scripting/FunctionGraph.hpp>
#include <scripting/ScriptingService.hpp>
#include <extras/SimulationThread.hpp>
#include <extras/SimulationActivity.hpp>
#include <Service.hpp>

#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include "operations_fixture.hpp"

using namespace RTT;
using namespace RTT::detail;
using namespace std;



class ProgramTest : public OperationsFixture
{
public:
    Parser parser;
    ScriptingService::shared_ptr sa;
    int var_i;
    int const_i;
    SendStatus tss;

    void doProgram( const std::string& prog, TaskContext*, bool test=true );
    void finishProgram( TaskContext* , std::string );
    void loopProgram( ProgramInterfacePtr );

    ProgramTest()
        : parser(tc->engine()), sa( ScriptingService::Create(tc) )
    {
        tc->stop();
        BOOST_REQUIRE( tc->setActivity(new SimulationActivity(0.01)) );
        BOOST_REQUIRE( tc->start() );
        tc->provides()->addService( sa );
        tc->provides()->addAttribute("tvar_i", var_i);
        tc->provides()->addAttribute("tss", tss);
        tc->provides()->addConstant("tconst_i", const_i);
        // ltc has a test object

        const_i = -1;
        var_i = -1;
        i = 0;
        tss = SendNotReady;
        SimulationThread::Instance()->stop();
    }

    ~ProgramTest()
    {
    }

};

BOOST_FIXTURE_TEST_SUITE( ProgramTestSuite, ProgramTest )
// Registers the fixture into the 'registry'

BOOST_AUTO_TEST_CASE(testEmptyProgram)
{
    string prog = "";
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& /*exc*/ )
        {
            BOOST_CHECK( false );
        }
    if ( !pg_list.empty() )
        {
            BOOST_CHECK( false );
        }
}

BOOST_AUTO_TEST_CASE(testReturnProgram)
{
    string prog = "program x { return \n }";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

// check that loading a faulty program causes an exception
// and that the program is not present as a service in tc
BOOST_AUTO_TEST_CASE(testProgramError)
{
    string prog = "program x { not_exist = 10\n }";
    Parser::ParsedPrograms pg_list;
    BOOST_CHECK_THROW(pg_list = parser.parseProgram( prog, tc ),file_parse_exception);
    BOOST_CHECK( pg_list.empty() );
    BOOST_REQUIRE( tc->provides()->hasService("x") == false);

    BOOST_CHECK( sa->runScript(prog) == false);
    BOOST_REQUIRE( tc->provides()->hasService("x") == false);
}

// same as above, but with runScript
BOOST_AUTO_TEST_CASE(testrunScriptError)
{
    string prog = "tvar_i = 33\n program x { not_exist = 10\n } \n tvar_i = 66\n";
    BOOST_CHECK( sa->eval(prog) == false);
    BOOST_CHECK_EQUAL( var_i, 33 );
    BOOST_REQUIRE( tc->provides()->hasService("x") == false);
}

// tests if the text is properly saved
BOOST_AUTO_TEST_CASE(test_getProgramText)
{
    // a program which should never fail
    // test this methods.
    string prog = string("/** This is the test_getProgramText to parse */\nprogram x { \n")
        + " true\n"
        + "}";

    this->doProgram( prog, tc );
    BOOST_CHECK_EQUAL( sa->getProgramText("x"), prog );
    this->finishProgram( tc, "x");
}

// tests if the text is properly saved in runScript
BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(test_getProgramText_runScript, 1)
BOOST_AUTO_TEST_CASE(test_getProgramText_runScript)
{
    // a program which should never fail
    // test this methods.
    string prog = string("/** This is the test_getProgramText_runScript to parse */\nprogram x { \n")
        + " true\n"
        + "}\n";

    string evaled = string("tvar_i = 33\n") + prog + "tvar_i = 66\n";

    BOOST_CHECK( sa->eval(evaled) == true);
    BOOST_CHECK_EQUAL( sa->getProgramText("x"), evaled + "\n" );
}

BOOST_AUTO_TEST_CASE(testParseProgram)
{
    // a program which should never fail
    // test this methods.
    string prog = string("program x { \n")
        + " do true\n"
        + " do test.assert( true )\n"
        + " do test.increase()\n"
        + " do test.resetI()\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " do test.assert( test.i == 0 )\n"
        + " do test.increase()\n"
        + " do test.assert( test.i == 1 )\n"
        + " do test.resetI()\n"
        + " do test.assert( test.i == 0 )\n"
        + " if test.increase() + test.increase() + test.increase() != 6  then \n "
        + "    do test.assert( false )\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " do test.resetI()\n"
        + " do test.assert( test.i == 0 )\n"
        + " if test.increase() + test.increase() + test.increase() != 6  then \n "
        + "    do test.assert( false )\n"
        + " if true then\n"
        + "    return\n"
        + " do test.assert(false)"  // do not reach
        + "}";

    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramFailure)
{
    //also small test for program without newlines
    string prog = string("program x { do test.fail();}");

    this->doProgram( prog, tc, false );

    BOOST_CHECK( sa->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishProgram( tc, "x");
}
BOOST_AUTO_TEST_CASE(testProgramCondition)
{
    // see if checking a remote condition works
    string prog = string("program x { \n")
        + "if test.isTrue(true) then \n"
        + "  do test.good() \n"
        + "else \n"
        + "  do test.fail() \n"
        + "if test.isTrue(false) then \n"
        + "  do test.fail() \n"
        + "else \n"
        + "  do test.good() \n"
        + "if test.isTrue(true) then \n" //10
        + "   if test.isTrue(true) then \n"
        + "      if test.isTrue(false) then \n"
        + "         do test.fail() \n"
        + "      else \n"
        + "         if test.isTrue(false) then \n"
        + "            do test.fail() \n"
        + "         else \n"
        + "            do test.good() \n"
        + "   else \n"
        + "      do test.fail() \n" //20
        + "else \n"
        + "   do test.fail() \n"
        + "var bool trig = false \n"
        + "do test.resetI()\n"
        + "while test.increase() != 100 && !trig \n"
        + "   if test.i == 50 then \n"
        + "       set trig = true \n"
        + "if test.i != 51 then \n" // the test.increase() will first increment i to 51, and then we detect trig.
        + "    do test.fail() \n"
        + "do test.resetI()\n" //30
        + "set trig = false\n"
        + "for (var int j = 0; j != 100 && !trig ; set j = test.increase() )\n"
        + "   if j == 50 then \n"
        + "       set trig = true \n"
//         + "if test.i != 51 then \n" // require same result as with ISO C
//         + "    do test.fail() \n"
        + "return "
        + "}";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramBreak)
{
    // see if  break statement works
    string prog = string("program x { \n")
        + "do test.resetI()\n"
        + "while (test.increase() != 100)\n"
        + "   if test.i == 50 then {\n"
        + "       break\n"
        + "       do test.fail() \n" // do not reach.
        + "   }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        + "for (var int j = 0; j != 100  ; set j = test.increase() )\n"
        + "   if j != 50 then \n"
        + "       do nothing \n"
        + "   else {\n"
        + "       break \n"
        + "       do test.fail() \n" // do not reach.
        + "     }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        + "while test.increase() != 100 {\n"
        + "   while test.increase() != 100 \n"
        + "     if test.i == 50 then {\n"
        + "         break \n"
        + "         do test.fail() \n" // do not reach.
        + "     }\n"
        + "   if test.i != 50 then \n" // break on 50
        + "      do test.fail() \n"
        + "   if test.i == 50 then \n"
        + "      break \n"
        + "   do test.fail() \n" // do not reach.
        + "   }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.fail() \n"
        + "return \n"
        + "}";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramLoops)
{
    // see if (nested) loop statements work
    string prog = string("program x { \n")
        + "do test.resetI()\n"
        // single while loop
        + "while (test.increase() != 100) {\n"
        + "}\n"
        + "if test.i != 100 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // double while loop
        + "while (test.increase() != 200) {\n"
        + "   while (test.i < 100) {\n"
        + "       test.increase()\n"
        + "   }\n"
        + "   if test.i < 100 then \n"
        + "      do test.fail() \n"
        + "}\n"
        + "if test.i != 200 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // single for loop
        + "for (var int j = 0; j != 100  ; j = test.increase() ) {\n"
        + "}\n"
        + "if test.i != 100 then \n" // 20
        + "    do test.fail() \n"
        + "if j != 100 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // double for loop
        + "for ( j = 0; j != 100  ; j = test.increase() ) {\n"
        + "   for (var int j2 = 0; j2 != 100  ; j2 = j2 + 1 ) {\n"
        + "   }\n"
        + "   if j2 != 100 then \n"
        + "      do test.fail() \n"
        + "}\n"
        + "if test.i != 100 then \n"
        + "    do test.fail() \n"
        + "if j != 100 then \n"
        + "    do test.fail() \n"
        + "if j2 != 100 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // for loop in while loop
        + "while (test.increase() != 200) {\n"
        + "   for (var int j3 = 0; j3 != 100  ; j3 = j3 + 1 ) {\n"
        + "   }\n"   // 40
        + "}\n"
        + "if test.i != 200 then \n"
        + "    do test.fail() \n"
        + "if j3 != 100 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // for loop in while loop + break in for
        + "while (test.increase() != 200) {\n"
        + "   for (var int j3b = 0; j3b != 100  ; j3b = j3b + 1 ) {\n"
        + "      if j3b == 50 then break \n"
        + "   }\n"
        + "   if j3b != 50 then \n"
        + "      do test.fail() \n"
        + "}\n"
        + "if test.i != 200 then \n"
        + "    do test.fail() \n"
        + "if j3b != 50 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // while loop in for loop
        + "for (var int j4 = 0; j4 != 100  ; j4 = j4 + 1 ) {\n"
        + "   test.resetI()\n" // 60
        + "   while (test.increase() != 200) {\n"
        + "   }\n"
        + "}\n"
        + "if test.i != 200 then \n"
        + "    do test.fail() \n"
        + "if j4 != 100 then \n"
        + "    do test.fail() \n"
        + "do test.resetI()\n"
        // while loop in for loop + break in while
        + "for (var int j5 = 0; j5 != 100  ; j5 = j5 + 1 ) {\n"
        + "   test.resetI()\n"
        + "   while (test.increase() != 200) {\n"
        + "      if test.i == 50 then break \n"
        + "   }\n"
        + "   if test.i != 50 then test.fail() \n"
        + "}\n"
        + "if test.i != 50 then \n"
        + "    do test.fail() \n"
        + "if j5 != 100 then  {\n"
        + "    do test.print(\" j5 is:\" + j5 ) \n"
        + "    do test.fail() \n" // 80
        + " }\n"
        + "do test.resetI()\n"
        + "}";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramAnd)
{
    // see if checking a remote condition works
    string prog = string("program x { do test.good()\n")
        + " && test.good() \n"
        + " && test.good() \n"
        + " }";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramTry)
{
    // see if checking a remote condition works
    string prog = string("program progtry { try test.fail()\n")
        + "try test.good() \n"
        + " && test.fail() \n"
        + " && test.good() \n"
        + "try test.good() \n"
        + " && test.fail() \n"
        + "try test.good() \n"
        + " && test.fail() \n"
        + " && test.fail() catch { \n"
        + "  try test.good() \n"
        + "   && test.good() \n"
        + "   && test.good() catch {\n"
        + "      do test.fail()\n"
        + "  }\n"
        + "}\n"
        + "do test.resetI() \n"
        + "try test.good() \n"
        + " && test.fail() \n"
        + " && test.good() catch { \n"
        + "  do test.increase()\n"
        + "}\n"
        + "if test.i == 0 then\n" // check if catch was reached
        + "  do test.fail()\n"
        + "}";
    this->doProgram( prog, tc );
    this->finishProgram( tc, "progtry");
}

BOOST_AUTO_TEST_CASE(testProgramToProgram)
{
    // test a program which starts/stops another program.
    string prog = string("program y { do test.good() \n")
        + " do test.resetI()\n"
        + " do test.assert( test.i == 0 )\n"
        + "}";

    string prog2 = string("program x {\n")
        + " do test.assert( test.i == 0 )\n"
        + " do test.increase()\n"
        + " do test.assert( test.i == 1 )\n"
        + " do y.start()\n"         // test start-stop
        + " do yield\n"
        + " do test.assert( test.i == 0 )\n"
        + " do y.stop()\n"
        + " do test.increase()\n"
        + " do y.pause()\n"        // test pause-step // 10
        + " do yield\n"
        + " do test.assert( y.isPaused() )\n"
        + " do test.assert( test.i == 1 )\n"
        + " do y.step()\n"
        + " do yield\n"
        + " do test.assert( test.i == 1 )\n"
        + " do y.step()\n"
        + " do yield\n"
        + " do test.assertEqual( test.i, 0 )\n" // if this fails, stepping is broken
        + " do y.step()\n"
        + " do yield\n"
        + " do y.step()\n"         // 20
        + " do yield\n"
        + " do test.assert( !y.isRunning() )\n"
        + "}";

    this->doProgram( prog, tc );
    this->doProgram( prog2, tc );
    this->finishProgram( tc, "x");
    this->finishProgram( tc, "y");
}

BOOST_AUTO_TEST_CASE(testProgramCallFoo)
{
    // see if modifying an attribute works.
    string prog = string("export function foo {\n")
        + "  do test.assert( tvar_i == +2 ) \n"
        + "  do test.assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = +4\n"
        + "  do test.assert( tvar_i == +4 ) \n"
        + "}\n"
        + "program x { \n"
        + "do test.assert( tvar_i == -1 ) \n"
        + "do test.assert( tvar_i == tconst_i ) \n"
        + "set tvar_i = +2\n"
        + "do test.assert( tvar_i == +2 )\n"
        + "call foo()\n"
        + "do test.assert( tvar_i == +4 ) \n"
        + "}";
    this->doProgram( prog, tc );
    BOOST_REQUIRE_EQUAL( 4, var_i );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramSendFoo)
{
    // see if modifying an attribute works.
    string prog = string("export function foo(int arg) {\n")
        + "  do test.assert( tvar_i == arg ) \n"
        + "  do test.assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = tvar_i+2\n"
        + "  do test.assert( tvar_i == arg + 2 ) \n"
        + "}\n"
        + "program x { \n"
        + "do test.assert( tvar_i == -1 ) \n"
        + "do test.assert( tvar_i == tconst_i ) \n"
        + "set tvar_i = +2\n" // 10
        + "var SendHandle sh\n"
        + "sh = foo.send(tvar_i)\n"
        + "while(sh.collectIfDone() != SendSuccess)\n"
        + "   yield\n"
        + "do test.assert( sh.collectIfDone() == SendSuccess )\n"
        + "do test.assert( tvar_i == +4 )\n"

        // test parallel send
        + "var SendHandle sh1, sh2\n"
        + "sh1 = foo.send(tvar_i)\n"
        + "sh2 = foo.send(tvar_i + 2)\n"
        + "while(sh2.collectIfDone() != SendSuccess)\n"
        + "   yield\n"
        + "do test.assert( tvar_i == +8 )\n"
        + "do test.assert( sh1.collectIfDone() == SendSuccess )\n"
        + "do test.assert( sh2.collectIfDone() == SendSuccess )\n"
        + "do test.assert( tvar_i == +8 )\n"
        + "}";
    this->doProgram( prog, tc );
    BOOST_REQUIRE_EQUAL( 8, var_i );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramCmdFoo)
{
    // see if modifying an attribute works.
    string prog = string("export function foo(int arg) {\n")
        + "  do test.assert( tvar_i == arg ) \n"
        + "  do test.assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = tvar_i+2\n"
        + "  do test.assert( tvar_i == arg + 2 ) \n"
        + "}\n"
        + "program x { \n"
        + "do test.assert( tvar_i == -1 ) \n"
        + "do test.assert( tvar_i == tconst_i ) \n"
        + "set tvar_i = +2\n" // 10
        + "while (tvar_i != +6) {\n"
        + "   tss = foo.cmd(tvar_i)\n"
        + "   do test.assert( tss == SendSuccess )\n"
        + "}\n"
        + "do test.assert( tvar_i == +6 )\n"
        + "tss = foo.cmd(tvar_i)\n"
        + "do test.assert( tvar_i == +8 )\n"
        + "do test.assert( tss == SendSuccess )\n"
        + "}";
    this->doProgram( prog, tc );
    BOOST_REQUIRE_EQUAL( 8, var_i );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testSend)
{
    // see if modifying an attribute works.
    string prog = string("")
        + "program x { \n"
        + "test.assertEqual( test.i, 0 )\n"
        + "test.increaseCmd.send() \n"
        + "yield \n"
        + "test.assertEqual( test.i, 1 )\n"
        + "yield \n" // make sure that increaseCmd is not evaluated twice!
        + "test.assertEqual( test.i, 1 )\n"

        + "var SendHandle sh\n"
        + "set sh = test.increaseCmd.send()\n"
        + "test.assertEqual( test.i, 1 )\n" // not yet send
        + "var int r = 0\n"
        //+ "sh.collect(r)\n" // hangs
        + "while (sh.collectIfDone(r) != SendSuccess)\n"
        + "    yield \n"
        + "test.assertEqual( r , 2 )\n"
        + "test.assertEqual( test.i, 2 )\n"

        + "set sh = test.increaseCmd.send()\n"
        //+ "sh.collect(tvar_i)\n" // hangs
        + "while (sh.collectIfDone(tvar_i) != SendSuccess)\n"
        + "    yield \n"
        + "test.assertEqual( tvar_i, 3 )\n" // i is 3 but r isn't.
        + "}";
    this->doProgram( prog, tc );
    BOOST_REQUIRE_EQUAL( i, 3 );
    BOOST_REQUIRE_EQUAL( var_i, 3 );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_CASE(testCmd)
{
    // see if modifying an attribute works.
    string prog = string("")
        + "program x { \n"
        + "test.assertEqual( test.i, 0 )\n"
        + "var SendStatus ss\n"
        + "tss = test.increaseCmd.cmd() \n"
        + "test.assert( tss == SendSuccess )\n"
        + "test.assertEqual( test.i, 1 )\n"
        + "ss = test.increaseCmd.cmd()\n"
        + "test.assert( ss == SendSuccess )\n"
        + "test.assertEqual( test.i , 2 )\n"

        + "tss = methods.vo0.cmd() \n"
        + "test.assert( tss == SendSuccess )\n"
        + "ss = methods.vo0.cmd()\n"
        + "test.assert( ss == SendSuccess )\n"
        + "}";
    this->doProgram( prog, tc );
    BOOST_REQUIRE( tss == SendSuccess );
    BOOST_CHECK_EQUAL( i, 2 );
    this->finishProgram( tc, "x");
}

BOOST_AUTO_TEST_SUITE_END()

void ProgramTest::doProgram( const std::string& prog, TaskContext* tc, bool test )
{
    BOOST_CHECK( tc->engine() );

    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( false, exc.what() );
        }
    if ( pg_list.empty() )
        {
            BOOST_REQUIRE_MESSAGE( false, "No program could be found by the parser." );
        }
    ProgramInterfacePtr pi = *pg_list.begin();

    sa->loadProgram( pi );
    BOOST_CHECK( pi->start() );
    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << pi->getLineNumber() <<"."<<endl;
        BOOST_REQUIRE_MESSAGE( pi->getStatus() != ProgramInterface::Status::error , "Runtime error encountered" + errormsg.str());
        BOOST_CHECK_MESSAGE( pi->getStatus() == ProgramInterface::Status::stopped , "Program stalled " + errormsg.str());

        // Xtra test, only do it if all previous went ok :
        loopProgram( pi );
    }
}

void ProgramTest::loopProgram( ProgramInterfacePtr f)
{
    //std::cerr <<std::endl<< "Looping " << f->getName();
    // especially handy for performance testing :
    // This bypasses the processor however, does not
    // measure its performance.
    int loops = 100;
    f->reset();
    while ( loops-- != 0 ) {
        while ( !f->isStopped() && !f->inError() )
            f->execute();
        f->reset();
        //std::cerr << ".";
    }
}


void ProgramTest::finishProgram(TaskContext* tc, std::string prog_name)
{
    BOOST_REQUIRE( sa->getProgram(prog_name) );
    BOOST_CHECK( sa->getProgram( prog_name )->stop() );
    BOOST_CHECK( sa->unloadProgram( prog_name ) );

}
