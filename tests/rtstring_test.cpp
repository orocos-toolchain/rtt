/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"

#include <rtt-config.h>
#include <extras/SimulationThread.hpp>
#include <extras/SimulationActivity.hpp>
#include <scripting/StateMachine.hpp>
#include <scripting/ParsedStateMachine.hpp>
#include <scripting/DumpObject.hpp>
#include <scripting/Parser.hpp>

#include <Service.hpp>
#include <TaskContext.hpp>
#include <OperationCaller.hpp>
#include <Port.hpp>
#include <scripting/ScriptingService.hpp>
#include "operations_fixture.hpp"

#include <string>
#include <iostream>
#include <sstream>

using namespace RTT;
using namespace RTT::detail;
using namespace std;

class StateTest
    : public OperationsFixture
{
public:
    Parser parser;
    ScriptingService::shared_ptr sa;

    void doState(const std::string& name, const std::string& prog, TaskContext*, bool test=true );
    void parseState( const std::string& prog, TaskContext*, bool test=true );
    void runState(const std::string& name, TaskContext*, bool test=true );
    void checkState( const std::string& name, TaskContext*, bool test=true );
    void finishState( std::string const& name, TaskContext*, bool test=true );

    std::string sline;
public:
    StateTest()
        :
         sa( ScriptingService::Create(tc) )
    {
        tc->stop();
        tc->setActivity( new SimulationActivity(0.001) );

        tc->start();
        i = 0;
        SimulationThread::Instance()->stop();
    }
    ~StateTest(){
    }
};

BOOST_FIXTURE_TEST_SUITE( RtStringTestSuite, StateTest )

BOOST_AUTO_TEST_CASE( testCreateRtString )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string s1\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}

BOOST_AUTO_TEST_CASE( testCreateRtstringOfFixedSize )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string s1(22)\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}

#if 0
// this is incorrect syntax:
BOOST_AUTO_TEST_CASE( testCreateRtstringFromCharPtr )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string s1(\"hello world\")\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;
    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}
#endif

BOOST_AUTO_TEST_CASE( testCreateRtstringFromRtString )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string s1 = rt_string(\"hello world\")\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}

BOOST_AUTO_TEST_CASE( testConcatRtstring )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string s1\n"
        + "     entry {\n"
        + "       s1 =  rt_string(\"Multiply \")\n"
        + "       s1 =  s1 + 10\n"
        + "       s1 =  s1 + rt_string(\" times \")\n"
        + "       s1 =  s1 + 33.3\n"
        + "       s1 =  rt_string(\"Now: \") + 1 + rt_string(\"st \") + s1 + '!'\n"
        + "     }\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}

BOOST_AUTO_TEST_CASE( testRtstringConversion )
{
    string prog = string("StateMachine X {\n")
        + "   initial state INIT {\n"
        + "     var rt_string rts1\n"
        + "     var string s1\n"
        + "     entry {\n"
        + "       s1 =  \"s1\"\n"
        + "       rts1 =  rt_string(\"rts1\")\n"
        + "       test.assert(s1 ==  \"s1\")\n"
        + "       test.assert(rts1 ==  rt_string(\"rts1\"))\n"
        + "       s1 = string(rts1)\n"
        + "       test.assert(s1 == string(rts1))\n"
        + "       test.assert(s1 == \"rts1\")\n"
        + "       s1 =  \"s1\"\n"
        + "       rts1 = rt_string(s1)\n"
        + "       test.assert(rts1 == rt_string(\"s1\"))\n"
        + "       s1 =  string( rt_string(\" s1 \") )\n"
        + "       rts1 =  rt_string( string(\" rts1 \") )\n"
        + "       test.assert(s1 ==  \" s1 \")\n"
        + "       test.assert(rts1 ==  rt_string(\" rts1 \"))\n"
        + "     }\n"
        + "     transitions { select FINI }\n"
        + "   }\n"
        + "   final state FINI {\n"
        + "   }\n"
        + " }\n"
        + " RootMachine X x\n" // instantiate a non hierarchical SC
        ;

    this->doState("x", prog, tc );
    this->finishState( "x", tc );
}


BOOST_AUTO_TEST_SUITE_END()

void StateTest::doState(  const std::string& name, const std::string& prog, TaskContext* tc, bool test )
{
    BOOST_CHECK( tc->engine() );

    parseState( prog, tc, test);
    runState(name, tc, test);
    checkState(name, tc, test);
}

void StateTest::parseState(const std::string& prog, TaskContext* tc, bool test )
{
    // Alternative way: test ScriptingService as well.
    try {
        sa->loadStateMachines( prog, std::string("state_test.cpp"), true );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( !test, exc.what() );
        }
    catch( const parse_exception& exc )
        {
            BOOST_REQUIRE_MESSAGE( !test, exc.what() );
        }
    catch( const program_load_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( !test, e.what() );
        }
    catch( const std::exception& e ) {
            BOOST_CHECK_MESSAGE( !test , e.what());
            BOOST_REQUIRE_MESSAGE( !test, "Uncaught Processor load exception" );
    }
}

void StateTest::runState(const std::string& name, TaskContext* tc, bool test )
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    sm->trace(true);
    OperationCaller<bool(StateMachine*)> act = tc->provides(name)->getOperation("activate");
    OperationCaller<bool(StateMachine*)> autom = tc->provides(name)->getOperation("automatic");
    BOOST_CHECK( act(sm.get()) );
    BOOST_CHECK( SimulationThread::Instance()->run(1) );
    BOOST_CHECK_MESSAGE( sm->isActive(), "Error : Activate Command for '"+sm->getName()+"' did not have effect." );
    BOOST_CHECK( autom(sm.get()) || !test  );

    BOOST_CHECK( SimulationThread::Instance()->run(1000) );
}

void StateTest::checkState(const std::string& name, TaskContext* tc, bool test )
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    if ( test ) {
        // check error status of parent :
        BOOST_CHECK_MESSAGE( sm->isActive(), "Error : State Context '"+sm->getName()+"' did not get activated." );
        stringstream errormsg;
        int line = sm->getLineNumber();
        errormsg <<" in StateMachine "+sm->getName()
                 <<" in state "<< (sm->currentState() ? sm->currentState()->getName() : "(null)")
                 <<" on line " << line <<" of that StateMachine:"<<endl;
        {
            stringstream sctext( sm->getText() );
            int cnt = 1;
            while ( cnt++ <line && sctext ) {
                string garbage;
                getline( sctext, garbage, '\n' );
            }
            getline( sctext, sline, '\n' );
        }
        errormsg <<"here  > " << sline << endl;
        if ( sm->inError() ) {
            RTT::scripting::DumpObject( tc->provides() );
            RTT::scripting::DumpObject( tc->provides(name) );
        }
        BOOST_CHECK_MESSAGE( sm->inError() == false, "Runtime error (inError() == true) encountered" + errormsg.str() );
        // check error status of all children:
        StateMachine::ChildList cl = sm->getChildren();
        StateMachine::ChildList::iterator clit = cl.begin();
        while( clit != cl.end() ) {
            line = (*clit)->getLineNumber();
            {
                stringstream sctext( (*clit)->getText() );
                int cnt = 1;
                while ( cnt++ <line && sctext ) {
                    string garbage;
                    getline( sctext, garbage, '\n' );
                }
                getline( sctext, sline, '\n' );
            }
            stringstream cerrormsg;
            if ( (*clit)->currentState() )
                cerrormsg <<" in child "<< (*clit)->getName() <<" in state "<<(*clit)->currentState()->getName()<< " on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl <<"here  > " << sline << endl;
            else
                cerrormsg <<" child "<< (*clit)->getName() << " (deactivated) on line " <<  (*clit)->getLineNumber() <<" of that StateMachine."<<endl<<"here  > " << sline << endl;

            BOOST_CHECK_MESSAGE( (*clit)->inError() == false, "Runtime error (inError() == true) encountered" + cerrormsg.str() );
            if ( (*clit)->inError() == false && sm->inError() == true) {
                cout << "Child Status:" << cerrormsg.str() << endl;
            }
            ++clit;
        }
    }
}

void StateTest::finishState(std::string const& name, TaskContext* tc, bool test)
{
    StateMachinePtr sm = sa->getStateMachine(name);
    BOOST_REQUIRE( sm );
    BOOST_CHECK( sa->getStateMachine( name )->stop() );
    BOOST_CHECK( SimulationThread::Instance()->run(500) );
    if (test) {
        stringstream errormsg;
        errormsg << " on line " << sm->getLineNumber() <<", status is "<< sa->getStateMachineStatusStr(name) <<endl <<"here  > " << sline << endl;;
        BOOST_CHECK_MESSAGE( sm->isStopped(), "StateMachine stalled " + errormsg.str() );
    }
    // you can call deactivate even when the proc is not running.
    // but deactivation may be 'in progress if exit state has commands in it.
    BOOST_CHECK( sa->getStateMachine( name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(200) );
    if ( sm->isActive() )
        BOOST_CHECK( sa->getStateMachine( name )->deactivate() );
    BOOST_CHECK( SimulationThread::Instance()->run(200) );
    BOOST_CHECK( sa->getStateMachine( name )->isActive() == false );

    // only stop now, since deactivate won't work if simtask not running.
    tc->stop();

    try {
        BOOST_CHECK( sa->unloadStateMachine( name ) );
    }
    catch( const program_unload_exception& e)
        {
            BOOST_REQUIRE_MESSAGE( false, e.what() );
        }
    catch( ... ) {
            BOOST_REQUIRE_MESSAGE( false, "Uncaught Processor unload exception" );
    }

}
