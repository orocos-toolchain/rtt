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



#include <iostream>
#include <sstream>
#include <string>

#include <scripting/Parser.hpp>
#include <scripting/FunctionGraph.hpp>
#include <scripting/ScriptingService.hpp>
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



class ProgramTest
{
public:
    Parser parser;
    TaskContext gtc;
    ScriptingService* sa;
    ServiceProvider::shared_ptr createObject( ServiceProvider::shared_ptr);
    Attribute<int> var_i;
    Constant<int>* const_i;

    bool assertEqual(int a, int b) { if (a != b) { cout << "Not Equal a!=b ("<<a<<" != "<< b<<")"<<endl; return false;} return true;}
    bool assertBool( bool b) { return b; }
    bool assertMsg( bool b, const std::string& msg) {
        if ( b == false )
            cout << "Asserted :" << msg << endl;
        return true; // allow to continue to check other commands.
    }
    int increase() { return ++i;}
    void resetI() { i = 0; }

    void doProgram( const std::string& prog, TaskContext*, bool test=true );
    void finishProgram( TaskContext* , std::string );
    void loopProgram( ProgramInterfacePtr );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int getI() const { return i; }

    int i;

    ProgramTest()
    : gtc("root"),
      sa( new ScriptingService(&gtc) )
    {
        gtc.setActivity(new SimulationActivity(0.01));
        Attribute<int> init_var("tvar_i");
        var_i = &init_var;
        const_i = new Constant<int>("tconst_i",-1);
        // ltc has a test object
        gtc.provides()->addService( this->createObject(ServiceProvider::Create("test") ) );

        // also this functions
        this->createObject( gtc.provides() );

        gtc.provides()->addAttribute( var_i );
        gtc.provides()->addConstant( *const_i );
        var_i.set(-1);
        i = 0;
        SimulationThread::Instance()->stop();
    }

    ~ProgramTest()
    {
        delete const_i;
    }

};

ServiceProvider::shared_ptr ProgramTest::createObject(ServiceProvider::shared_ptr dat)
{
    // Add the data of the EE:
    dat->addOperation("assert", &ProgramTest::assertBool, this).doc("Assert").arg("bool", "");
    dat->addOperation("increase", &ProgramTest::increase, this).doc("Return increasing i");
    dat->addOperation("increaseCmd", &ProgramTest::increase, this, OwnThread).doc("Return increasing i");
    dat->addOperation("resetI", &ProgramTest::resetI, this).doc("ResetI i");
    dat->addOperation("assertMsg", &ProgramTest::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
    dat->addOperation("assertEqual", &ProgramTest::assertEqual, this);
    dat->addOperation("isTrue", &ProgramTest::assertBool, this).doc("Identity function").arg("bool", "");
    dat->addOperation("i", &ProgramTest::getI, this).doc("Return the current number");
    dat->addOperation("instantDone", &ProgramTest::true_genCom, this).doc("returns immediately");
    dat->addOperation("instantDoneDone", &ProgramTest::true_gen, this).doc("Returns true when instantDone is done.");
    dat->addOperation("neverDone", &ProgramTest::true_genCom, this).doc("returns never");
    dat->addOperation("neverDoneDone", &ProgramTest::false_gen, this).doc("Returns true when neverDone is done.");
    dat->addOperation("instantFail", &ProgramTest::false_genCom, this).doc("fails immediately");
    dat->addOperation("instantFailDone", &ProgramTest::true_gen, this).doc("Returns true when instantFail is done.");
    dat->addOperation("totalFail", &ProgramTest::false_genCom, this).doc("fails in command and condition");
    dat->addOperation("totalFailDone", &ProgramTest::false_gen, this).doc("Returns true when totalFail is done.");
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
#if 0
        + " \n " + "&&\n" + " test.instantDone() && test.instantDone()\n"
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
        + " && instantDone() && instantDone()\n"
        + " do resetI()\n"
        + " do assert( i == 0 )\n"
        + " if increase() + increase() + increase() != 6  then \n "
        + "    do assert( false )\n"
#endif
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
    string prog = string("program x { do test.instantFail();}");

    this->doProgram( prog, &gtc, false );

    BOOST_CHECK( sa->getProgramStatus("x") == ProgramInterface::Status::error );

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
        + " && test.instantDone() \n"
        + " && test.instantDone() \n"
        + " }";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramTry)
{
    // see if checking a remote condition works
    string prog = string("program progtry { try test.instantFail()\n")
        + "try instantDone() \n"
        + " && test.instantFail() \n"
        + " && test.instantDone() \n"
        + "try test.instantDone() \n"
        + " && test.instantFail() \n"
        + " && totalFail()\n"
        + "try test.instantDone() \n"
        + " && test.instantFail() \n"
        + " && test.instantFail() catch { \n"
        + "  try test.instantDone() \n"
        + "   && test.instantDone() \n"
        + "   && test.instantDone() catch {\n"
        + "      do instantFail()\n"
        + "  }\n"
        + "}\n"
        + "do test.resetI() \n"
        + "try test.instantDone() \n"
        + " && test.instantFail() \n"
        + " && test.instantDone() catch { \n"
        + "  do test.increase()\n"
        + "}\n"
        + "if test.i == 0 then\n" // check if catch was reached
        + "  do test.totalFail()\n"
        + "}";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "progtry");
}

BOOST_AUTO_TEST_CASE(testProgramToProgram)
{
    // test a program which starts/stops another program.
    string prog = string("program y { do test.instantDone() \n")
        + " do resetI()\n"
        + " do assert( i == 0 )\n"
        + "}";

    string prog2 = string("program x {\n")
        + " do assert( i == 0 )\n"
        + " do test.increase()\n"
        + " do assert( i == 1 )\n"
        + " do y.start()\n"         // test start-stop
        + " do yield\n"
        + " do assert( i == 0 )\n"
        + " do y.stop()\n"
        + " do test.increase()\n"
        + " do y.pause()\n"        // test pause-step // 10
        + " do yield\n"
        + " do assert( y.isPaused() )\n"
        + " do assert( i == 1 )\n"
        + " do y.step()\n"
        + " do yield\n"
        + " do assert( i == 1 )\n"
        + " do y.step()\n"
        + " do yield\n"
        + " do assert( i == 0 )\n"
        + " do y.step()\n"
        + " do yield\n"
        + " do y.step()\n"         // 20
        + " do yield\n"
        + " do assert( !y.isRunning() )\n"
        + "}";

    this->doProgram( prog, &gtc );
    this->doProgram( prog2, &gtc );
    this->finishProgram( &gtc, "x");
    this->finishProgram( &gtc, "y");
}

BOOST_AUTO_TEST_CASE(testProgramCallFoo)
{
    // see if modifying an attribute works.
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
        + "call foo()\n"
        + "}";
    this->doProgram( prog, &gtc );
    Attribute<int> i = gtc.provides()->getAttribute("tvar_i");
    BOOST_REQUIRE_EQUAL( 4, i.get() );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testProgramDoFoo)
{
    // see if modifying an attribute works.
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
    Attribute<int> i = gtc.provides()->getAttribute("tvar_i");
    BOOST_REQUIRE_EQUAL( 4, i.get() );
    this->finishProgram( &gtc, "x");
}

BOOST_AUTO_TEST_CASE(testSend)
{
    // see if modifying an attribute works.
    string prog = string("")
        + "program x { \n"
        + "assertEqual( i, 0 )\n"
        + "increaseCmd.send() \n"
        + "yield \n"
        + "assertEqual( i, 1 )\n"
        + "var SendHandle sh\n"
        + "set sh = increaseCmd.send()\n"
        + "var int r = 0\n"
        //+ "sh.collect(r)\n" // hangs
        + "while (sh.collectIfDone(r) != SendSuccess)\n"
        + "yield \n"
        + "assertEqual( r , 2 )\n"
        + "set sh = increaseCmd.send()\n"
        //+ "sh.collect(tvar_i)\n" // hangs
        + "while (sh.collectIfDone(tvar_i) != SendSuccess)\n"
        + "yield \n"
        + "assertEqual( tvar_i, 3 )\n" // i is 3 but r isn't.
        + "}";
    this->doProgram( prog, &gtc );
    BOOST_REQUIRE_EQUAL( i, 3 );
    BOOST_REQUIRE_EQUAL( var_i.get(), 3 );
    this->finishProgram( &gtc, "x");
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
