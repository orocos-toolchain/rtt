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
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <rtt/FunctionGraph.hpp>
#include <rtt/SimulationThread.hpp>
#include <rtt/TaskBrowser.hpp>
#include <rtt/Method.hpp>
#include <rtt/Command.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProgramTest );


    ProgramTest::ProgramTest()
        : gtc("root"),
          gtask( 0.01, gtc.engine() )
    {}


void 
ProgramTest::setUp()
{
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
}


void 
ProgramTest::tearDown()
{
    delete const_i;
    SimulationThread::Instance()->stop();
    gtask.stop();
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

void ProgramTest::reset() {
    i = 0;
}


OperationInterface* ProgramTest::createObject(OperationInterface* dat, CommandProcessor* cp)
{
    // Add the data of the EE:
    dat->methods()->addMethod( method( "assert", &ProgramTest::assertBool, this),
                              "Assert", "bool", "" );
    dat->methods()->addMethod( method( "increase", &ProgramTest::increase, this),
                                "Return increasing i"  );
    dat->methods()->addMethod( method( "reset", &ProgramTest::reset, this),
                              "Reset i" );
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


void ProgramTest::testParseProgram()
{
    // a program which should never fail
    // test this methods, commands etc.
    string prog = string("program x { do test.instantDone() \n")
        + " \n and \n test.instantDone()"
        + " and test.instantDone()\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " do test.assert( test.i == 0 )\n"
        + " do test.increase()\n"
        + " do test.assert( test.i == 1 )\n"
        + " do test.reset()\n"
        + " do test.assert( test.i == 0 )\n"
        + " if test.increase() + test.increase() + test.increase() != 6  then \n "
        + "    do test.assert( false )\n"
        + " do assert( isTrue( true ) )\n"
        + " do instantDone() \n"
        + " and instantDone() and instantDone()\n"
        + " do reset()\n"
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

void ProgramTest::testProgramFailure()
{
    //also small test for program without newlines
    string prog = string("program x { do test.instantFail() until { ")
        + " if true then continue }"
        + "}";

    this->doProgram( prog, &gtc, false );

    CPPUNIT_ASSERT( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishProgram( &gtc, "x");
}
void ProgramTest::testProgramCondition()
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
        + "do test.reset()\n"
        + "while test.increase() != 100 && !trig \n"
        + "   if test.i == 50 then \n"
        + "       set trig = true \n"
        + "if test.i != 51 then \n" // the test.increase() will first increment i to 51, and then we detect trig.
        + "    do test.instantFail() \n"
        + "do test.reset()\n"
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

void ProgramTest::testProgramBreak()
{
    // see if  break statement works
    string prog = string("program x { \n")
        + "do test.reset()\n"
        + "while (test.increase() != 100)\n"
        + "   if test.i == 50 then {\n"
        + "       break\n"
        + "       do test.instantFail() \n" // do not reach.
        + "   }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.instantFail() \n"
        + "do test.reset()\n"
        + "for (var int i = 0; i != 100  ; set i = test.increase() )\n"
        + "   if test.i != 50 then \n"
        + "       do nothing \n"
        + "   else {\n"
        + "       break \n"
        + "       do test.instantFail() \n" // do not reach.
        + "     }\n"
        + "if test.i != 50 then \n" // break on 50
        + "    do test.instantFail() \n"
        + "do test.reset()\n"
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

void ProgramTest::testProgramAnd()
{
    // see if checking a remote condition works
    string prog = string("program x { do test.instantDone()\n")
        + "and test.instantDone() \n"
        + "and test.instantDone() \n"
        + " }";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

void ProgramTest::testProgramTry()
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
        + "do test.reset() \n"
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

void ProgramTest::testProgramTask()
{
    // see if checking a remote condition works
    string prog = string("export function foo {\n")
        + "  do assert( task.tvar_i == +2 ) \n"
        + "  do assert( task.tvar_i != task.tconst_i ) \n"
        + "  set task.tvar_i = +4\n"
        + "  do assert( task.tvar_i == +4 ) \n"
        + "}\n"
        + "program x { \n"
        + "do assert( task.tvar_i == -1 ) \n"
        + "do assert( task.tvar_i == task.tconst_i ) \n"
        + "set task.tvar_i = +2\n"
        + "do assert( task.tvar_i == +2 )\n"
        + "do foo()\n"
        + "}";
    this->doProgram( prog, &gtc );
    CPPUNIT_ASSERT_EQUAL( 4, gtc.attributes()->getAttribute<int>("tvar_i")->get() );
    this->finishProgram( &gtc, "x");
}

void ProgramTest::testProgramUntil()
{
    // see if checking a remote condition works
    string prog = string("program proguntil {\n")
        +" do test.reset()\n"
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

void ProgramTest::testProgramUntilFail()
{
    // see if checking a remote condition works
    string prog = string("program x { do test.instantFail()\n")
        + "until { \n"
        + " if done then continue \n" //  program should go into error
        + "} \n"
        + " }";
    this->doProgram( prog, &gtc, false );

    CPPUNIT_ASSERT( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishProgram( &gtc, "x");
}

void ProgramTest::doProgram( const std::string& prog, TaskContext* tc, bool test )
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
            CPPUNIT_ASSERT( false );
        }
    ProgramInterfacePtr pi = *pg_list.begin();

    tc->engine()->programs()->loadProgram( pi );
    CPPUNIT_ASSERT( gtask.start() );
    CPPUNIT_ASSERT( pi->start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1000) );

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << pi->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( "Runtime error encountered" + errormsg.str(), pi->getStatus() != ProgramInterface::Status::error );
        CPPUNIT_ASSERT_MESSAGE( "Program stalled " + errormsg.str(), pi->getStatus() == ProgramInterface::Status::stopped );

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
    CPPUNIT_ASSERT( gtask.stop() );
    CPPUNIT_ASSERT( tc->engine()->programs()->getProgram( prog_name )->stop() );
    tc->engine()->programs()->unloadProgram( prog_name );

    TaskContext* ptc= tc->getPeer("programs")->getPeer(prog_name);
    tc->getPeer("programs")->removePeer(prog_name);
    delete ptc;

    ptc= tc->getPeer("programs");
    tc->removePeer("programs");
    delete ptc;
}

    

