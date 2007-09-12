/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:50 CET 2005  types_test.cpp 

                        types_test.cpp -  description
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
 
 

#include "types_test.hpp"
#include <unistd.h>
#include <iostream>
#include <FunctionGraph.hpp>
#include <Method.hpp>
#include <SimulationActivity.hpp>
#include <SimulationThread.hpp>
#include <TaskContext.hpp>
#include <ProgramProcessor.hpp>
#include <StateMachineProcessor.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TypesTest );


void 
TypesTest::setUp()
{
    tc =  new TaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
    tsim = new SimulationActivity( 0.001, tc->engine() );
    SimulationThread::Instance()->stop();
}


void 
TypesTest::tearDown()
{
    tsim->stop();
    delete tc;
    delete tsim;
}


bool TypesTest::assertBool( bool b) { 
    return b;
}
bool TypesTest::assertMsg( bool b, const std::string& msg) {
    return b;
}


    TaskObject* TypesTest::createMethodFactory()
    {
        TaskObject* to = new TaskObject("test");
        to->methods()->addMethod( method("assert", &TypesTest::assertBool, this),
                                  "Assert", "bool", "");
        to->methods()->addMethod( method("assertMsg", &TypesTest::assertMsg, this),
                                     "Assert message", "bool", "", "text", "text" );
        return to;
    }


void TypesTest::testTypes()
{
    // XXX
    // for some reason, we can not compare the double6D's one fails
    // to parse, the others assert false, thus inequality.
    string test = 
        string("var int i2 = -1, j = 10, k; set k = 20\n") +
        "do test.assert( i2 == -1 ) ; do test.assert( j == 10 ); do test.assert(k == 20)\n" +
        "var double d = 10.0\n"+
        "do test.assert( d == 10.0 )\n" +
        "var bool b = false\n"+
        "do test.assert( b == false )\n" +
        "var string s=\"string\"\n"+
        "do test.assert( s == \"string\" )\n" +
        "const int ic = i2\n" +
        "do test.assert( ic == 0 )\n" + // i was null at parse time !
        "const double dc = 10.0\n"+     // evaluate 10.0 at parse time
        "do test.assert( dc == 10.0 )\n" +
        "const bool bc = true && false\n"+
        "do test.assert( bc == false )\n" +
        "const string sc= \"hello\"\n"+
        "do test.assert( sc == \"hello\" )\n" +
        "var array ar(10)\n"+
        "set ar[0] = 0.0\n"+
        // 40
        "set ar[1] = 1.0\n"+
        "set ar[2] = 0.2\n"+
        "set ar[8] = 0.8\n"+
        "set ar[9] = 9.0\n"+
        "do test.assert( ar[0] == 0.0 )\n"+
        "do test.assert( ar[1] == 1.0 )\n"+
        "do test.assert( ar[2] == 0.2 )\n"+
        "do test.assert( ar[8] == 0.8 )\n"+
        "do test.assert( ar[9] == 9.0 )\n"+
        "do test.assert( ar[10] == 0.0 )\n"+
        // 50
        "var string str(10) = \"hello\"\n"+
        "set str[0] = 'a'\n"+
        "set str[1] = 'b'\n"+
        "set str[2] = 'c'\n"+
        "do test.assert( str[0] == 'a' )\n"+
        "do test.assert( str[1] == 'b' )\n"+
        "do test.assert( str[2] == 'c' )\n"+
        "do test.assert( str == \"abclo\" )\n"+
        "do test.assert( str[20] == '\\0' )\n"+
        "do test.assert( str[8] == '\\0' )\n"+
        // 60
        "do test.assert( str[9] == '\\0' )\n"+
        "do test.assert( str[10] == '\\0' )\n";

    string state = string("StateMachine X { initial state Init { run {\n")
        +test
        +"} }\n"
        +"final state Fini {} }\n"
        +"RootMachine X x\n";
    
    string prog = string("program x {\n") + test + "}\n";
    executeStates(state);
                          
    // execute
    executePrograms(prog);

}

void TypesTest::testOperators()
{
    string prog = string("program x {\n") +
        "var int i = 3\n" +
        "var char c = 'c'\n" +
        "var double d = 10.0*i\n"+
        "do test.assert( d == 30.0 )\n" +
        "var bool b = false\n"+
        "var string s=\"string\"\n"+
//         "do test.assert( d == 10.0 )\n" +
        "set b = b\n or\n b\n and\n true\n && false\n || true\n"+
        "do test.assert( b == false )\n" +
//         "set s = s+\"abc\"\n"+
        "}";
    // execute
    executePrograms(prog);
}

void TypesTest::testProperties()
{
    string prog = string("program x {\n") +
        "do test.assert( Double1 == 1.234 )\n" +
        "do test.assert( Double2 == 2.234 )\n" +
        "do test.assert( Double3 == 3.234 )\n" +
        "do test.assert( Collection.Double1 == 1.234 )\n" +
        "do test.assert( Collection.Double3 == 3.234 )\n" +
        "do test.assert( Collection.Collection.Double1 == 1.234 )\n" +
        "do test.assert( Collection.Collection.Collection.Double3 == 3.234 )\n" +
        "do test.assert( V[0] == 4.0 )\n" +
        "do test.assert( V[1] == 4.0 )\n" +
        "do test.assert( V[2] == 4.0 )\n" +
        "do test.assert( V[3] == 4.0 )\n" +
        //"do test.assert( V.size() == 4 )\n" +
        "set Double1 = 4.321\n" +
        "set Double2 = Double1\n" +
        // -> = 10
        "set Collection.Double3 = 0.3\n" +
        "set V[0] = 0.321\n" +
        "set V[1] = 1.321\n" +
        "set V[2] = 2.321\n" +
        "set V[3] = 3.321\n" +
        "do test.assert( Double1 == 4.321 )\n" +
        "do test.assert( Double2 == 4.321 )\n" +
        "do test.assert( Double3 == 0.3 )\n" +
        "set Collection.Collection.Collection.Double3 = 3.0\n" +
        "do test.assert( Double3 == 3.0 )\n" +
        "}";

    Property<PropertyBag> pb("Collection","");
    Property<double> pd1("Double1","",1.234);
    Property<double> pd2("Double2","",2.234);
    Property<double> pd3("Double3","",3.234);

    Property< std::vector<double> > pv("V","",std::vector<double>(4, 4.0) );

    pb.value().add( &pd1 );
    pb.value().add( &pd3 );
    pb.value().add( &pb ); // yep, recursive !

    tc->properties()->addProperty( &pd1 );
    tc->properties()->addProperty( &pd2 );
    tc->properties()->addProperty( &pd3 );
    tc->properties()->addProperty( &pb );
    tc->properties()->addProperty( &pv );

    // execute
    executePrograms(prog);

    CPPUNIT_ASSERT_EQUAL( 4.321, pd1.get() );
    CPPUNIT_ASSERT_EQUAL( 3.0, pd3.get() );
    CPPUNIT_ASSERT_EQUAL( 0.321, pv.value()[0] );
    CPPUNIT_ASSERT_EQUAL( 1.321, pv.value()[1] );
    CPPUNIT_ASSERT_EQUAL( 2.321, pv.value()[2] );
    CPPUNIT_ASSERT_EQUAL( 3.321, pv.value()[3] );
}

void TypesTest::executePrograms(const std::string& prog )
{
    CPPUNIT_ASSERT( tc->engine() );
    CPPUNIT_ASSERT( tc->engine()->programs());

    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }

    CPPUNIT_ASSERT( tc->engine()->programs()->loadProgram( *pg_list.begin() ) );
    CPPUNIT_ASSERT( tsim->start() );
    CPPUNIT_ASSERT( (*pg_list.begin())->start() );

    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1000) );

    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " Program error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( errormsg.str(), false );
    }

    if ( (*pg_list.begin())->isRunning() ) {
        stringstream errormsg;
        errormsg << " Program still running on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( errormsg.str(), false );
    }

    if ( (*pg_list.begin())->isStopped() == false ) {
        stringstream errormsg;
        errormsg << " Program not stopped on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( errormsg.str(), false );
    }
    CPPUNIT_ASSERT( (*pg_list.begin())->stop() );
    tsim->stop();
    tc->engine()->programs()->unloadProgram( (*pg_list.begin())->getName() );
}

void TypesTest::executeStates(const std::string& state )
{
    CPPUNIT_ASSERT( tc->engine() );
    CPPUNIT_ASSERT( tc->engine()->states());
    Parser::ParsedStateMachines pg_list;
    try {
        pg_list = parser.parseStateMachine( state, tc );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }

    CPPUNIT_ASSERT( tc->engine()->states()->loadStateMachine( *pg_list.begin() ) );
    CPPUNIT_ASSERT( tsim->start() );
    CPPUNIT_ASSERT( (*pg_list.begin())->activate() );
    CPPUNIT_ASSERT( (*pg_list.begin())->start() );

    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1000) );
    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " State error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( errormsg.str(), false );
    }

    CPPUNIT_ASSERT( (*pg_list.begin())->stop() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(100) );
    CPPUNIT_ASSERT( (*pg_list.begin())->deactivate() );

    tsim->stop();
    tc->engine()->states()->unloadStateMachine( (*pg_list.begin())->getName() );
}

    

