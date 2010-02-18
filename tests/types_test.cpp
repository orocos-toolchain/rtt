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

#include <iostream>
#include <scripting/FunctionGraph.hpp>
#include <Method.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <interface/ServiceProvider.hpp>
#include <TaskContext.hpp>
#include <interface/ServiceProvider.hpp>
#include <types/GlobalsRepository.hpp>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


void
TypesTest::setUp()
{
    tc =  new TaskContext( "root" );
    sa = dynamic_cast<scripting::ScriptingAccess*>( tc->provides()->getService("scripting").get() );
    tc->provides()->addService( this->createMethodFactory() );
    tc->setActivity( new SimulationActivity( 0.001 ));
    SimulationThread::Instance()->stop();
}


void
TypesTest::tearDown()
{
    delete tc;
}


bool TypesTest::assertBool( bool b) {
    return b;
}
bool TypesTest::assertEqual( double a, double b) {
    BOOST_CHECK_EQUAL(a,b);
    return a == b;
}
bool TypesTest::assertMsg( bool b, const std::string& msg) {
    return b;
}


    ServiceProvider::shared_ptr TypesTest::createMethodFactory()
    {
        ServiceProvider::shared_ptr to = ServiceProvider::Create("test");
        to->addOperation("assert", &TypesTest::assertBool, this).doc("Assert").arg("bool", "");
        to->addOperation("assertEqual", &TypesTest::assertEqual, this).doc("Assert equality").arg("a1", "").arg("a2", "");
        //to->addOperation("assertMsg", &TypesTest::assertMsg, this).doc("Assert message").arg("bool", "").arg("text", "text");
        to->addOperation("print", &TypesTest::print, this ).doc("print").arg("v", "v");
        to->addOperation("printb", &TypesTest::printb, this ).doc("printb").arg("v", "v");
        to->addOperation("pass",&TypesTest::pass, this);
        return to;
    }

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  TypesTestSuite,  TypesTest )

BOOST_AUTO_TEST_CASE( testTypes )
{
    // XXX
    // for some reason, we can not compare the double6D's one fails
    // to parse, the others assert false, thus inequality.
    string test =
        // Line 2 (see below):
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
        "var array ar(10)\n"+ // size hint syntax != constructor syntax
        "do test.assert( ar.size == 10)\n"+
        // 20:
        "do test.assert( ar.capacity == 10)\n"+
        "set ar[0] = 0.0\n"+
        "set ar[1] = 1.0\n"+
        "set ar[2] = 0.2\n"+
        "set ar[8] = 0.8\n"+
        "set ar[9] = 9.0\n"+
        "do test.assert( ar[0] == 0.0 )\n"+
        "do test.assert( ar[1] == 1.0 )\n"+
        "do test.assert( ar[2] == 0.2 )\n"+
        "do test.assert( ar[8] == 0.8 )\n"+
        // 30:
        "do test.assert( ar[9] == 9.0 )\n"+
        "do test.assert( ar[10] == 0.0 )\n"+
        "var array ar1 = array(12,2.0)\n"+
        "do test.assert(ar1.size == 12)\n"+
        "do test.print(ar1[11])\n"+
        "do test.assert(ar1[0] == 2.0)\n"+
        "var array ar2 = array(5,3.0)\n"+
        "do test.assert(ar2.size == 5)\n"+
        "do test.assert(ar2[0] == 3.0)\n"+
        "var array ar3(3) = array(2.0,3.0,4.0)\n"+
        "do test.assert(ar3.size == 3)\n"+
        //40:
        "do test.assert(ar3[0]==2.0)\n"+
        "do test.assert(ar3[1]==3.0)\n"+
        "do test.assert(ar3[2]==4.0)\n"+
        "var array ar4 = array(2.0,3.0,4.0,5.0)\n"+
        "do test.assert(ar4.size == 4)\n"+
        "do test.assert(ar4[0]==2.0)\n"+
        "do test.assert(ar4[1]==3.0)\n"+
        "do test.assert(ar4[2]==4.0)\n"+
        "do test.assert(ar4[3]==5.0)\n"+
        "var string str(10) = \"hello\"\n"+
        // 50:
        "do test.assert( str.size == 5)\n"+
        "do test.assert( str.capacity >= 10)\n"+ // fails: check assignment & constructor stuff.
        "set str[0] = 'a'\n"+
        "set str[1] = 'b'\n"+
        "set str[2] = 'c'\n"+
        "do test.assert( str[0] == 'a' )\n"+
        "do test.assert( str[1] == 'b' )\n"+
        "do test.assert( str[2] == 'c' )\n"+
        "do test.assert( str == \"abclo\" )\n"+
        "do test.assert( str[20] == '\\0' )\n"+
        // 60:
        "do test.assert( str[8] == '\\0' )\n"+
        "do test.assert( str[9] == '\\0' )\n"+
        "do test.assert( str[10] == '\\0' )\n"+
        // various array constructors
        "set ar2 = array(10.,5.)\n"+ // keeps capacity
        "do test.assertEqual( ar2.size, 2)\n"+
        "do test.assertEqual( ar2.capacity, 5)\n"+
        "do test.assert( ar2[0] == 10.0 )\n"+
        "do test.assert( ar2[1] == 5.0 )\n"+
        "set ar3 = array(10.)\n"+
        "do test.assert( ar3.size == 1)\n"+
        // 70:
        "do test.assert( ar3.capacity >= 1)\n"+
        "do test.assert( ar3[0] == 10.0 )\n"+
        "set ar4 = array(2, 7.)\n"+
        "do test.assert( ar4.size == 2)\n"+
        "do test.assert( ar4.capacity >= 2)\n"+
        "do test.assert( ar4[0] == 7.0 )\n"+
        "do test.assert( ar4[1] == 7.0 )\n"+
        // various array assignments
        "set ar2 = ar4\n"+
        "do test.assert( ar2.size == 2)\n"+
        "do test.assert( ar2.capacity >= 5)\n"+
        // 80:
        "do test.assert( ar2[0] == 7.0 )\n"+
        "do test.assert( ar2[1] == 7.0 )\n"+
        "do test.assert( ar.capacity == 10)\n"+ // pre-condition
        "var array ar7(7) = array(7)\n"+
        "set ar = ar7\n"+                       // assignment must keep capacity and only change size
        //"do test.print( ar.size )\n"+
        "do test.assert( ar.size == 7)\n"+
        //"do test.print( ar.capacity )\n"+
        "do test.assertEqual( ar.capacity, 10)\n"+ // check keeping capacity: ar(10) vs ar2(2)
        //-- This fails because .capacity() gets a copy of the std::vector
        // See DataSourceAdaptor.hpp:263 and :676 ('returns/make a copy'
        "do test.assert( ar2[0] == 7.0 )\n"+
        "do test.assert( ar2[1] == 7.0 )\n";

    string state = string("StateMachine X { initial state Init { entry {\n")
        +test
        +"} }\n"
        +"final state Fini {} }\n"
        +"RootMachine X x\n";

    string prog = string("program x {\n") + test + "}\n";
    // execute
    executePrograms(prog);
    executeStates(state);


}

BOOST_AUTO_TEST_CASE( testOperators )
{
    string prog = string("program x {\n") +
        "var int i = 3\n" +
        "var char c = 'c'\n" +
        "var double d = 10.0*i\n"+
        "do test.assert( d == 30.0 )\n" +
        "var bool b = false\n"+
        "var string s=\"string\"\n"+
//         "do test.assert( d == 10.0 )\n" +
        "set b = b\n ||\n b\n &&\n true\n && false\n || true\n"+
        "do test.assert( b == false )\n" +
        "var array a1 = array(2, 7.)\n"+
        "do test.assert( a1.size == 2 )\n" +
        "do test.assert( a1.capacity == 2 )\n" +
//         "set s = s+\"abc\"\n"+
        "}";
    // execute
    executePrograms(prog);
}

BOOST_AUTO_TEST_CASE( testConversions )
{
    string prog = string("program x {\n") +
        "var int i = 3.0\n" +
        "var double d = float(10.0*i)\n"+
        "do test.assert( float(d) == float(30.0) )\n" +
        "var float f = 5\n" +
        "set f = double(5) * double(-1) + i\n" +
        "set i = f\n" +
        "set f = i\n" +
        "set i = double(float(int(f)))\n" +
        "set f = int(float(double(int(3.333))))\n" +
        "do test.assert( f == 3 )\n" +
        "}";
    // execute
    executePrograms(prog);
}

BOOST_AUTO_TEST_CASE( testProperties )
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

    BOOST_CHECK_EQUAL( 4.321, pd1.get() );
    BOOST_CHECK_EQUAL( 3.0, pd3.get() );
    BOOST_CHECK_EQUAL( 0.321, pv.value()[0] );
    BOOST_CHECK_EQUAL( 1.321, pv.value()[1] );
    BOOST_CHECK_EQUAL( 2.321, pv.value()[2] );
    BOOST_CHECK_EQUAL( 3.321, pv.value()[3] );
}

BOOST_AUTO_TEST_CASE( testOperatorOrder )
{
    string prog = string("program x {\n") +
        "do test.assert( 6/2*4 == 12 )\n" + // not: 0
        "do test.assert( 6*2/4 == 3 )\n" +  // not: 0
        "do test.assert( 3+2*5 == 13)\n" +  // not: 30
        "do test.assert( 3 < 2 != 5 > 1)\n" +
        "do test.assert( 3*2 <= 12/2 )\n" +
        "do test.assert( 3*2 < 6+1 )\n" +
        "do test.assert( 6 - 9 % 2*3 ==  15/3 % 3 + 1 )\n" + // 3 == 3
        "do test.assert( 3*(2+1) == 9 )\n" +
        "}";
    // execute
    executePrograms(prog);
}

BOOST_AUTO_TEST_CASE( testGlobals )
{
    GlobalsRepository::Instance()->setValue( new Constant<double>("cd_num", 3.33));
    GlobalsRepository::Instance()->setValue( new Constant<string>("c_string", "Hello World!"));
    GlobalsRepository::Instance()->setValue( new Attribute<double>("d_num", 3.33));
    string prog = string("program x {\n") +
        "do test.assert( cd_num == 3.33 )\n" +
        "do test.assert( cd_num == d_num )\n" +
        "do test.assert( c_string == \"Hello World!\")\n" +
        "set d_num = 6.66\n" +
        "do test.assert( d_num == 6.66 )\n" +
        "}";
    // execute
    executePrograms(prog);
}

BOOST_AUTO_TEST_CASE( testFlowStatus )
{
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("NewData") );
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("OldData") );
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("NoData") );
    string prog = string("program x {\n") +
        "do test.assert( NewData )\n" +
        "do test.assert( OldData )\n" +
        "do test.assert( !bool(NoData) )\n" +
        "do test.assert( NewData > NoData )\n" +
        "do test.assert( NewData > OldData )\n" +
        "do test.assert( OldData > NoData )\n" +
        "do test.assert( OldData == OldData )\n" +
        "if ( bool(NewData) && OldData ) then {\n" +
        "} else {\n" +
        "   do test.assert(false)\n" +
        "}\n" +
        "if ( bool(NoData) ) then {\n" +
        "   do test.assert(false)\n" +
        "}\n" +
        "if ( !bool(NoData) ) then {} else {\n" +
        "   do test.assert(false)\n" +
        "}\n" +
        "}";
    // execute
    executePrograms(prog);
}

BOOST_AUTO_TEST_SUITE_END()

void TypesTest::executePrograms(const std::string& prog )
{
    BOOST_CHECK( tc->engine() );

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
            BOOST_REQUIRE( false && "Got empty test program." );
        }

    BOOST_CHECK( sa->loadProgram( *pg_list.begin() ) );
    BOOST_CHECK( (*pg_list.begin())->start() );

    BOOST_CHECK( SimulationThread::Instance()->run(1000) );

    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " Program error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( false, errormsg.str() );
    }

    if ( (*pg_list.begin())->isRunning() ) {
        stringstream errormsg;
        errormsg << " Program still running on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( false, errormsg.str() );
    }

    if ( (*pg_list.begin())->isStopped() == false ) {
        stringstream errormsg;
        errormsg << " Program not stopped on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( false , errormsg.str() );
    }
    BOOST_CHECK( (*pg_list.begin())->stop() );
    sa->unloadProgram( (*pg_list.begin())->getName() );
}

void TypesTest::executeStates(const std::string& state )
{
    BOOST_CHECK( tc->engine() );
    Parser::ParsedStateMachines pg_list;
    try {
        pg_list = parser.parseStateMachine( state, tc );
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_CHECK_MESSAGE( false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_CHECK( false );
        }

    BOOST_CHECK( sa->loadStateMachine( *pg_list.begin() ) );
    BOOST_CHECK( (*pg_list.begin())->activate() );
    BOOST_CHECK( (*pg_list.begin())->start() );

    BOOST_CHECK( SimulationThread::Instance()->run(1000) );
    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " State error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( false , errormsg.str());
    }

    BOOST_CHECK( (*pg_list.begin())->stop() );
    BOOST_CHECK( SimulationThread::Instance()->run(100) );
    BOOST_CHECK( (*pg_list.begin())->deactivate() );

    sa->unloadStateMachine( (*pg_list.begin())->getName() );
}
