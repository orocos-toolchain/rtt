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
 
 

#include "dispatch_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/ProgramGraph.hpp>
#include <corelib/SimulationThread.hpp>
#include <execution/TemplateFactories.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DispatchTest );

DispatchTest::DispatchTest()
    : gtask( 0.1, &gprocessor ), mtask(0.05, &mprocessor), ltask(0.01, &lprocessor),
      gtc("root",  &gprocessor),
      mtc("space", &mprocessor),
      ltc("subspace", &lprocessor)
{}


void 
DispatchTest::setUp()
{
    // ltc has a test object
    ltc.methodFactory.registerObject("test", this->createMethodFactory() );
    // mtc has two methods.
    mtc.methodFactory.registerObject("this", this->createMethodFactory() );
    gtc.addPeer( &mtc );
    mtc.connectPeers( &ltc );
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


MethodFactoryInterface* DispatchTest::createMethodFactory()
{
    // Add the data of the EE:
    TemplateMethodFactory< DispatchTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &DispatchTest::assertBool,
                              "Assert", "bool", "") );
    dat->add( "assertMsg", method( &DispatchTest::assertMsg,
                                 "Assert message", "bool", "", "text", "text" ) );
    return dat;
}


void DispatchTest::testParseDispatch()
{
    // this is a global program requesting a command on a local
    // task/processor (ie assert).
    string prog = string("program x { do space.subspace.test.assertMsg(true,\"tpdtrue\") \n")
        + " if space.subspace.test.assert(true) then \n"
        + "       do nothing\n"
//         + " do space.assertMsg(true,\"tpdfail\")\n"
//         + " do this.space.assertMsg(true,\"donotreach\")\n"
        + "}";

    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::running );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testDispatchFailure()
{
    // this is a global program requesting a command on a local
    // task/processor (ie assert).
    string prog = string("program x { do space.subspace.test.assert(false) \n")
        + "}";

    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::error );

    this->finishDispatch( &gtc, "x");
}
void DispatchTest::testDispatchCondition()
{
    // see if checking a remote condition works
    // also tests peerparser in expressions 
    string prog = string("program x { if ( space.subspace.test.assert(true) ) then \n")
        + "do space.subspace.test.assert(true) \n"
        + "else \n"
        + "do space.subspace.test.assert(false) \n"
        + "if ( space.subspace.test.assert(false) ) then \n"
        + "do space.subspace.test.assert(false) \n"
        + "else \n"
        + "do space.subspace.test.assert(true) \n"
        + " }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testDispatchAnd()
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.assert(true)\n")
        + "and space.subspace.test.assert(true) \n"
        + "and space.subspace.test.assert(true) \n"
        + " }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testDispatchTry()
{
    // see if checking a remote condition works
    string prog = string("program x { try space.subspace.test.assert(false)\n")
        + "try space.subspace.test.assert(true) \n"
        + "and space.subspace.test.assert(false) \n"
        + "and space.subspace.test.assert(true) \n"
        + " }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testDispatchUntil()
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.assert(true)\n")
        + "until { \n"
        + " if  time > 10 ms then continue \n" //  test in simulation takes far less than 1 second
        + "} \n"
        + "do space.subspace.test.assert(true)\n"
        + "until { \n"
        + " if done then continue \n" 
        + "} \n"
        + " }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testDispatchUntilFail()
{
    // see if checking a remote condition works
    string prog = string("program x { do space.subspace.test.assert(false)\n")
        + "until { \n"
        + " if done then continue \n" //  program should go into error
        + "} \n"
        + " }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::error );

    this->finishDispatch( &gtc, "x");
}

void DispatchTest::testSendDispatch()
{
    // a program which must not fail, even if the command failes.
    string prog = "program x { send space.subspace.test.assert(false) \n }";
    this->doDispatch( prog, &gtc );

    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") != Processor::ProgramStatus::error );
    CPPUNIT_ASSERT( gprocessor.getProgramStatus("x") == Processor::ProgramStatus::stopped );

    this->finishDispatch( &gtc, "x" );
}


void DispatchTest::doDispatch( const std::string& prog, TaskContext* tc )
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
    SimulationThread::Instance()->start();
    ltask.start();
    mtask.start();
    gtask.start();
    tc->getProcessor()->startProgram( (*pg_list.begin())->getName() );
    //     while (1)
    sleep(1);
    gtask.stop();
    mtask.stop();
    ltask.stop();
    SimulationThread::Instance()->stop();
}

void DispatchTest::finishDispatch(TaskContext* tc, std::string prog_name)
{
    tc->getProcessor()->stopProgram( prog_name );
    tc->getProcessor()->deleteProgram( prog_name );
}

    

