/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp 

                        generictask_test.cpp -  description
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
 
 

#include "generictask_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/FunctionGraph.hpp>
#include <execution/TemplateFactories.hpp>
#include <execution/Ports.hpp>
#include <execution/Command.hpp>
#include <execution/CommandDS.hpp>
#include <execution/Method.hpp>
#include <execution/OperationInterface.hpp>

#include <corelib/SimulationActivity.hpp>
#include <corelib/SimulationThread.hpp>

#include <boost/function_types/function_type_signature.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
using namespace ORO_Geometry;
#endif

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( Generic_TaskTest );


void 
Generic_TaskTest::setUp()
{
    tc =  new TaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
    tc->addObject( this->createCommandFactory() );
    tsim = new SimulationActivity(0.001, tc->engine() );
}


void 
Generic_TaskTest::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tsim->stop();
    SimulationThread::Instance()->stop();
    delete tc;
    delete tsim;
}

bool Generic_TaskTest::assertBool( bool b) { 
    return b;
}

TaskObject* Generic_TaskTest::createMethodFactory()
{
    TaskObject* to = new TaskObject("methods");

    to->methods()->addMethod( method("assert",  &Generic_TaskTest::assertBool, this), "assert","b","bd");

    to->methods()->addMethod( method("m0",  &Generic_TaskTest::m0, this), "M0");
    to->methods()->addMethod( method("m1",  &Generic_TaskTest::m1, this), "M1","a","ad");
    to->methods()->addMethod( method("m2",  &Generic_TaskTest::m2, this), "M2","a","ad","a","ad");
    to->methods()->addMethod( method("m3",  &Generic_TaskTest::m3, this), "M3","a","ad","a","ad","a","ad");
    to->methods()->addMethod( method("m4",  &Generic_TaskTest::m4, this), "M4","a","ad","a","ad","a","ad","a","ad");
    return to;
}

TaskObject* Generic_TaskTest::createCommandFactory()
{
    TaskObject* to = new TaskObject("commands");

    to->commands()->addCommand( command("c00", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, this, tc->engine()->commands()), "c0d");
    to->commands()->addCommand( command("c10", &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, this, tc->engine()->commands()), "c1d","a","ad");
    to->commands()->addCommand( command("c11", &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, this, tc->engine()->commands()), "c1d","a","ad");
    to->commands()->addCommand( command("c20", &Generic_TaskTest::cd2, &Generic_TaskTest::cn0, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c21", &Generic_TaskTest::cd2, &Generic_TaskTest::cn1, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c22", &Generic_TaskTest::cd2, &Generic_TaskTest::cn2, this, tc->engine()->commands()), "c2d","a","ad","a","ad");
    to->commands()->addCommand( command("c30", &Generic_TaskTest::cd3, &Generic_TaskTest::cn0, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c31", &Generic_TaskTest::cd3, &Generic_TaskTest::cn1, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c33", &Generic_TaskTest::cd3, &Generic_TaskTest::cn3, this, tc->engine()->commands()), "c3d","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c40", &Generic_TaskTest::cd4, &Generic_TaskTest::cn0, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c41", &Generic_TaskTest::cd4, &Generic_TaskTest::cn1, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    to->commands()->addCommand( command("c44", &Generic_TaskTest::cd4, &Generic_TaskTest::cn4, this, tc->engine()->commands()), "c4d","a","ad","a","ad","a","ad","a","ad");
    return to;
}

void Generic_TaskTest::testMethodsC()
{
    MethodC mc;
    double r = 0.0;
    mc = tc->getObject("methods")->methods()->create("m0").ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -1.0 );

    mc = tc->getObject("methods")->methods()->create("m2").argC(1).argC(1.0).ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -3.0 );

    mc = tc->getObject("methods")->methods()->create("m3").ret( r ).argC(1).argC(1.0).argC(true);
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -4.0 );
    
#if 0
        +" set r = methods.m0()\n"
        +" do methods.assert( r == -1.0 )\n"
        +" set r = methods.m1(1)\n"
        +" do methods.assert( r == -2.0 )\n"
        +" set r = methods.m2(1, 1.0)\n"
        +" do methods.assert( r == -3.0 )\n"
        +" set r = methods.m3(1, 1.0, true)\n"
        +" do methods.assert( r == -4.0 )\n"
        +" set r = methods.m4(1, 1.0, true, \"hello\")\n"
        +" do methods.assert( r == -5.0 )\n"
#endif
}

void Generic_TaskTest::testCommandsC()
{
    CommandC cc = tc->getObject("commands")->commands()->create("c00");
    CommandC c20 = tc->getObject("commands")->commands()->create("c20").argC(1).argC(1.0);
    CommandC c32 = tc->getObject("commands")->commands()->create("c31").argC(1).argC(1.0).argC('a');
    CommandC c33 = tc->getObject("commands")->commands()->create("c33").argC(1).argC(1.0).argC('a');
    CommandC c44 = tc->getObject("commands")->commands()->create("c44").argC(1).argC(1.0).argC('a').argC(true);
    SimulationThread::Instance()->stop();
    tsim->stop();

    // CASE 1 : Send command to not running task.
    CPPUNIT_ASSERT( cc.ready() );
    CPPUNIT_ASSERT( c20.ready() );
    CPPUNIT_ASSERT( c32.ready() );
    CPPUNIT_ASSERT( c33.ready() );
    CPPUNIT_ASSERT( c44.ready() );
    CPPUNIT_ASSERT( !cc.sent() );
    CPPUNIT_ASSERT( !c20.sent() );
    CPPUNIT_ASSERT( !c32.sent() );
    CPPUNIT_ASSERT( !c33.sent() );
    CPPUNIT_ASSERT( !c44.sent() );
    CPPUNIT_ASSERT( !cc.executed() );
    CPPUNIT_ASSERT( !c20.executed() );
    CPPUNIT_ASSERT( !c32.executed() );
    CPPUNIT_ASSERT( !c33.executed() );
    CPPUNIT_ASSERT( !c44.executed() );
    CPPUNIT_ASSERT( !cc.accepted() );
    CPPUNIT_ASSERT( !c20.accepted() );
    CPPUNIT_ASSERT( !c32.accepted() );
    CPPUNIT_ASSERT( !c33.accepted() );
    CPPUNIT_ASSERT( !c44.accepted() );
    CPPUNIT_ASSERT( !cc.valid() );
    CPPUNIT_ASSERT( !c20.valid() );
    CPPUNIT_ASSERT( !c32.valid() );
    CPPUNIT_ASSERT( !c33.valid() );
    CPPUNIT_ASSERT( !c44.valid() );
    CPPUNIT_ASSERT( !cc.evaluate() );
    CPPUNIT_ASSERT( !c20.evaluate() );
    CPPUNIT_ASSERT( !c32.evaluate() );
    CPPUNIT_ASSERT( !c33.evaluate() );
    CPPUNIT_ASSERT( !c44.evaluate() );
    CPPUNIT_ASSERT( !cc.execute() );
    CPPUNIT_ASSERT( !c20.execute() );
    CPPUNIT_ASSERT( !c32.execute() );
    CPPUNIT_ASSERT( !c33.execute() );
    CPPUNIT_ASSERT( !c44.execute() );
    CPPUNIT_ASSERT( cc.sent() );
    CPPUNIT_ASSERT( c20.sent() );
    CPPUNIT_ASSERT( c32.sent() );
    CPPUNIT_ASSERT( c33.sent() );
    CPPUNIT_ASSERT( c44.sent() );
    CPPUNIT_ASSERT( !cc.accepted() );
    CPPUNIT_ASSERT( !c20.accepted() );
    CPPUNIT_ASSERT( !c32.accepted() );
    CPPUNIT_ASSERT( !c33.accepted() );
    CPPUNIT_ASSERT( !c44.accepted() );
    CPPUNIT_ASSERT( !cc.executed() );
    CPPUNIT_ASSERT( !c20.executed() );
    CPPUNIT_ASSERT( !c32.executed() );
    CPPUNIT_ASSERT( !c33.executed() );
    CPPUNIT_ASSERT( !c44.executed() );
    CPPUNIT_ASSERT( !cc.valid() );
    CPPUNIT_ASSERT( !c20.valid() );
    CPPUNIT_ASSERT( !c32.valid() );
    CPPUNIT_ASSERT( !c33.valid() );
    CPPUNIT_ASSERT( !c44.valid() );
    CPPUNIT_ASSERT( !cc.evaluate() );
    CPPUNIT_ASSERT( !c20.evaluate() );
    CPPUNIT_ASSERT( !c32.evaluate() );
    CPPUNIT_ASSERT( !c33.evaluate() );
    CPPUNIT_ASSERT( !c44.evaluate() );

    // Test Reset:
    cc.reset();
    c20.reset();
    c32.reset();
    c33.reset();
    c44.reset();
    CPPUNIT_ASSERT( cc.ready() );
    CPPUNIT_ASSERT( c20.ready() );
    CPPUNIT_ASSERT( c32.ready() );
    CPPUNIT_ASSERT( c33.ready() );
    CPPUNIT_ASSERT( c44.ready() );
    CPPUNIT_ASSERT( !cc.sent() );
    CPPUNIT_ASSERT( !c20.sent() );
    CPPUNIT_ASSERT( !c32.sent() );
    CPPUNIT_ASSERT( !c33.sent() );
    CPPUNIT_ASSERT( !c44.sent() );
    CPPUNIT_ASSERT( !cc.accepted() );
    CPPUNIT_ASSERT( !c20.accepted() );
    CPPUNIT_ASSERT( !c32.accepted() );
    CPPUNIT_ASSERT( !c33.accepted() );
    CPPUNIT_ASSERT( !c44.accepted() );
    CPPUNIT_ASSERT( !cc.executed() );
    CPPUNIT_ASSERT( !c20.executed() );
    CPPUNIT_ASSERT( !c32.executed() );
    CPPUNIT_ASSERT( !c33.executed() );
    CPPUNIT_ASSERT( !c44.executed() );
    CPPUNIT_ASSERT( !cc.valid() );
    CPPUNIT_ASSERT( !c20.valid() );
    CPPUNIT_ASSERT( !c32.valid() );
    CPPUNIT_ASSERT( !c33.valid() );
    CPPUNIT_ASSERT( !c44.valid() );
    CPPUNIT_ASSERT( !cc.evaluate() );
    CPPUNIT_ASSERT( !c20.evaluate() );
    CPPUNIT_ASSERT( !c32.evaluate() );
    CPPUNIT_ASSERT( !c33.evaluate() );
    CPPUNIT_ASSERT( !c44.evaluate() );

    // CASE 2 send command to running task
    CPPUNIT_ASSERT( tsim->start() );

    // freezed sim thread
    CPPUNIT_ASSERT( cc.execute() );
    CPPUNIT_ASSERT( c20.execute() );
    CPPUNIT_ASSERT( c32.execute() );
    CPPUNIT_ASSERT( c33.execute() );
    CPPUNIT_ASSERT( c44.execute() );
    CPPUNIT_ASSERT( cc.sent() );
    CPPUNIT_ASSERT( c20.sent() );
    CPPUNIT_ASSERT( c32.sent() );
    CPPUNIT_ASSERT( c33.sent() );
    CPPUNIT_ASSERT( c44.sent() );
    CPPUNIT_ASSERT( cc.accepted() );
    CPPUNIT_ASSERT( c20.accepted() );
    CPPUNIT_ASSERT( c32.accepted() );
    CPPUNIT_ASSERT( c33.accepted() );
    CPPUNIT_ASSERT( c44.accepted() );
    CPPUNIT_ASSERT( !cc.executed() );
    CPPUNIT_ASSERT( !c20.executed() );
    CPPUNIT_ASSERT( !c32.executed() );
    CPPUNIT_ASSERT( !c33.executed() );
    CPPUNIT_ASSERT( !c44.executed() );
    CPPUNIT_ASSERT( !cc.valid() );
    CPPUNIT_ASSERT( !c20.valid() );
    CPPUNIT_ASSERT( !c32.valid() );
    CPPUNIT_ASSERT( !c33.valid() );
    CPPUNIT_ASSERT( !c44.valid() );
    CPPUNIT_ASSERT( !cc.evaluate() );
    CPPUNIT_ASSERT( !c20.evaluate() );
    CPPUNIT_ASSERT( !c32.evaluate() );
    CPPUNIT_ASSERT( !c33.evaluate() );
    CPPUNIT_ASSERT( !c44.evaluate() );
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    tc->engine()->step();
    CPPUNIT_ASSERT( cc.executed() );
    CPPUNIT_ASSERT( c20.executed() );
    CPPUNIT_ASSERT( c32.executed() );
    CPPUNIT_ASSERT( c33.executed() );
    CPPUNIT_ASSERT( c44.executed() );
    CPPUNIT_ASSERT( cc.valid() );
    CPPUNIT_ASSERT( c20.valid() );
    CPPUNIT_ASSERT( c32.valid() );
    CPPUNIT_ASSERT( c33.valid() );
    CPPUNIT_ASSERT( c44.valid() );
    CPPUNIT_ASSERT( cc.evaluate() );
    CPPUNIT_ASSERT( c20.evaluate() );
    CPPUNIT_ASSERT( c32.evaluate() );
    CPPUNIT_ASSERT( c33.evaluate() );
    CPPUNIT_ASSERT( c44.evaluate() );
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

void Generic_TaskTest::testProperties()
{
    Property<double> d1("d1", "desc1", 1.234);
    tc->properties()->addProperty( &d1);
#ifdef OROPKG_GEOMETRY
    using namespace ORO_Geometry;
    Property<Frame> f1("f1", "descf1", Frame::Identity());
    tc->properties()->addProperty( &f1 );
#endif

    CPPUNIT_ASSERT_EQUAL( double(1.234), d1.get() );
    CPPUNIT_ASSERT_EQUAL( double(1.234), tc->properties()->getProperty<double>("d1")->get() );

#if 0    
    CPPUNIT_ASSERT(tc->writeProperties("Generic_TaskTest_Properties.cpf"));
    CPPUNIT_ASSERT( tc->readProperties("Generic_TaskTest_Properties.cpf"));
#endif
}

void Generic_TaskTest::testAttributes()
{
    Attribute<int> i1;
    Attribute<double> d1( 1.234);
    CPPUNIT_ASSERT(tc->attributes()->addAttribute("d1", &d1 ));
    CPPUNIT_ASSERT(tc->attributes()->addAttribute("i1", &i1 ));

    i1.set( 3 );
    CPPUNIT_ASSERT_EQUAL( double(1.234), d1.get() );
    CPPUNIT_ASSERT_EQUAL( int(3), i1.get() );
    
    CPPUNIT_ASSERT_EQUAL( double(1.234), tc->attributes()->getAttribute<double>("d1")->get() );
    CPPUNIT_ASSERT_EQUAL( int(3),        tc->attributes()->getAttribute<int>("i1")->get() );

#ifdef OROPKG_GEOMETRY
    using namespace ORO_Geometry;
    Attribute<Frame> f1(Frame::Identity());
    tc->attributes()->addAttribute("f1", &f1 );
#endif
}

void Generic_TaskTest::verifydispatch(DispatchInterface& com)
{
    CPPUNIT_ASSERT( com.sent() );
    CPPUNIT_ASSERT( com.accepted() );
    CPPUNIT_ASSERT( !com.executed() );
    CPPUNIT_ASSERT( !com.valid() );
    CPPUNIT_ASSERT( !com.evaluate() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1) );
    CPPUNIT_ASSERT( com.executed() );
    CPPUNIT_ASSERT( com.valid() );
    CPPUNIT_ASSERT( com.evaluate() );
    com.reset();
    CPPUNIT_ASSERT( !com.sent() );
    CPPUNIT_ASSERT( !com.accepted() );
    CPPUNIT_ASSERT( !com.executed() );
    CPPUNIT_ASSERT( !com.valid() );
    CPPUNIT_ASSERT( !com.evaluate() );
}

void Generic_TaskTest::verifycommand(CommandC& com)
{
    CPPUNIT_ASSERT( com.execute() );
    CPPUNIT_ASSERT( com.sent() );
    CPPUNIT_ASSERT( com.accepted() );
    CPPUNIT_ASSERT( !com.executed() );
    CPPUNIT_ASSERT( !com.valid() );
    CPPUNIT_ASSERT( !com.evaluate() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1) );
    CPPUNIT_ASSERT( com.executed() );
    CPPUNIT_ASSERT( com.valid() );
    CPPUNIT_ASSERT( com.evaluate() );
    com.reset();
    CPPUNIT_ASSERT( !com.sent() );
    CPPUNIT_ASSERT( !com.accepted() );
    CPPUNIT_ASSERT( !com.executed() );
    CPPUNIT_ASSERT( !com.valid() );
    CPPUNIT_ASSERT( !com.evaluate() );
}

void Generic_TaskTest::testCommand()
{
    Command<bool(void)> com0("command", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("command", &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("command", &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, this, tc->engine()->commands() );

    Command<bool(int,double)> com22("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    // start the activity, such that commands are accepted.
    CPPUNIT_ASSERT( tsim->start()) ;
    // execute commands and check status:
    CPPUNIT_ASSERT( com0() );
    
    CPPUNIT_ASSERT( com11(1) );
    CPPUNIT_ASSERT( com10(1) );

    CPPUNIT_ASSERT( com22(1, 1.0) );
    CPPUNIT_ASSERT( com20(1, 1.0) );
    CPPUNIT_ASSERT( com21(1, 1.0) );

    CPPUNIT_ASSERT( com33(1, 1.0, char('a') ) );
    CPPUNIT_ASSERT( com30(1, 1.0, char('a') ) );
    CPPUNIT_ASSERT( com31(1, 1.0, char('a') ) );

    CPPUNIT_ASSERT( com44(1, 1.0, char('a'),true) );
    CPPUNIT_ASSERT( com40(1, 1.0, char('a'),true) );
    CPPUNIT_ASSERT( com41(1, 1.0, char('a'),true) );

    verifydispatch(*com0.getCommandImpl());
    verifydispatch(*com11.getCommandImpl());
    verifydispatch(*com10.getCommandImpl());
    verifydispatch(*com22.getCommandImpl());
    verifydispatch(*com20.getCommandImpl());
    verifydispatch(*com21.getCommandImpl());
    verifydispatch(*com33.getCommandImpl());
    verifydispatch(*com30.getCommandImpl());
    verifydispatch(*com31.getCommandImpl());
    verifydispatch(*com44.getCommandImpl());
    verifydispatch(*com40.getCommandImpl());
    verifydispatch(*com41.getCommandImpl());

    CPPUNIT_ASSERT( tsim->stop() );
}

void Generic_TaskTest::testPorts()
{
    WriteDataPort<double> wdp("WDName");
    ReadDataPort<double> rdp("RDName");

    // Test initial value
    wdp.Set( 1.0 );
    CPPUNIT_ASSERT( wdp.Get() == 1.0 );

    WriteBufferPort<double> wbp("WBName", 10);
    ReadBufferPort<double> rbp("RBName");
    BufferPort<double> bp("BName", 10);

    tc->ports()->addPort( &wdp );
    tc->ports()->addPort( &rdp );
    tc->ports()->addPort( &wbp );
    tc->ports()->addPort( &rbp );
    tc->ports()->addPort( &bp );

    // Test connection creation.
    wdp.createConnection( &rdp )->connect();
    wbp.createConnection( &rbp )->connect();
    bp.connectTo( rbp.connection() );

    CPPUNIT_ASSERT( wdp.connected() );
    CPPUNIT_ASSERT( rdp.connected() );
    CPPUNIT_ASSERT( wbp.connected() );
    CPPUNIT_ASSERT( rbp.connected() );
    CPPUNIT_ASSERT( bp.connected() );

    // Test data transfer
    CPPUNIT_ASSERT( rdp.Get() == 1.0 );
    wdp.Set( 2.0 );
    CPPUNIT_ASSERT( rdp.Get() == 2.0 );

    double val;
    CPPUNIT_ASSERT( wbp.Push( 5.0 ) );
    CPPUNIT_ASSERT( rbp.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );

    CPPUNIT_ASSERT( wbp.Push( 6.0 ) );
    CPPUNIT_ASSERT( bp.Pop( val ) );
    CPPUNIT_ASSERT( val == 6.0 );

    CPPUNIT_ASSERT( bp.Push( 5.0 ) );
    CPPUNIT_ASSERT( bp.Pop( val ) );
    CPPUNIT_ASSERT( val == 5.0 );
    CPPUNIT_ASSERT( bp.Pop( val ) == false );
}

void Generic_TaskTest::testCommandFactory()
{
    // Test the addition of 'simple' commands to the operation interface,
    // and retrieving it back in a new Command object.

    Command<bool(void)> com0("c0", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("c11", &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("c10", &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, this, tc->engine()->commands() );

    TaskObject to("task");

    CPPUNIT_ASSERT( to.commands()->addCommand(&com0) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand(&com0) );
    CPPUNIT_ASSERT( to.commands()->addCommand(&com11) );
    CPPUNIT_ASSERT( to.commands()->addCommand(&com10) );

    // test constructor
    Command<bool(void)> rc0 = to.commands()->getCommand<bool(void)>("c0");
    CPPUNIT_ASSERT( rc0.getCommandImpl() );
    CPPUNIT_ASSERT( rc0.ready() );

    // test operator=()
    Command<bool(int)> rc11;
    rc11 = to.commands()->getCommand<bool(int)>("c11");
    CPPUNIT_ASSERT( rc11.getCommandImpl() );
    CPPUNIT_ASSERT( rc11.ready() );

    Command<bool(int)> rc10 = to.commands()->getCommand<bool(int)>("c10");
    CPPUNIT_ASSERT( rc10.getCommandImpl() );
    CPPUNIT_ASSERT( rc10.ready() );

    // start the activity, such that commands are accepted.
    CPPUNIT_ASSERT( tsim->start()) ;
    // execute commands and check status:
    CPPUNIT_ASSERT( com0() );
    
    CPPUNIT_ASSERT( com11(1) );
    CPPUNIT_ASSERT( com10(1) );

    verifydispatch(*com0.getCommandImpl());
    verifydispatch(*com11.getCommandImpl());
    verifydispatch(*com10.getCommandImpl());
}

void Generic_TaskTest::testCommandFromDS()
{
    Command<bool(void)> com0("c0", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11("c11", &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10("c10", &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com22("c22", &Generic_TaskTest::cd2, &Generic_TaskTest::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20("c20", &Generic_TaskTest::cd2, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21("c21", &Generic_TaskTest::cd2, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33("c33", &Generic_TaskTest::cd3, &Generic_TaskTest::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30("c30", &Generic_TaskTest::cd3, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31("c31", &Generic_TaskTest::cd3, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44("c44", &Generic_TaskTest::cd4, &Generic_TaskTest::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40("c40", &Generic_TaskTest::cd4, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41("c41", &Generic_TaskTest::cd4, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    TaskObject to("task");

    CPPUNIT_ASSERT( to.commands()->addCommand( &com0, "desc" ) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand( &com0, "desc" ) );

    CPPUNIT_ASSERT( to.commands()->addCommand( &com11, "desc","a1", "d1" ) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand( &com11, "desc","a1", "d1" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com10, "desc","a1", "d1" ) );

    CPPUNIT_ASSERT( to.commands()->addCommand( &com22, "desc","a1", "d1","a2","d2" ) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand( &com22, "desc","a1", "d1","a2","d2" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com20, "desc","a1", "d1","a2","d2" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com21, "desc","a1", "d1","a2","d2" ) );

    CPPUNIT_ASSERT( to.commands()->addCommand( &com33, "desc","a1", "d1","a2","d2","a3","d3" ) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand( &com33, "desc","a1", "d1","a2","d2","a3","d3" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com30, "desc","a1", "d1","a2","d2","a3","d3" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com31, "desc","a1", "d1","a2","d2","a3","d3" ) );

    CPPUNIT_ASSERT( to.commands()->addCommand( &com44, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    CPPUNIT_ASSERT( ! to.commands()->addCommand( &com44, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com40, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );
    CPPUNIT_ASSERT( to.commands()->addCommand( &com41, "desc","a1", "d1","a2","d2","a3","d3","a4","d4" ) );


    std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> args;
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

    CPPUNIT_ASSERT( tsim->start()) ;
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
    CPPUNIT_ASSERT( tsim->stop()) ;
}

template<class T, class F1, class F2, class O, class CP>
Command<T> command(const char* name, F1 comf, F2 conf, O object, CP cp)
{
    return Command<typename boost::function_type_signature<F1>::representee>(std::string(name), comf, conf, object, cp);
    //return Command<T>(std::string(name), comf, conf, object, cp);
}


void Generic_TaskTest::testDSCommand()
{
    TaskObject to("task");

    // A command of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    CommandDS<bool(Generic_TaskTest*)> com0("c0",
                                            &Generic_TaskTest::cd0, &Generic_TaskTest::cn0,
                                            tc->engine()->commands() );

    command_ds("c0", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, tc->engine()->commands() );

    CommandDS<bool(Generic_TaskTest*,int)> com1("c1",
                                                &Generic_TaskTest::cd1, &Generic_TaskTest::cn1,
                                                tc->engine()->commands() );

    command_ds("c1",&Generic_TaskTest::cd1, &Generic_TaskTest::cn1, tc->engine()->commands() );

    command_ds("cs",&Generic_TaskTest::comstr, &Generic_TaskTest::comstr, tc->engine()->commands() );

    boost::shared_ptr<Generic_TaskTest> ptr( new Generic_TaskTest() );
    ValueDataSource<boost::weak_ptr<Generic_TaskTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<Generic_TaskTest> >( ptr );
    CPPUNIT_ASSERT( to.commands()->addCommandDS( wp.get(), com0, "desc" ) );
    CPPUNIT_ASSERT( to.commands()->addCommandDS( wp.get(), com1, "desc", "a1", "d1" ) );

    // this actually works ! the command will detect the deleted pointer.
    //ptr.reset();
    
    CPPUNIT_ASSERT( tsim->start()) ;

    CommandC c0  = to.commands()->create("c0");
    verifycommand(c0);
    CommandC c1  = to.commands()->create("c1").argC(1);
    verifycommand(c1);

    CPPUNIT_ASSERT( tsim->stop()) ;

}

void Generic_TaskTest::testMethod()
{
    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &Generic_TaskTest::m4, this);

    CPPUNIT_ASSERT_EQUAL( -1.0, m0() );
    CPPUNIT_ASSERT_EQUAL( -2.0, m1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, m2(1, 2.0) );
    CPPUNIT_ASSERT_EQUAL( -4.0, m3(1, 2.0, false) );
    CPPUNIT_ASSERT_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}

void Generic_TaskTest::testMethodFactory()
{
    // Test the addition of 'simple' methods to the operation interface,
    // and retrieving it back in a new Method object.

    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);

    TaskObject to("task");

    CPPUNIT_ASSERT( to.methods()->addMethod(&m0) );
    CPPUNIT_ASSERT( ! to.methods()->addMethod(&m0) );
    CPPUNIT_ASSERT( to.methods()->addMethod(&m1) );
    CPPUNIT_ASSERT( to.methods()->addMethod(&m2) );

    // test constructor
    Method<double(void)> mm0 = to.methods()->getMethod<double(void)>("m0");
    CPPUNIT_ASSERT( mm0.getMethodImpl() );
    CPPUNIT_ASSERT( mm0.ready() );

    // test operator=()
    Method<double(int)> mm1;
    mm1 = to.methods()->getMethod<double(int)>("m1");
    CPPUNIT_ASSERT( mm1.getMethodImpl() );
    CPPUNIT_ASSERT( mm1.ready() );

    Method<double(int,double)> mm2 = to.methods()->getMethod<double(int,double)>("m2");
    CPPUNIT_ASSERT( mm2.getMethodImpl() );
    CPPUNIT_ASSERT( mm2.ready() );

    // start the activity, such that methods are accepted.
    CPPUNIT_ASSERT( tsim->start()) ;
    // execute methods and check status:
    CPPUNIT_ASSERT_EQUAL( -1.0, mm0() );
    
    CPPUNIT_ASSERT_EQUAL( -2.0, mm1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, mm2(1, 2.0) );

}

void Generic_TaskTest::testCRMethod()
{
    this->ret = -3.3;

    Method<double&(void)> m0c("m0r", &Generic_TaskTest::m0r, this);
    Method<const double&(void)> m0cr("m0cr", &Generic_TaskTest::m0cr, this);

    Method<double(double&)> m1c("m1r", &Generic_TaskTest::m1r, this);
    Method<double(const double&)> m1cr("m1cr", &Generic_TaskTest::m1cr, this);

    CPPUNIT_ASSERT_EQUAL( -3.3, m0c() );
    CPPUNIT_ASSERT_EQUAL( -3.3, m0cr() );

    double value = 5.3;
    CPPUNIT_ASSERT_EQUAL( 5.3, m1c(value) );
    CPPUNIT_ASSERT_EQUAL( 5.3, m1cr(5.3) );
}

void Generic_TaskTest::testCRCommand()
{
    this->ret = -3.3;

    Command<double(double&)> c1c("c1r", &Generic_TaskTest::cn1r, &Generic_TaskTest::cd1r, this, tc->engine()->commands() );
    Command<double(const double&)> c1cr("c1cr", &Generic_TaskTest::cn1cr, &Generic_TaskTest::cd1cr, this, tc->engine()->commands() );

    CPPUNIT_ASSERT( tsim->start()) ;
    // execute commands and check status:
    double result = 0.0;
    CPPUNIT_ASSERT( c1c(result) );
    
    CPPUNIT_ASSERT( c1cr(1.0) );

    verifydispatch(*c1c.getCommandImpl());
    CPPUNIT_ASSERT_EQUAL( 2*ret, result );
    verifydispatch(*c1cr.getCommandImpl());
    CPPUNIT_ASSERT_EQUAL( 1.0, ret );
}


void Generic_TaskTest::testMethodFromDS()
{
    TaskObject to("task");

    Method<double(void)> m0("m0", &Generic_TaskTest::m0, this);
    Method<double(int)> m1("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4("m4", &Generic_TaskTest::m4, this);

    to.methods()->addMethod( &m0, "desc");
    to.methods()->addMethod( &m1, "desc", "a1", "d1");
    to.methods()->addMethod( &m2, "desc", "a1", "d1", "a2","d2");
    to.methods()->addMethod( &m3, "desc", "a1", "d1", "a2","d2","a3","d3");
    to.methods()->addMethod( &m4, "desc", "a1", "d1", "a2","d2","a3","d3", "a4","d4");

    double ret;
    MethodC mc0( to.methods(), "m0");
    mc0.ret(ret);
    MethodC mc1( to.methods(), "m1");
    mc1.argC(1).ret(ret);
    MethodC mc2( to.methods(), "m2");
    mc2.argC(1).argC(2.0).ret(ret);
    MethodC mc3( to.methods(), "m3");
    mc3.argC(1).argC(2.0).argC(false).ret(ret);
    MethodC mc4( to.methods(), "m4");
    mc4.argC(1).argC(2.0).argC(false).argC(std::string("hello")).ret(ret);

    CPPUNIT_ASSERT( mc0.execute() );
    CPPUNIT_ASSERT_EQUAL(-1.0, ret);
    CPPUNIT_ASSERT( mc1.execute() );
    CPPUNIT_ASSERT_EQUAL(-2.0, ret);
    CPPUNIT_ASSERT( mc2.execute() );
    CPPUNIT_ASSERT_EQUAL(-3.0, ret);
    CPPUNIT_ASSERT( mc3.execute() );
    CPPUNIT_ASSERT_EQUAL(-4.0, ret);
    CPPUNIT_ASSERT( mc4.execute() );
    CPPUNIT_ASSERT_EQUAL(-5.0, ret);
}

void Generic_TaskTest::testDSMethod()
{
    TaskObject to("task");

    // A method of which the first argument type is a pointer to the object
    // on which it must be invoked. The pointer is internally stored as a weak_ptr,
    // thus the object must be stored in a shared_ptr, in a DataSource. Scripting
    // requires this for copying state machines.

    Method<double(Generic_TaskTest*)> meth0("m0",
                                          &Generic_TaskTest::m0);

    method_ds("m0", &Generic_TaskTest::m0);

    Method<double(Generic_TaskTest*,int)> meth1("m1",
                                          &Generic_TaskTest::m1);

    method_ds("m1", &Generic_TaskTest::m1);
    method_ds("ms",&Generic_TaskTest::comstr );

    boost::shared_ptr<Generic_TaskTest> ptr( new Generic_TaskTest() );
    ValueDataSource<boost::weak_ptr<Generic_TaskTest> >::shared_ptr wp = new ValueDataSource<boost::weak_ptr<Generic_TaskTest> >( ptr );
    CPPUNIT_ASSERT( to.methods()->addMethodDS( wp.get(), meth0, "desc" ) );
    CPPUNIT_ASSERT( to.methods()->addMethodDS( wp.get(), meth1, "desc", "a1", "d1" ) );

    // this actually works ! the method will detect the deleted pointer.
    //ptr.reset();
    
    CPPUNIT_ASSERT( tsim->start()) ;

    double ret;
    MethodC c0  = to.methods()->create("m0").ret(ret);
    CPPUNIT_ASSERT( c0.execute() );
    CPPUNIT_ASSERT_EQUAL( -1.0, ret );
    MethodC c1  = to.methods()->create("m1").argC(1).ret(ret);
    CPPUNIT_ASSERT( c1.execute() );
    CPPUNIT_ASSERT_EQUAL( -2.0, ret );

    CPPUNIT_ASSERT( tsim->stop()) ;

}

void Generic_TaskTest::testAddMethod()
{
    Method<double(void)> m0 = method("m0", &Generic_TaskTest::m0, this);

    Method<double(int)> m1 = method("m1", &Generic_TaskTest::m1, this);
    Method<double(int,double)> m2 = method("m2", &Generic_TaskTest::m2, this);
    Method<double(int,double,bool)> m3 = method("m3", &Generic_TaskTest::m3, this);
    Method<double(int,double,bool,std::string)> m4 = method("m4", &Generic_TaskTest::m4, this);

    CPPUNIT_ASSERT_EQUAL( -1.0, m0() );
    CPPUNIT_ASSERT_EQUAL( -2.0, m1(1) );
    CPPUNIT_ASSERT_EQUAL( -3.0, m2(1, 2.0) );
    CPPUNIT_ASSERT_EQUAL( -4.0, m3(1, 2.0, false) );
    CPPUNIT_ASSERT_EQUAL( -5.0, m4(1, 2.0, false,"hello") );
}


void Generic_TaskTest::testAddCommand()
{

    Command<bool(void)> com0= command("command", &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int)> com11= command("command", &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, this, tc->engine()->commands() );
    Command<bool(int)> com10= command("command", &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, this, tc->engine()->commands() );

    Command<bool(int,double)> com22= command("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn2, this, tc->engine()->commands() );
    Command<bool(int,double)> com20= command("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double)> com21= command("command", &Generic_TaskTest::cd2, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char)> com33= command("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn3, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com30= command("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char)> com31= command("command", &Generic_TaskTest::cd3, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    Command<bool(int,double,char,bool)> com44= command("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn4, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com40= command("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn0, this, tc->engine()->commands() );
    Command<bool(int,double,char,bool)> com41= command("command", &Generic_TaskTest::cd4, &Generic_TaskTest::cn1, this, tc->engine()->commands() );

    // start the activity, such that commands are accepted.
    CPPUNIT_ASSERT( tsim->start()) ;
    // execute commands and check status:
    CPPUNIT_ASSERT( com0() );
    
    CPPUNIT_ASSERT( com11(1) );
    CPPUNIT_ASSERT( com10(1) );

    CPPUNIT_ASSERT( com22(1, 1.0) );
    CPPUNIT_ASSERT( com20(1, 1.0) );
    CPPUNIT_ASSERT( com21(1, 1.0) );

    CPPUNIT_ASSERT( com33(1, 1.0, char('a') ) );
    CPPUNIT_ASSERT( com30(1, 1.0, char('a') ) );
    CPPUNIT_ASSERT( com31(1, 1.0, char('a') ) );

    CPPUNIT_ASSERT( com44(1, 1.0, char('a'),true) );
    CPPUNIT_ASSERT( com40(1, 1.0, char('a'),true) );
    CPPUNIT_ASSERT( com41(1, 1.0, char('a'),true) );

    verifydispatch(*com0.getCommandImpl());
    verifydispatch(*com11.getCommandImpl());
    verifydispatch(*com10.getCommandImpl());
    verifydispatch(*com22.getCommandImpl());
    verifydispatch(*com20.getCommandImpl());
    verifydispatch(*com21.getCommandImpl());
    verifydispatch(*com33.getCommandImpl());
    verifydispatch(*com30.getCommandImpl());
    verifydispatch(*com31.getCommandImpl());
    verifydispatch(*com44.getCommandImpl());
    verifydispatch(*com40.getCommandImpl());
    verifydispatch(*com41.getCommandImpl());

    CPPUNIT_ASSERT( tsim->stop() );
}
