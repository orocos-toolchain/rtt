/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  dispatch_test.cpp

                        dispatch_test.cpp -  description
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

#include "tests/unit.hpp"

#include "tests/dispatch_test.hpp"

#include <iostream>
#include <sstream>
#include "rtt/scripting/FunctionGraph.hpp"
#include "rtt/extras/SimulationThread.hpp"
#include "rtt/OperationCaller.hpp"
#include "rtt/OperationCaller.hpp"
#include "rtt/Service.hpp"

using namespace std;

DispatchTest::DispatchTest()
    : gtc("root"),
      mtc("space"),
      ltc("subspace")
{
	setUp();
}


void
DispatchTest::setUp()
{
    ltc.clear();
    mtc.clear();
    gtc.clear();
    // ltc has a test object
    ltc.addService( this->createObject("test", ltc.engine()) );
    // mtc has two methods.
    mtc.addService( this->createObject("test", mtc.engine()) );

    gtc.addPeer( &mtc );
    mtc.connectPeers( &ltc );

    gtc.setActivity( new SimulationActivity(0.1) );
    mtc.setActivity( new SimulationActivity(0.05) );
    ltc.setActivity( new SimulationActivity(0.01) );
}


void
DispatchTest::tearDown()
{
    gtc.removePeer( "space" );
    ltc.disconnectPeers( "subspace" );
}


bool DispatchTest::assertBool( bool b) {
    return b;
}
bool DispatchTest::assertMsg( bool b, const std::string& msg) {
    if ( b == false )
        cout << "Asserted :" << b << " with :" << msg << endl;
    return b;
}


Service* DispatchTest::createObject(string a, CommandProcessor* cp)
{
    Service* dat = new Service(a);
    dat->addOperation("assert", &DispatchTest::assertBool, this).doc("Assert").arg("bool", "");
    dat->addOperation("assertMsg", &DispatchTest::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
    dat->addOperation("isTrue", &DispatchTest::assertBool, this).doc("Identity function").arg("bool", "");
    dat->addOperation("instantDone", &DispatchTest::true_genCom, this).doc("returns immediately");
addOperation("instantDoneDone", &DispatchTest::true_gen, this).doc("Returns true when instantDone is done.");
    dat->addOperation("neverDone", &DispatchTest::true_genCom, this).doc("returns never");
addOperation("neverDoneDone", &DispatchTest::false_gen, this).doc("Returns true when neverDone is done.");
    dat->addCommand( command( "instantNotDone", &DispatchTest::true_genCom,
                                         &DispatchTest::true_gen, this, cp, false),
                                         "returns never");
    dat->addOperation("instantFail", &DispatchTest::false_genCom, this).doc("fails immediately");
addOperation("instantFailDone", &DispatchTest::true_gen, this).doc("Returns true when instantFail is done.");
    dat->addOperation("totalFail", &DispatchTest::false_genCom, this).doc("fails in command and condition");
addOperation("totalFailDone", &DispatchTest::false_gen, this).doc("Returns true when totalFail is done.");
    return dat;
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  DispatchTestSuite,  DispatchTest )

BOOST_AUTO_TEST_CASE( testParseDispatch)
{
    // this is a global program requesting a method on a local
    // task/processor (ie assert) and a command (instantDone)
    string prog = string("program x { do space.subspace.test.assertMsg(true,\"tpdtrue\") \n")
        + " if space.subspace.test.assert(true) then \n"
        + "       do nothing\n"
        + " do space.subspace.test.instantDone() \n"
//         + " do space.assertMsg(true,\"tpdfail\")\n"
//         + " do this.space.assertMsg(true,\"donotreach\")\n"
        + "}";

    this->doDispatch( prog, &gtc );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") != ProgramInterface::Status::error );
    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") != ProgramInterface::Status::running );
    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped );

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchFailure)
{
    // this is a global program requesting a command on a local
    // task/processor (ie instantFail).
    string prog = string("program x { do space.subspace.test.instantFail() \n")
        + "}";

    this->doDispatch( prog, &gtc );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishDispatch( &gtc, "x");
}
BOOST_AUTO_TEST_CASE( testDispatchCondition)
{
    // see if checking a remote condition works
    // also tests peerparser in expressions
    string prog = string("program x { if ( space.subspace.test.assert(true) ) then \n")
        + "do space.subspace.test.instantDone() \n"
        + "else \n"
        + "do space.subspace.test.instantFail() \n"
        + "if ( space.subspace.test.assert(false) ) then \n"
        + "do space.subspace.test.instantFail() \n"
        + "else \n"
        + "do space.subspace.test.instantDone() \n"
        + " }";
    this->doDispatch( prog, &gtc );

    stringstream msg;
    msg <<  "Status was not 'stopped', but "+gtc.engine()->programs()->getProgramStatusStr("x");
    msg << " on line " << gtc.engine()->programs()->getProgram("x")->getLineNumber();
    BOOST_CHECK_MESSAGE(gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped,
						 msg.str());

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchAnd)
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.assert(true)\n")
        + "and space.subspace.test.assert(true) \n"
        + "and space.subspace.test.assert(true) \n"
        + "do space.subspace.test.instantDone() \n"
        + "and space.subspace.test.instantDone() \n"
        + "and space.subspace.test.instantDone() \n"
        + " }";
    this->doDispatch( prog, &gtc );

    stringstream msg;
    msg <<  "Status was not 'stopped', but "+gtc.engine()->programs()->getProgramStatusStr("x");
    msg << " on line " << gtc.engine()->programs()->getProgram("x")->getLineNumber();
    BOOST_CHECK_MESSAGE(gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped
						, msg.str());

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchTry)
{
    // see if checking a remote condition works
    string prog = string("program x { try space.subspace.test.assert(false)\n")
        + "try space.subspace.test.assert(true) \n"
        + "and space.subspace.test.assert(false) \n"
        + "and space.subspace.test.assert(true) \n"
        + "try space.subspace.test.instantFail()\n"
        + "try space.subspace.test.instantDone() \n"
        + "and space.subspace.test.instantFail() \n"
        + "and space.subspace.test.instantDone() \n"
        + " }";
    this->doDispatch( prog, &gtc );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") != ProgramInterface::Status::error );
    stringstream msg;
    msg <<  "Status was not 'stopped', but "+gtc.engine()->programs()->getProgramStatusStr("x");
    msg << " on line " << gtc.engine()->programs()->getProgram("x")->getLineNumber();
    BOOST_CHECK_MESSAGE(gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped
    		, msg.str());

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchUntil)
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.instantDone()\n")
        + "until { \n"
        + " if time > 10 ms then continue \n" //  test in simulation takes far less than 1 second
        + "} \n"
        + "do space.subspace.test.instantDone()\n"
        + "until { \n"
        + " if done then continue \n"
        + "} \n"
        + " }";
    this->doDispatch( prog, &gtc );

    stringstream msg;
    msg <<  "Status was not 'stopped', but "+gtc.engine()->programs()->getProgramStatusStr("x");
    msg << " on line " << gtc.engine()->programs()->getProgram("x")->getLineNumber();
    BOOST_CHECK_MESSAGE(gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped,
    		msg.str());

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchUntilFail)
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.instantFail()\n")
        + "until { \n"
        + " if done then continue \n" //  program should go into error
        + "} \n"
        + " }";
    this->doDispatch( prog, &gtc );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::error );

    this->finishDispatch( &gtc, "x");
}

BOOST_AUTO_TEST_CASE( testDispatchMany)
{
    // XXX not a valid test. send not present in Orocos, this looks like 'try'
    // a program which must not fail, even if the command failes.
    string prog = string("program x { ")
        +"  do space.subspace.test.instantDone()\n"
        +"  do space.subspace.test.instantDone()\n"
        +"  do space.subspace.test.instantDone()\n"
        +"  do space.subspace.test.instantDone()\n"
        +" }";
    this->doDispatch( prog, &gtc );

    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") != ProgramInterface::Status::error );
    BOOST_CHECK( gtc.engine()->programs()->getProgramStatus("x") == ProgramInterface::Status::stopped );

    this->finishDispatch( &gtc, "x" );
}

BOOST_AUTO_TEST_SUITE_END()

void DispatchTest::doDispatch( const std::string& prog, TaskContext* tc )
{
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_REQUIRE( false );
        }
    BOOST_REQUIRE( tc->engine()->programs()->loadProgram( *pg_list.begin() ) );
    BOOST_CHECK(ltc.start());
    BOOST_CHECK(mtc.start());
    BOOST_CHECK(gtc.start());
    BOOST_REQUIRE( tc->engine()->programs()->getProgram( (*pg_list.begin())->getName() )->start() );

    SimulationThread::Instance()->stop();
    SimulationThread::Instance()->run(1000);
}

void DispatchTest::finishDispatch(TaskContext* tc, std::string prog_name)
{
    BOOST_CHECK(gtc.stop());
    BOOST_CHECK(mtc.stop());
    BOOST_CHECK(ltc.stop());
    tc->engine()->programs()->getProgram( prog_name )->stop();
    BOOST_CHECK( tc->engine()->programs()->unloadProgram( prog_name ) );

}

