
#include "program_test.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <execution/ProgramGraph.hpp>
#include <corelib/SimulationThread.hpp>
#include <execution/TemplateFactories.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProgramTest );


    ProgramTest::ProgramTest()
        : gtc("root", &gprocessor),
          gtask( 0.01, &gprocessor )
    {}


void 
ProgramTest::setUp()
{
    // ltc has a test object
    gtc.methodFactory.registerObject("test", this->createMethodFactory() );
    gtc.commandFactory.registerObject("test", this->createCommandFactory() );
    gtc.dataFactory.registerObject("test", this->createDataSourceFactory() );

    i = 0;
}


void 
ProgramTest::tearDown()
{
    gtc.methodFactory.unregisterObject( "test" );
    gtc.commandFactory.unregisterObject( "test" );
    gtc.dataFactory.unregisterObject( "test" );
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


MethodFactoryInterface* ProgramTest::createMethodFactory()
{
    // Add the data of the EE:
    TemplateMethodFactory< ProgramTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &ProgramTest::assertBool,
                              "Assert", "bool", "") );
    dat->add( "increase", method( &ProgramTest::increase,
                                "Return increasing i" ) );
    dat->add( "reset", method( &ProgramTest::reset,
                              "Reset i") );
    dat->add( "assertMsg", method( &ProgramTest::assertMsg,
                                 "Assert message", "bool", "", "text", "text" ) );
    return dat;
}

DataSourceFactoryInterface* ProgramTest::createDataSourceFactory()
{
    // Add the data of the EE:
    TemplateDataSourceFactory< ProgramTest >* dat =
        newDataSourceFactory( this );

    dat->add( "isTrue", data( &ProgramTest::assertBool,
                              "Identity function", "bool", "") );
    dat->add( "i", data( &ProgramTest::i,
                         "Return the current number" ) );
    return dat;
}

CommandFactoryInterface* ProgramTest::createCommandFactory()
{
    // Add the data of the EE:
    TemplateCommandFactory< ProgramTest >* dat =
        newCommandFactory( this );

    dat->add( "instantDone", command( &ProgramTest::true_genCom,
                                      &ProgramTest::true_gen,
                                      "returns immediately") );
    dat->add( "neverDone", command( &ProgramTest::true_genCom,
                                    &ProgramTest::false_gen,
                                    "returns never") );
    dat->add( "instantNotDone", command( &ProgramTest::true_genCom,
                                         &ProgramTest::true_gen,
                                         "returns never", false ) );
    dat->add( "instantFail", command( &ProgramTest::false_genCom,
                                      &ProgramTest::true_gen,
                                      "fails immediately") );
    dat->add( "totalFail", command( &ProgramTest::false_genCom,
                                    &ProgramTest::false_gen,
                                    "fails in command and condition") );
    return dat;
}


void ProgramTest::testParseProgram()
{
    // a program which should never fail
    string prog = string("program x { do test.instantDone() \n")
        + " and test.instantDone()"
        + " and test.instantDone()\n"
        + " do test.assert( test.isTrue( true ) )\n"
        + " do test.assert( test.i == 0 )\n"
        + " do test.increase()\n"
        + " do test.assert( test.i == 1 )\n"
        + " do test.reset()\n"
        + " do test.assert( test.i == 0 )\n"
        + " if test.increase() + test.increase() + test.increase() != 6  then \n "
        + "    do test.assert( false )\n"
        + " if true then\n"
        + "    return\n"
        + " do test.assert(false)\n"  // do not reach
        + "}";

    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
}

void ProgramTest::testProgramFailure()
{
    string prog = string("program x { do test.instantFail() until {  \n")
        + " if true then continue }\n"
        + "}";

    this->doProgram( prog, &gtc, false );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::error );

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
        + "if test.i != 51 then \n" // require same result as with ISO C
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
    string prog = string("program x { try test.instantFail()\n")
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.instantDone() \n"
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.totalFail() until { \n"
        + "  if true then continue }\n"
        + "try test.instantDone() \n"
        + "and test.instantFail() \n"
        + "and test.instantFail() catch { \n"
        + "  try test.instantDone() \n"
        + "  and test.instantDone() \n"
        + "  and test.instantDone() catch {\n"
        + "      do test.instantFail()\n"
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
    this->finishProgram( &gtc, "x");
}

void ProgramTest::testProgramUntil()
{
    // see if checking a remote condition works
    string prog = string("program x { do test.neverDone()\n")
        + "until { \n"
        + " if  time > 10 ms then continue \n" //  test in simulation takes far less than 1 second
        + "} \n"
        + "do test.neverDone()\n"
        + "until { \n"
        + " if done then continue \n" 
        + " if test.increase() == 10 then continue \n" // we continue after 10 checks
        + "} \n"
        + " }";
    this->doProgram( prog, &gtc );
    this->finishProgram( &gtc, "x");
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

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::error );

    this->finishProgram( &gtc, "x");
}

void ProgramTest::doProgram( const std::string& prog, TaskContext* tc, bool test )
{
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, tc );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE( exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    tc->getProcessor()->loadProgram( *pg_list.begin() );
    tc->getProcessor()->startProgram( (*pg_list.begin())->getName() );
    SimulationThread::Instance()->start();
    gtask.start();
//     while (1)
    sleep(1);
    gtask.stop();
    SimulationThread::Instance()->stop();

    if (test ) {
        stringstream errormsg;
        errormsg << " on line " << gprocessor.getProgram("x")->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( "Runtime error encountered" + errormsg.str(), gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
        CPPUNIT_ASSERT_MESSAGE( "Program stalled " + errormsg.str(), gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );
    }
}

void ProgramTest::finishProgram(TaskContext* tc, std::string prog_name)
{
    tc->getProcessor()->stopProgram( prog_name );
    tc->getProcessor()->deleteProgram( prog_name );
}

    

