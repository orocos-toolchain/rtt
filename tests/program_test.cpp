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



#include "program_test.hpp"

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

ProgramTest::ProgramTest()
	: gtc("root")
{
	setUp();
}


void
ProgramTest::setUp()
{
    SimulationThread::Instance()->stop();
    gtc.setActivity( new SimulationActivity(0.01) );

    Attribute<int> init_var("tvar_i");
    var_i = &init_var;
    const_i = new Constant<int>("tconst_i",-1);
    // ltc has a test object
    gtc.addObject( this->createObject(new TaskObject("test"), gtc.engine()->commands()) );

    // also this functions
    this->createObject( &gtc, gtc.engine()->commands());

    gtc.attributes()->addAttribute( &var_i );
    gtc.attributes()->addConstant( const_i );
    var_i.set(-1);
    i = 0;
    gtc.start();
}


void
ProgramTest::tearDown()
{
    gtc.stop();
    delete const_i;
}


bool ProgramTest::assertBool( bool b) {
    return b;
}
bool ProgramTest::assertMsg( bool b, const std::string& msg) {
    if ( b == false )
        cout << "Asserted :" << b << " with :" << msg << endl;
    return b;
}

int ProgramTest::increase() {
    return ++i;
}

void ProgramTest::resetI() {
    i = 0;
}


OperationInterface* ProgramTest::createObject(OperationInterface* dat, CommandProcessor* cp)
{
    // Add the data of the EE:
    dat->methods()->addMethod( method( "assert", &ProgramTest::assertBool, this),
                              "Assert", "bool", "" );
    dat->methods()->addMethod( method( "increase", &ProgramTest::increase, this),
                                "Return increasing i"  );
    dat->methods()->addMethod( method( "resetI", &ProgramTest::resetI, this),
                              "ResetI i" );
    dat->methods()->addMethod( method( "assertMsg", &ProgramTest::assertMsg, this),
                                 "Assert message", "bool", "", "text", "text"  );
    dat->methods()->addMethod( method( "isTrue", &ProgramTest::assertBool, this),
                              "Identity function", "bool", "" );
    dat->methods()->addMethod( method( "i", &ProgramTest::getI, this),
                         "Return the current number"  );
    dat->commands()->addCommand( command( "instantDone", &ProgramTest::true_genCom,
                                      &ProgramTest::true_gen, this, cp),
                                      "returns immediately" );
    dat->commands()->addCommand( command( "neverDone", &ProgramTest::true_genCom,
                                    &ProgramTest::false_gen, this, cp),
                                    "returns never" );
    dat->commands()->addCommand( command( "instantNotDone", &ProgramTest::true_genCom,
                                         &ProgramTest::true_gen, this, cp, false),
                                         "returns never" );
    dat->commands()->addCommand( command( "instantFail", &ProgramTest::false_genCom,
                                      &ProgramTest::true_gen, this, cp),
                                      "fails immediately" );
    dat->commands()->addCommand( command( "totalFail", &ProgramTest::false_genCom,
                                    &ProgramTest::false_gen, this, cp),
                                    "fails in command and condition" );
    return dat;
}

BOOST_FIXTURE_TEST_SUITE( ProgramTestSuite, ProgramTest )
// Registers the fixture into the 'registry'

BOOST_AUTO_TEST_CASE(testEmptyProgram)
{
    string prog = "";
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, &gtc );
    }
    catch( const file_parse_exception& exc )
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
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testParseProgram)
{
    // a program which should never fail
    // test this methods, commands etc.
    string prog = string("program x { do test.instantDone() \n")
        + " \n " + "and\n" + " test.instantDone() and test.instantDone()\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " do test.assert( test.i == 0 )\n"
        + " do test.increase()\n"
        + " do test.assert( test.i == 1 )\n"
        + " do test.resetI()\n"
        + " do test.assert( test.i == 0 )\n"
        + " if test.increase() + test.increase() + test.increase() != 6  then \n "
        + "    do test.assert( false )\n"
        + " do assert( isTrue( true ) )\n"
        + " do instantDone() \n"
        + " and instantDone() and instantDone()\n"
        + " do resetI()\n"
        + " do assert( i == 0 )\n"
        + " if increase() + increase() + increase() != 6  then \n "
        + "    do assert( false )\n"
        + " if true then\n"
        + "    return\n"
        + " do test.assert(false)"  // do not reach
        + "}";

    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramFailure)
{
    //also small test for program without newlines
    string prog = string("program x { do test.instantFail() until { ")
        + " if true then continue }"
        + "}";

    this->doProgram( prog, &gtc, false );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishProgram( &gtc, "x");
}
BOOST_AUTO_TEST_CASE(testProgramCondition)
{
    // see if checking a remote condition works
    string prog = string("program x { \n")
        + "if test.isTrue(true) then \n"
        + "  do test.instantDone() \n"
        + "else \n"
        + "  do test.instantFail() \n"
        + "if test.isTrue(false) then \n"
        + "  do test.instantFail() \n"
        + "else \n"
        + "  do test.instantDone() \n"
        + "if test.isTrue(true) then \n"
        + "   if test.isTrue(true) then \n"
        + "      if test.isTrue(false) then \n"
        + "         do test.instantFail() \n"
        + "      else \n"
        + "         if test.isTrue(false) then \n"
        + "            do test.instantFail() \n"
        + "         else \n"
        + "            do test.instantDone() \n"
        + "   else \n"
        + "      do test.instantFail() \n"
        + "else \n"
        + "   do test.instantFail() \n"
        + "var bool trig = false \n"
        + "do test.resetI()\n"
        + "while test.increase() != 100 && !trig \n"
        + "   if test.i == 50 then \n"
        + "       set trig = true \n"
        + "if test.i != 51 then \n" // the test.increase() will first increment i to 51, and then we detect trig.
        + "    do test.instantFail() \n"
        + "do test.resetI()\n"
        + "set trig = false\n"
        + "for (var int i = 0; i != 100 && !trig ; set i = test.increase() )\n"
        + "   if test.i == 50 then \n"
        + "       set trig = true \n"
//         + "if test.i != 51 then \n" // require same result as with ISO C
//         + "    do test.instantFail() \n"
        + "return "
        + "}";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramBreak)
{
    // see if  break statement works
    string prog = string("program x { \n")
        + "do test.resetI()\n"
        + "while (test.increase() != 100)\n"
        + "   if test.i == 50 then {\n"
        + "       break\n"
        + "       do test.instantFail() \n" // do not reach.
        + "   }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.instantFail() \n"
        + "do test.resetI()\n"
        + "for (var int i = 0; i != 100  ; set i = test.increase() )\n"
        + "   if test.i != 50 then \n"
        + "       do nothing \n"
        + "   else {\n"
        + "       break \n"
        + "       do test.instantFail() \n" // do not reach.
        + "     }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.instantFail() \n"
        + "do test.resetI()\n"
        + "while test.increase() != 100 {\n"
        + "   while test.increase() != 100 \n"
        + "     if test.i == 50 then {\n"
        + "         break \n"
        + "         do test.instantFail() \n" // do not reach.
        + "     }\n"
        + "   if test.i != 50 then \n" // break on 50
        + "      do test.instantFail() \n"
        + "   if test.i == 50 then \n"
        + "      break \n"
        + "   do test.instantFail() \n" // do not reach.
        + "   }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.instantFail() \n"
        + "return \n"
        + "}";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramAnd)
{
    // see if checking a remote condition works
    string prog = string("program x { do test.instantDone()\n")
        + "and test.instantDone() \n"
        + "and test.instantDone() \n"
        + " }";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramTry)
{
    // see if checking a remote condition works
    string prog = string("program progtry { try test.instantFail()\n")
        + "try instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.instantDone() \n"
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and totalFail() until { \n"
        + "  if true then continue }\n"
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.instantFail() catch { \n"
        + "  try test.instantDone() \n"
        + "  and test.instantDone() \n"
        + "  and test.instantDone() catch {\n"
        + "      do instantFail()\n"
        + "  }\n"
        + "}\n"
        + "do test.resetI() \n"
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.instantDone() until {\n"
        + "  if done then continue\n"
        + " } catch { \n"
        + "  do test.increase()\n"
        + "}\n"
        + "if test.i == 0 then\n" // check if catch was reached
        + "  do test.totalFail()\n"
        + "}";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "progtry");
}

BOOST_AUTO_TEST_CASE(testProgramTask)
{
    // see if checking a remote condition works
    string prog = string("export function foo {\n")
        + "  do assert( tvar_i == +2 ) \n"
        + "  do assert( tvar_i != tconst_i ) \n"
        + "  set tvar_i = +4\n"
        + "  do assert( tvar_i == +4 ) \n"
        + "}\n"
        + "program x { \n"
        + "do assert( tvar_i == -1 ) \n"
        + "do assert( tvar_i == tconst_i ) \n"
        + "set tvar_i = +2\n"
        + "do assert( tvar_i == +2 )\n"
        + "do foo()\n"
        + "}";
    this->doProgram( prog, &gtc );
    BOOST_REQUIRE_EQUAL( 4, gtc.attributes()->getAttribute<int>("tvar_i")->get() );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramUntil)
{
    // see if checking a remote condition works
    string prog = string("program proguntil {\n")
        +" do test.resetI()\n"
        +" do test.neverDone()\n"
        + "until { \n"
        + " if  time > 10 ms then continue \n" //  test in simulation takes far less than 1 second
        + "} \n"
        + "do neverDone()\n"
        + "until { \n"
        + " if done then continue \n"
        + " if test.increase() == 10 then continue \n" // we continue after 10 checks
        + "} \n"
        + " }";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "proguntil");
}

BOOST_AUTO_TEST_CASE(testProgramUntilFail)
{
    // see if checking a remote condition works
    string prog = string("program x { do test.instantFail()\n")
        + "until { \n"
        + " if done then continue \n" //  program should go into error
        + "} \n"
        + " }";
    this->doProgram( prog, &gtc, false );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramAssignArrayIndex)
{
    // see if assigning an array from an int works (bug #773)
    string prog = string("program x {\n")
        + "var array myArray(10);\n"
        + "var int i;\n"
        + "for (var int cpt = 0; cpt < 10; set cpt = cpt +1 )\n"
        + "{\n"
        + "   set myArray[cpt] = cpt;\n"
        + "   set i = cpt;\n"
        + "}\n"
        + "for (set cpt = 0; cpt < 10; set cpt = cpt +1 )\n"
        + "{\n"
        + "   do test.assert( myArray[cpt] == cpt );\n"
        + "}\n"
        + "}\n";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}


BOOST_AUTO_TEST_SUITE_END()

void ProgramTest::doProgram( const std::string& prog, TaskContext* tc, bool test )
{
    BOOST_CHECK( tc->engine() );
    BOOST_CHECK( tc->engine()->programs());

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
            BOOST_REQUIRE( false );
        }
    ProgramInterfacePtr pi = *pg_list.begin();

    tc->engine()->programs()->loadProgram( pi );
    BOOST_CHECK( pi->start() );
    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << pi->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( pi->getStatus() != ProgramInterface::Status::error , "Runtime error encountered" + errormsg.str());
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
    BOOST_CHECK( tc->engine()->programs()->getProgram( prog_name )->stop() );
    tc->engine()->programs()->unloadProgram( prog_name );

}
