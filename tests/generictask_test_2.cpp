/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test_2.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "generictask_test_2.hpp"

#include <iostream>
#include <Command.hpp>
#include <Activity.hpp>
#include <scripting/CommandDS.hpp>
#include <interface/OperationInterface.hpp>
#include <internal/RemoteCommand.hpp>
#include <internal/TaskObject.hpp>

#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <internal/TaskObject.hpp>

#include <boost/function_types/function_type_signature.hpp>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

void
Generic_TaskTest_2::setUp()
{
    tc =  new TaskContext( "root" );
    tc->addObject( this->createCommandFactory() );
    tsim = new SimulationActivity(0.001);
    tc->setActivity( tsim );
    SimulationThread::Instance()->stop();
    cd0count = 0;
    cd1count = 0;
    cd2count = 0;
    cd3count = 0;
    cd4count = 0;
}


void
Generic_TaskTest_2::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tc->stop();
    SimulationThread::Instance()->stop();
    delete tc;
}

bool Generic_TaskTest_2::assertBool( bool b) {
    return b;
}


TaskObject* Generic_TaskTest_2::createCommandFactory()
{
    TaskObject* to = new TaskObject("commands");

    to->commands()->addCommand( command("c00", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, this, tc->engine()->commands()), "c0d");
    to->commands()->addCommand( command("c10", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn0, this, tc->engine()->commands()), "c1d","a","ad");
    to->commands()->addCommand( command("c11", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, this, tc->engine()->commands()), "c1d","a","ad");
    to->commands()->addCommand( command("c20", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn0, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c21", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn1, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c22", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn2, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c30", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn0, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c31", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn1, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c33", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn3, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c40", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn0, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c41", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn1, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c44", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn4, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    return to;
}


struct Sender: public TaskContext
{
    Command<bool(void)> com0;
    Command<bool(int)> com1;
    int com0count,com1count;
    TaskContext* mrecv;

    Sender(TaskContext* receiver)
        : TaskContext("Sender"),
          com0count(0),
          com1count(0)
    {
        mrecv = receiver;
        com0 = mrecv->getObject("commands")->commands()->getCommand<bool(void)>("c00");
        com1 = mrecv->getObject("commands")->commands()->getCommand<bool(int)>("c11");
        BOOST_CHECK(com0.ready());
        BOOST_CHECK(com1.ready());
    }

    void updateHook() {
        if (com0())
            com0count++;
        if (com1(1))
            com1count++;
    }
};

void Generic_TaskTest_2::verifydispatch(DispatchInterface& com)
{
    BOOST_CHECK( SimulationThread::Instance()->isRunning() == false );
    BOOST_CHECK( com.sent() );
    BOOST_CHECK( com.accepted() );
    BOOST_CHECK( !com.executed() );
    BOOST_CHECK( !com.valid() );
    BOOST_CHECK( !com.done() );
    BOOST_CHECK( SimulationThread::Instance()->run(1) );
    BOOST_CHECK( com.executed() );
    BOOST_CHECK( com.valid() );
    BOOST_CHECK( com.done() );
    com.reset();
    BOOST_CHECK( !com.sent() );
    BOOST_CHECK( !com.accepted() );
    BOOST_CHECK( !com.executed() );
    BOOST_CHECK( !com.valid() );
    BOOST_CHECK( !com.done() );
}

void Generic_TaskTest_2::verifycommand(CommandC& com)
{
    BOOST_CHECK( SimulationThread::Instance()->isRunning() == false );
    BOOST_CHECK( com.execute() );
    BOOST_CHECK( com.sent() );
    BOOST_CHECK( com.accepted() );
    BOOST_CHECK( !com.executed() );
    BOOST_CHECK( !com.valid() );
    BOOST_CHECK( !com.done() );
    BOOST_CHECK( SimulationThread::Instance()->run(1) );
    BOOST_CHECK( com.executed() );
    BOOST_CHECK( com.valid() );
    BOOST_CHECK( com.done() );
    com.reset();
    BOOST_CHECK( !com.sent() );
    BOOST_CHECK( !com.accepted() );
    BOOST_CHECK( !com.executed() );
    BOOST_CHECK( !com.valid() );
    BOOST_CHECK( !com.done() );
}


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  Generic_TaskTest2Suite,  Generic_TaskTest_2 )

BOOST_AUTO_TEST_CASE( testCommandThreading )
{
    // Sends periodic commands.
    Sender scomp ( tc ); // tc is receiver.

    tc->setActivity( new Activity(ORO_SCHED_OTHER, 0, 0.0) );
    scomp.setActivity( new PeriodicActivity(ORO_SCHED_OTHER, 0, 0.01) );

    BOOST_CHECK( tc->start() );
    BOOST_CHECK( scomp.start() );
    sleep(1);
    BOOST_CHECK( scomp.stop() );
    BOOST_CHECK( tc->stop() );

    BOOST_CHECK_EQUAL(scomp.com0count, cd0count);
    BOOST_CHECK_EQUAL(scomp.com1count, cd1count);
}

BOOST_AUTO_TEST_CASE( testCommandsC)
{
    CommandC cc = tc->getObject("commands")->commands()->create("c00");
    CommandC c20 = tc->getObject("commands")->commands()->create("c20").argC(1).argC(1.0);
    CommandC c32 = tc->getObject("commands")->commands()->create("c31").argC(1).argC(1.0).argC('a');
    CommandC c33 = tc->getObject("commands")->commands()->create("c33").argC(1).argC(1.0).argC('a');
    CommandC c44 = tc->getObject("commands")->commands()->create("c44").argC(1).argC(1.0).argC('a').argC(true);

    // CASE 1 : Send command to not running task.
    BOOST_CHECK( cc.ready() );
    BOOST_CHECK( c20.ready() );
    BOOST_CHECK( c32.ready() );
    BOOST_CHECK( c33.ready() );
    BOOST_CHECK( c44.ready() );
    BOOST_CHECK( !cc.sent() );
    BOOST_CHECK( !c20.sent() );
    BOOST_CHECK( !c32.sent() );
    BOOST_CHECK( !c33.sent() );
    BOOST_CHECK( !c44.sent() );
    BOOST_CHECK( !cc.executed() );
    BOOST_CHECK( !c20.executed() );
    BOOST_CHECK( !c32.executed() );
    BOOST_CHECK( !c33.executed() );
    BOOST_CHECK( !c44.executed() );
    BOOST_CHECK( !cc.accepted() );
    BOOST_CHECK( !c20.accepted() );
    BOOST_CHECK( !c32.accepted() );
    BOOST_CHECK( !c33.accepted() );
    BOOST_CHECK( !c44.accepted() );
    BOOST_CHECK( !cc.valid() );
    BOOST_CHECK( !c20.valid() );
    BOOST_CHECK( !c32.valid() );
    BOOST_CHECK( !c33.valid() );
    BOOST_CHECK( !c44.valid() );
    BOOST_CHECK( !cc.done() );
    BOOST_CHECK( !c20.done() );
    BOOST_CHECK( !c32.done() );
    BOOST_CHECK( !c33.done() );
    BOOST_CHECK( !c44.done() );
    BOOST_CHECK( !cc.execute() );
    BOOST_CHECK( !c20.execute() );
    BOOST_CHECK( !c32.execute() );
    BOOST_CHECK( !c33.execute() );
    BOOST_CHECK( !c44.execute() );
    BOOST_CHECK( cc.sent() );
    BOOST_CHECK( c20.sent() );
    BOOST_CHECK( c32.sent() );
    BOOST_CHECK( c33.sent() );
    BOOST_CHECK( c44.sent() );
    BOOST_CHECK( !cc.accepted() );
    BOOST_CHECK( !c20.accepted() );
    BOOST_CHECK( !c32.accepted() );
    BOOST_CHECK( !c33.accepted() );
    BOOST_CHECK( !c44.accepted() );
    BOOST_CHECK( !cc.executed() );
    BOOST_CHECK( !c20.executed() );
    BOOST_CHECK( !c32.executed() );
    BOOST_CHECK( !c33.executed() );
    BOOST_CHECK( !c44.executed() );
    BOOST_CHECK( !cc.valid() );
    BOOST_CHECK( !c20.valid() );
    BOOST_CHECK( !c32.valid() );
    BOOST_CHECK( !c33.valid() );
    BOOST_CHECK( !c44.valid() );
    BOOST_CHECK( !cc.done() );
    BOOST_CHECK( !c20.done() );
    BOOST_CHECK( !c32.done() );
    BOOST_CHECK( !c33.done() );
    BOOST_CHECK( !c44.done() );

    // Test Reset:
    cc.reset();
    c20.reset();
    c32.reset();
    c33.reset();
    c44.reset();
    BOOST_CHECK( cc.ready() );
    BOOST_CHECK( c20.ready() );
    BOOST_CHECK( c32.ready() );
    BOOST_CHECK( c33.ready() );
    BOOST_CHECK( c44.ready() );
    BOOST_CHECK( !cc.sent() );
    BOOST_CHECK( !c20.sent() );
    BOOST_CHECK( !c32.sent() );
    BOOST_CHECK( !c33.sent() );
    BOOST_CHECK( !c44.sent() );
    BOOST_CHECK( !cc.accepted() );
    BOOST_CHECK( !c20.accepted() );
    BOOST_CHECK( !c32.accepted() );
    BOOST_CHECK( !c33.accepted() );
    BOOST_CHECK( !c44.accepted() );
    BOOST_CHECK( !cc.executed() );
    BOOST_CHECK( !c20.executed() );
    BOOST_CHECK( !c32.executed() );
    BOOST_CHECK( !c33.executed() );
    BOOST_CHECK( !c44.executed() );
    BOOST_CHECK( !cc.valid() );
    BOOST_CHECK( !c20.valid() );
    BOOST_CHECK( !c32.valid() );
    BOOST_CHECK( !c33.valid() );
    BOOST_CHECK( !c44.valid() );
    BOOST_CHECK( !cc.done() );
    BOOST_CHECK( !c20.done() );
    BOOST_CHECK( !c32.done() );
    BOOST_CHECK( !c33.done() );
    BOOST_CHECK( !c44.done() );

    // CASE 2 send command to running task
    BOOST_CHECK( tsim->start() );

    // freezed sim thread
    BOOST_CHECK( cc.execute() );
    BOOST_CHECK( c20.execute() );
    BOOST_CHECK( c32.execute() );
    BOOST_CHECK( c33.execute() );
    BOOST_CHECK( c44.execute() );
    BOOST_CHECK( cc.sent() );
    BOOST_CHECK( c20.sent() );
    BOOST_CHECK( c32.sent() );
    BOOST_CHECK( c33.sent() );
    BOOST_CHECK( c44.sent() );
    BOOST_CHECK( cc.accepted() );
    BOOST_CHECK( c20.accepted() );
    BOOST_CHECK( c32.accepted() );
    BOOST_CHECK( c33.accepted() );
    BOOST_CHECK( c44.accepted() );
    BOOST_CHECK( !cc.executed() );
    BOOST_CHECK( !c20.executed() );
    BOOST_CHECK( !c32.executed() );
    BOOST_CHECK( !c33.executed() );
    BOOST_CHECK( !c44.executed() );
    BOOST_CHECK( !cc.valid() );
    BOOST_CHECK( !c20.valid() );
    BOOST_CHECK( !c32.valid() );
    BOOST_CHECK( !c33.valid() );
    BOOST_CHECK( !c44.valid() );
    BOOST_CHECK( !cc.done() );
    BOOST_CHECK( !c20.done() );
    BOOST_CHECK( !c32.done() );
    BOOST_CHECK( !c33.done() );
    BOOST_CHECK( !c44.done() );
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    BOOST_CHECK( cc.executed() );
    BOOST_CHECK( c20.executed() );
    BOOST_CHECK( c32.executed() );
    BOOST_CHECK( c33.executed() );
    BOOST_CHECK( c44.executed() );
    BOOST_CHECK( cc.valid() );
    BOOST_CHECK( c20.valid() );
    BOOST_CHECK( c32.valid() );
    BOOST_CHECK( c33.valid() );
    BOOST_CHECK( c44.valid() );
    BOOST_CHECK( cc.done() );
    BOOST_CHECK( c20.done() );
    BOOST_CHECK( c32.done() );
    BOOST_CHECK( c33.done() );
    BOOST_CHECK( c44.done() );
    tsim->stop();
#if 0
    string prog = string("program x { ")
        +" do commands.c00()\n"
        +" do commands.c10(1)\n"
        +" do commands.c11(1)\n"
        +" do commands.c20(1, 1.0)\n"
        +" do commands.c21(1, 1.0)\n"
        +" do commands.c22(1, 1.0)\n"
        +" do commands.c30(1, 1.0, 'a')\n"
        +" do commands.c31(1, 1.0, 'a')\n"
        +" do commands.c33(1, 1.0, 'a')\n"
        +" do commands.c44(1, 1.0, 'a', true)\n"
        +"}";
#endif
}

BOOST_AUTO_TEST_CASE( testRemoteCommand)
{
    Command<bool(void)> com0;
    Command<bool(int)> com11;

    com0 = new detail::RemoteCommand<bool(void)>(tc->getObject("commands")->commands(), "c00");
    com11 = new detail::RemoteCommand<bool(int)>(tc->getObject("commands")->commands(), "c11");

    BOOST_CHECK( com0.ready() );
    com0(); // execute

    BOOST_CHECK( com0.ready() );
    com11(1); // execute

}
BOOST_AUTO_TEST_CASE( testCommand)
{
    Command<bool(void)> com0("command", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("command", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("command", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );

    Command<bool(int,double)> com22("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    // start the activity, such that commands are accepted.
    BOOST_CHECK( tsim->start()) ;
    // execute commands and check status:
    BOOST_CHECK( com0() );
    verifydispatch(*com0.getCommandImpl());

    BOOST_CHECK( com11(1) );
    verifydispatch(*com11.getCommandImpl());
    BOOST_CHECK( com10(1) );
    verifydispatch(*com10.getCommandImpl());

    BOOST_CHECK( com22(1, 1.0) );
    verifydispatch(*com22.getCommandImpl());
    BOOST_CHECK( com20(1, 1.0) );
    verifydispatch(*com20.getCommandImpl());
    BOOST_CHECK( com21(1, 1.0) );
    verifydispatch(*com21.getCommandImpl());

    BOOST_CHECK( com33(1, 1.0, char('a') ) );
    verifydispatch(*com33.getCommandImpl());
    BOOST_CHECK( com30(1, 1.0, char('a') ) );
    verifydispatch(*com30.getCommandImpl());
    BOOST_CHECK( com31(1, 1.0, char('a') ) );
    verifydispatch(*com31.getCommandImpl());

    BOOST_CHECK( com44(1, 1.0, char('a'),true) );
    verifydispatch(*com44.getCommandImpl());
    BOOST_CHECK( com40(1, 1.0, char('a'),true) );
    verifydispatch(*com40.getCommandImpl());
    BOOST_CHECK( com41(1, 1.0, char('a'),true) );
    verifydispatch(*com41.getCommandImpl());

    BOOST_CHECK( tsim->stop() );
}

BOOST_AUTO_TEST_CASE( testCommandFactory)
{
    // Test the addition of 'simple' commands to the operation interface,
    // and retrieving it back in a new Command object.

    Command<bool(void)> com0("c0", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("c11", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("c10", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );

    TaskObject to("task");

    BOOST_CHECK( to.commands()->addCommand(&com0) );
    BOOST_CHECK( ! to.commands()->addCommand(&com0) );
    BOOST_CHECK( to.commands()->addCommand(&com11) );
    BOOST_CHECK( to.commands()->addCommand(&com10) );

    // test constructor
    Command<bool(void)> rc0 = to.commands()->getCommand<bool(void)>("c0");
    BOOST_CHECK( rc0.getCommandImpl() );
    BOOST_CHECK( rc0.ready() );

    // test operator=()
    Command<bool(int)> rc11;
    rc11 = to.commands()->getCommand<bool(int)>("c11");
    BOOST_CHECK( rc11.getCommandImpl() );
    BOOST_CHECK( rc11.ready() );

    Command<bool(int)> rc10 = to.commands()->getCommand<bool(int)>("c10");
    BOOST_CHECK( rc10.getCommandImpl() );
    BOOST_CHECK( rc10.ready() );

    // start the activity, such that commands are accepted.
    BOOST_CHECK( tsim->start()) ;
    // execute commands and check status:
    BOOST_CHECK( com0() );
    verifydispatch(*com0.getCommandImpl());

    BOOST_CHECK( com11(1) );
    verifydispatch(*com11.getCommandImpl());

    BOOST_CHECK( com10(1) );
    verifydispatch(*com10.getCommandImpl());

    // test error cases:
    // Add uninitialised command:
    Command<bool(void)> cvoid;
    BOOST_CHECK(to.commands()->addCommand( &cvoid ) == false);
    cvoid = Command<bool(void)>("voidc");
    BOOST_CHECK(to.commands()->addCommand( &cvoid ) == false);

    // wrong type:
    cvoid = to.commands()->getCommand<bool(bool)>("c0");
    BOOST_CHECK( cvoid.ready() == false );
    // wrong type 2:
    cvoid = to.commands()->getCommand<bool(int)>("c11");
    BOOST_CHECK( cvoid.ready() == false );
    // wrong type 3:
    cvoid = to.commands()->getCommand<bool(void)>("c11");
    BOOST_CHECK( cvoid.ready() == false );
    // not existing:
    cvoid = to.commands()->getCommand<bool(void)>("voidm");
    BOOST_CHECK( cvoid.ready() == false );

    cvoid.reset();
    BOOST_CHECK( cvoid() == false);
    BOOST_CHECK( cvoid.accepted() == false);
    BOOST_CHECK( cvoid.executed() == false);
    BOOST_CHECK( cvoid.sent() == false);
    BOOST_CHECK( cvoid.valid() == false);
    BOOST_CHECK( cvoid.done() == false);

}

BOOST_AUTO_TEST_CASE( testCommandFromDS)
{
    Command<bool(void)> com0("c0", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("c11", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("c10", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com22("c22", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20("c20", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21("c21", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33("c33", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30("c30", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31("c31", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44("c44", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40("c40", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41("c41", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    TaskObject to("task");

    BOOST_CHECK( to.commands()->addCommand( &com0, "desc" ) );
    BOOST_CHECK( ! to.commands()->addCommand( &com0, "desc" ) );

    BOOST_CHECK( to.commands()->addCommand( &com11, "desc","a1", "d1" ) );
    BOOST_CHECK( ! to.commands()->addCommand( &com11, "desc","a1", "d1" ) );
    BOOST_CHECK( to.commands()->addCommand( &com10, "desc","a1", "d1" ) );

    BOOST_CHECK( to.commands()->addCommand( &com22, "desc","a1", "d1","a2","d2" ) );
    BOOST_CHECK( ! to.commands()->addCommand( &com22, "desc","a1", "d1","a2","d2" ) );
    BOOST_CHECK( to.commands()->addCommand( &com20, "desc","a1", "d1","a2","d2" ) );
    BOOST_CHECK( to.commands()->addCommand( &com21, "desc","a1", "d1","a2","d2" ) );

    BOOST_CHECK( to.commands()->addCommand( &com33, "desc","a1", "d1","a2","d2","a3","d3" ) );
    BOOST_CHECK( ! to.commands()->addCommand( &com33, "desc","a1", "d1","a2","d2","a3","d3" ) );
    BOOST_CHECK( to.commands()->addCommand( &com30, "desc","a1", "d1","a2","d2","a3","d3" ) );
    BOOST_CHECK( to.commands()->addCommand( &com31, "desc","a1", "d1","a2","d2","a3","d3" ) );

    BOOST_CHECK( to.commands()->addCommand( &com44, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    BOOST_CHECK( ! to.commands()->addCommand( &com44, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    BOOST_CHECK( to.commands()->addCommand( &com40, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    BOOST_CHECK( to.commands()->addCommand( &com41, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );


    std::vector<DataSourceBase::shared_ptr> args;
    CommandC c0  = to.commands()->create("c0");
    CommandC c10 = to.commands()->create("c10").argC(1);
    CommandC c11 = to.commands()->create("c11").argC(1);
    CommandC c20 = to.commands()->create("c20").argC(1).argC(1.0);
    CommandC c21 = to.commands()->create("c21").argC(1).argC(1.0);
    CommandC c22 = to.commands()->create("c22").argC(1).argC(1.0);
    CommandC c30 = to.commands()->create("c30").argC(1).argC(1.0).argC('a');
    CommandC c31 = to.commands()->create("c31").argC(1).argC(1.0).argC('a');
    CommandC c33 = to.commands()->create("c33").argC(1).argC(1.0).argC('a');
    CommandC c40 = to.commands()->create("c40").argC(1).argC(1.0).argC('a').argC(true);
    CommandC c41 = to.commands()->create("c41").argC(1).argC(1.0).argC('a').argC(true);
    CommandC c44 = to.commands()->create("c44").argC(1).argC(1.0).argC('a').argC(true);

    BOOST_CHECK( tsim->start()) ;
    verifycommand(c0);
    verifycommand(c11);
    verifycommand(c10);
    verifycommand(c22);
    verifycommand(c20);
    verifycommand(c21);
    verifycommand(c33);
    verifycommand(c30);
    verifycommand(c31);
    verifycommand(c44);
    verifycommand(c40);
    verifycommand(c41);
    BOOST_CHECK( tsim->stop()) ;
}

BOOST_AUTO_TEST_CASE( testDSCommand)
{
    TaskObject to("task");

    // A command of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    CommandDS<bool(Generic_TaskTest_2*)> com0("c0",
                                            &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0,
                                            tc->engine()->commands() );

    command_ds("c0", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, tc->engine()->commands() );

    CommandDS<bool(Generic_TaskTest_2*,int)> com1("c1",
                                                &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1,
                                                tc->engine()->commands() );

    command_ds("c1",&Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, tc->engine()->commands() );

    command_ds("cs",&Generic_TaskTest_2::comstr, &Generic_TaskTest_2::comstr, tc->engine()->commands() );

    boost::shared_ptr<Generic_TaskTest_2> ptr( new Generic_TaskTest_2() );
    ValueDataSource<boost::weak_ptr<Generic_TaskTest_2> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<Generic_TaskTest_2> >( ptr );
    BOOST_CHECK( to.commands()->addCommandDS( wp.get(), com0, "desc" ) );
    BOOST_CHECK( to.commands()->addCommandDS( wp.get(), com1, "desc", "a1", "d1" ) );

    // this actually works ! the command will detect the deleted pointer.
    //ptr.reset();

    BOOST_CHECK( tsim->start()) ;

    CommandC c0  = to.commands()->create("c0");
    verifycommand(c0);
    CommandC c1  = to.commands()->create("c1").argC(1);
    verifycommand(c1);

    BOOST_CHECK( tsim->stop()) ;

}

BOOST_AUTO_TEST_CASE( testCRCommand)
{
    this->ret = -3.3;

    Command<bool(double&)> ic1r("c1r", &Generic_TaskTest_2::cn1r, &Generic_TaskTest_2::cd1r, this, tc->engine()->commands() );
    Command<bool(const double&)> ic1cr("c1cr", &Generic_TaskTest_2::cn1cr, &Generic_TaskTest_2::cd1cr, this, tc->engine()->commands() );

    Command<bool(double&)> c1r = ic1r.getCommandImpl()->clone();
    Command<bool(const double&)> c1cr = ic1cr.getCommandImpl()->clone();

    BOOST_CHECK( c1r.ready() );
    BOOST_CHECK( c1cr.ready() );

    BOOST_CHECK( tsim->start()) ;
    // execute commands and check status:
    BOOST_CHECK( c1cr(1.0) );
    verifydispatch(*c1cr.getCommandImpl());
    BOOST_CHECK_EQUAL( 1.0, ret );

    this->ret = -3.3;
    double result = 0.0;
    BOOST_CHECK( c1r(result) );
    verifydispatch(*c1r.getCommandImpl());
    // ret == -3.3, result == -6.6
    BOOST_CHECK_EQUAL( -3.3, ret );
    BOOST_CHECK_EQUAL( ret * 2, result );

    BOOST_CHECK( tsim->stop() ) ;
}

BOOST_AUTO_TEST_CASE( testCSCRCommand)
{
    // using a struct:
    CS cs;
    cs.y = 1.234;
    cs.z = 0.123;
    cs.x = cs.y + cs.z;

    CS cs2 = cs;

    Command<bool(CS&)> ic1r("c1r", &Generic_TaskTest_2::CScn1r, &Generic_TaskTest_2::CScd1r, this, tc->engine()->commands() );
    Command<bool(const CS&)> ic1cr("c1cr", &Generic_TaskTest_2::CScn1cr, &Generic_TaskTest_2::CScd1cr, this, tc->engine()->commands() );

    Command<bool(CS&)> c1r = ic1r.getCommandImpl()->clone();
    Command<bool(const CS&)> c1cr = ic1cr.getCommandImpl()->clone();
    BOOST_CHECK( c1r.ready() );
    BOOST_CHECK( c1cr.ready() );

    BOOST_CHECK( tsim->start()) ;
    // execute commands and check status:
    BOOST_CHECK( c1cr(cs) );
    verifydispatch(*c1cr.getCommandImpl());

    BOOST_CHECK( c1r(cs2) );
    verifydispatch(*c1r.getCommandImpl());
    BOOST_CHECK_CLOSE( 2*cs2.y+2*cs2.z, cs2.x, 0.0001 );

    BOOST_CHECK( tsim->stop() ) ;
}

BOOST_AUTO_TEST_CASE( testAddCommand)
{

    Command<bool(void)> com0("c0");
    com0 = command("command", &Generic_TaskTest_2::cd0, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );

    Command<bool(int)> com11("c11");
    com11= command("command", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int)> com10= command("command", &Generic_TaskTest_2::cd1, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );

    Command<bool(int,double)> com22= command("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20= command("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21= command("command", &Generic_TaskTest_2::cd2, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33= command("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30= command("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31= command("command", &Generic_TaskTest_2::cd3, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44= command("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40= command("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41= command("command", &Generic_TaskTest_2::cd4, &Generic_TaskTest_2::cn1, this, tc->engine()->commands() );

    // start the activity, such that commands are accepted.
    BOOST_CHECK( tsim->start()) ;

    // execute commands and check status:
    BOOST_CHECK( com0() );
    verifydispatch(*com0.getCommandImpl());

    BOOST_CHECK( com11(1) );
    verifydispatch(*com11.getCommandImpl());
    BOOST_CHECK( com10(1) );
    verifydispatch(*com10.getCommandImpl());

    BOOST_CHECK( com22(1, 1.0) );
    verifydispatch(*com22.getCommandImpl());
    BOOST_CHECK( com20(1, 1.0) );
    verifydispatch(*com20.getCommandImpl());
    BOOST_CHECK( com21(1, 1.0) );
    verifydispatch(*com21.getCommandImpl());

    BOOST_CHECK( com33(1, 1.0, char('a') ) );
    verifydispatch(*com33.getCommandImpl());
    BOOST_CHECK( com30(1, 1.0, char('a') ) );
    verifydispatch(*com30.getCommandImpl());
    BOOST_CHECK( com31(1, 1.0, char('a') ) );
    verifydispatch(*com31.getCommandImpl());

    BOOST_CHECK( com44(1, 1.0, char('a'),true) );
    verifydispatch(*com44.getCommandImpl());
    BOOST_CHECK( com40(1, 1.0, char('a'),true) );
    verifydispatch(*com40.getCommandImpl());
    BOOST_CHECK( com41(1, 1.0, char('a'),true) );
    verifydispatch(*com41.getCommandImpl());

    BOOST_CHECK( tsim->stop() );
}

BOOST_AUTO_TEST_SUITE_END()
