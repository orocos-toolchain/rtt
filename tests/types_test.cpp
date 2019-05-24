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

#include "unit.hpp"

#include <iostream>
#include <scripting/FunctionGraph.hpp>
#include <OperationCaller.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <Service.hpp>
#include <TaskContext.hpp>
#include <scripting/ScriptingService.hpp>
#include <scripting/Parser.hpp>
#include <Service.hpp>
#include <types/GlobalsRepository.hpp>
#include <types/Types.hpp>
#include <types/StructTypeInfo.hpp>
#include <types/SequenceTypeInfo.hpp>

#include "datasource_fixture.hpp"
#include "operations_fixture.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

class TypesTest : public OperationsFixture
{
public:
    Parser parser;
    ScriptingService::shared_ptr sa;
    void executePrograms(const std::string& prog);
    void executeStates(const std::string& state);

    TypesTest()
    : sa( ScriptingService::Create(tc) )
    {
        tc->stop();
        BOOST_REQUIRE( tc->setActivity(new SimulationActivity(0.01)) );
        BOOST_REQUIRE( tc->start() );
        SimulationThread::Instance()->stop();
    }
    ~TypesTest(){
    }
};


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  TypesTestSuite,  TypesTest )

//! Tests the preservation of the capacity of a string in the type system.
BOOST_AUTO_TEST_CASE( testStringCapacity )
{
    Attribute<string> str = Types()->type("string")->buildVariable("str",10);
    size_t strCapacity=str.get().capacity();
    // check size hint:
    BOOST_CHECK_EQUAL( str.get().size() , 10 );

    // False test http://www.cplusplus.com/reference/string/string/capacity/
    // BOOST_CHECK_EQUAL( str.get().capacity() , 10 );

    str.set() = "hello"; // note: assign to C string preserves capacity
    BOOST_CHECK_EQUAL( str.get().size() , 5 );
    BOOST_CHECK_EQUAL( str.get().capacity() , strCapacity );

    // create empty target:
    Attribute<string> copy("copy");
    BOOST_CHECK_EQUAL( copy.get().size() , 0 );
    // False test http://www.cplusplus.com/reference/string/string/capacity/
    //BOOST_CHECK_EQUAL( copy.get().capacity() , 0 );

    // copy str to target and check:
    copy.getDataSource()->update( str.getDataSource().get() );

    BOOST_CHECK_EQUAL( copy.get().size(), 5 );
    // We can't assume much here: on Linux copy.get().capacity() returns 5, on win32: 10
    //BOOST_CHECK_EQUAL( copy.get().capacity(), strCapacity );
    BOOST_CHECK_EQUAL( copy.get(), str.get() );

    copy.set() = "world";

    // now copy target back to str and check if capacity remains:
    str.getDataSource()->update( copy.getDataSource().get() );
    BOOST_CHECK_EQUAL( str.get().size() , 5 );
    BOOST_CHECK_EQUAL( str.get().capacity() , strCapacity );
    BOOST_CHECK_EQUAL( copy.get(), str.get() );



    // Same exercise as above, but with updateCommand():
    str.set() = "hello"; // note: assign to C string preserves capacity
    strCapacity=str.get().capacity();

    BOOST_CHECK_EQUAL( str.get().size() , 5 );
    // False test http://www.cplusplus.com/reference/string/string/capacity/
    //BOOST_CHECK_EQUAL( str.get().capacity() , 10 );

    // copy str to target and check:
    ActionInterface* act = copy.getDataSource()->updateAction( str.getDataSource().get() );
    BOOST_CHECK( act );
    act->readArguments();
    BOOST_CHECK( act->execute() );
    delete act;

    BOOST_CHECK_EQUAL( copy.get().size(), 5 );
    // We can't assume much here: on Linux copy.get().capacity() returns 5, on win32: 10
    //BOOST_CHECK_EQUAL( copy.get().capacity(), strCapacity );
    BOOST_CHECK_EQUAL( copy.get(), str.get() );

    copy.set() = "world";

    // now copy target back to str and check if capacity remains:
    act = str.getDataSource()->updateAction( copy.getDataSource().get() );
    BOOST_CHECK( act );
    act->readArguments();
    BOOST_CHECK( act->execute() );
    delete act;

    BOOST_CHECK_EQUAL( str.get().size() , 5 );
    BOOST_CHECK_EQUAL( str.get().capacity() , strCapacity );
    BOOST_CHECK_EQUAL( copy.get(), str.get() );

}

BOOST_AUTO_TEST_CASE( testTypes )
{
    Types()->addType( new StructTypeInfo<AType,false>("astruct"));
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
//        "do test.print(ar1[11])\n"+
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
        "var string str(10)\n"+
        "var int strCapacity = str.capacity\n"
        // 50:
//        "do test.print(str.size)\n"+
//        "do test.print(str.capacity)\n"+
        "do test.assertEqual( str.size, 10)\n"+
//        "do test.assertEqual( str.capacity, 10)\n"
        "set str = \"hello\"\n"+
//        "do test.print(str.size)\n"+
//        "do test.print(str.capacity)\n"+
        "do test.assertEqual( str.size, 5)\n"+
        "do test.assertEqual( str.capacity, strCapacity)\n"+ // if this fails, the COW implementation of std::string fooled us again.
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
        "do test.assert( ar2[0] == 7.0 )\n"+
        "do test.assert( ar2[1] == 7.0 )\n"+
        "var astruct aVar\n" +
        // 90:
        "var astruct bVar\n" +
        "aVar.a = 42\n" +
        "do test.assertMsg(aVar.a == 42, \"aVar.a != 42 assignement failed - testTypes\")\n" +
        "bVar = aVar\n" +
        "do test.assertMsg(bVar.a == 42, \"bVar.a != 42 assignement failed - testTypes\")\n" +
        " ";

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

BOOST_AUTO_TEST_CASE( testAliases )
{
    Types()->addType( new StructTypeInfo<AType,false>("astruct"));
    Types()->addType( new StructTypeInfo<AType,false>("aalias1"));
    Types()->type("astruct")->addAlias("aalias2");

    BOOST_CHECK( Types()->type("astruct") != 0 );
    BOOST_CHECK( Types()->type("aalias1") != 0 );
    BOOST_CHECK( Types()->type("aalias2") != 0 );

    BOOST_CHECK_EQUAL( Types()->type("astruct"), Types()->type("aalias1") );
    BOOST_CHECK_EQUAL( Types()->type("astruct"), Types()->type("aalias2") );

    BOOST_CHECK_EQUAL( "astruct", Types()->type("astruct")->getTypeNames()[0] );
    BOOST_CHECK_EQUAL( "aalias1", Types()->type("astruct")->getTypeNames()[1] );
    BOOST_CHECK_EQUAL( "aalias2", Types()->type("astruct")->getTypeNames()[2] );
}

BOOST_AUTO_TEST_CASE( testCharType )
{
    string test =
        // Line 2 (see below):
        string("var char c = char('c');\n") +
//        "do test.assert( c == c ); \n" +
//        "do test.assert( char('c') == c ); \n" +
        "var char d = char('d');\n" +
//        "do test.assert( c != d ); \n" +
//        "set c = 'a';\n" +
        "set c = char('a');\n" +
        "do test.assert( char('a') == c ); \n" +
        "do test.assert( c != d ); \n" +
        "do test.assert( c <  d ); \n" +
        "do test.assert( c <= d ); \n" +
        "do test.assert( d >  c ); \n" +
        "do test.assert( d >= c ); \n" +
        "set d = c;\n" +
        "do test.assert( c == d ); \n" +
        "do test.assert( char('a') == d ); \n" +
        "do test.assert( char('a') == char('a') ); \n" +
        "do test.assert( char('a') != char('b') ); \n" +
        "do test.assert( char('a') <  char('b') ); \n" +
        "do test.assert( char('a') <= char('b') ); \n" +
        "do test.assert( char('a') <= char('a') ); \n" +
        "do test.assert( char('z') >  char('w') ); \n" +
        "do test.assert( char('z') >= char('w') ); \n" +
        "do test.assert( char('z') >= char('z') ); \n"
        ;

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

/**
 * Tests some random operations, and alow the operator+ for strings.
 */
BOOST_AUTO_TEST_CASE( testOperators )
{
    string prog = string("program x {\n") +
        "var int i = 3\n" +
        "var char c = 'c'\n" +
        "var double d = 10.0*i\n"+
        "do test.assert( d == 30.0 )\n" +
        "var bool b = false\n"+
        "var string s=\"string\"\n"+
        "set b = b || b && true && false || true\n"+
        "try test.assertMsg( s == \"string\", \"Unexpected string:\'\" + s +\"' instead of 'string'\")\n"+
        "set s = \"  \" + s + \"  \"\n"+
        "try test.assertMsg( s == \"  string  \", \"Unexpected string:\'\" + s +\"' instead of '  string  '\")\n"+
        "set s = s + int(10)\n"+
        "try test.assertMsg( s == \"  string  10\", \"Unexpected string:\'\" + s +\"' instead of '  string  10'\")\n"+
        "set s = s + \" \" + false\n"+
        "do  test.assertMsg( s == \"  string  10 false\", \"Unexpected string:\'\" + s +\"' instead of '  string  10 false'\")\n"+
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

/**
 * Tests parsing multiple occurences of '[]' and '.' while indexing
 * into structs and sequences and any combination thereof.
 */
BOOST_AUTO_TEST_CASE( testDotsAndIndexes )
{
    Types()->addType( new SequenceTypeInfo<std::vector<std::vector<double> >,false >("matrix"));
    Types()->addType( new StructTypeInfo<AType,false>("astruct"));
    Types()->addType( new StructTypeInfo<CType,false>("cstruct"));
    Types()->addType( new SequenceTypeInfo<std::vector<CType>,false >("cstructv"));
    Types()->addType( new SequenceTypeInfo<std::vector<AType>,false >("astructv"));
    string prog = string("program x {\n") +
        "var matrix m = matrix(8,array(10))\n" + // 8 by 10 matrix
        "test.assertMsg(m.size == 8, \"Matrix column size is wrong.\")\n" +
        "test.assertMsg(m[0].size == 10, \"Matrix row size is wrong.\")\n" +
        "m[0][0] = 3.33\n" +
        "m[1][1] = 4.33\n" +
        "m[2][2] = 5.33\n" +
        "m[8][10] = 6.33\n" +
        "test.assertMsg(m[0][0] == 3.33, \"Matrix element assignment failed.\")\n"+
        "test.assertMsg(m[1][1] == 4.33, \"Matrix element assignment failed.\")\n"+
        "test.assertMsg(m[2][2] == 5.33, \"Matrix element assignment failed.\")\n"+
        "test.assertMsg(m[8][10] == 6.33, \"Matrix element assignment failed.\")\n"+
        "var matrix m2 = m;\n"
        "test.assertMsg(m2[0][0] == 3.33, \"Matrix assignment failed.\")\n"+
        "test.assertMsg(m2[1][1] == 4.33, \"Matrix assignment failed.\")\n"+
        "test.assertMsg(m2[2][2] == 5.33, \"Matrix assignment failed.\")\n"+
        "test.assertMsg(m2[8][10] == 6.33, \"Matrix assignment failed.\")\n"+
        "var cstructv structv = cstructv(3)\n"+ // vector<struct> of size 10
        "structv[1].a.b = 3.33\n"+
        "test.assertMsg(structv[1].a.b == 3.33, \"Sequence of struct element assignment failed.\")\n"+
        "structv[1].av[3].b = 4.33\n"+
        "test.assertMsg(structv[1].av[3].b == 4.33, \"Sequence of struct element assignment failed.\")\n"+
        "}";
    // execute
    executePrograms(prog);
}

/**
 * Tests converting double to float to int in several
 * directions.
 */
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

/**
 * Tests converting (unsigned) long long types to and from other types.
 */
BOOST_AUTO_TEST_CASE( testLongLong )
{
    string prog = string("program x {\n") +
        "var llong ll = 9223372036854775807ll\n" +
        "do test.assert( -(ll + 2) == 9223372036854775807ll )\n" +
        "var ullong ull = 18446744073709551615ull\n" +
        "do test.assert( ull + 1 == 0 )\n" +
        "var double d = ll\n" +
        "do test.assert( d == 9.223372036854775807e+18 )\n" +
        "set ll = 3.0f\n" +
        "do test.assert( ll == 3ll )\n" +
        "set ll = -1.0\n" +
        "do test.assert( ll == -1ll )\n" +
        "set ll = int(-1000)\n" +
        "do test.assert( ll == -1000ll )\n" +
        "set ull = int(1000)\n" +
        "do test.assert( ull == 1000ull )\n" +
        "set ull = uint(1000)\n" +
        "do test.assert( ull == 1000ull )\n" +
        "set ull = llong(12345)\n" +
        "do test.assert( ull == 12345ull )\n" +
        "}";
    // execute
    executePrograms(prog);
}

/**
 * Tests hexadecimals
 */
BOOST_AUTO_TEST_CASE( testHex )
{
    string prog = string("program x {\n") +
        "var uint i = 0xabc\n" +
        "test.assert( i == 0xabc )\n"+
        "test.assert( i == 2748 )\n"+
        "i = 0Xcba\n" +
        "test.assert( i == 0Xcba )\n"+
        "test.assert( i == 3258 )\n"+
        "i = 0XABC\n" +
        "test.assert( i == 0XABC )\n"+
        "test.assert( i == 2748 )\n"+
        "i = 0xCBA\n" +
        "test.assert( i == 0xCBA )\n"+
        "test.assert( i == 3258 )\n"+
        "}";
    // execute
    executePrograms(prog);
}

/**
 * Tests reading/writing property(bags) in scripting.
 */
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

    pb.value().addProperty( pd1 );
    pb.value().addProperty( pd3 );
    pb.value().addProperty( pb ); // yep, recursive !

    tc->properties()->addProperty( pd1 );
    tc->properties()->addProperty( pd2 );
    tc->properties()->addProperty( pd3 );
    tc->properties()->addProperty( pb );
    tc->properties()->addProperty( pv );

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
        "do test.assert( 1 - 1 + 5 == 5 )\n" +  // not: -5
        "var int a,b,c;\n" +
        " a = b = c = 3;\n" +  // not 0,0,3
        "do test.assertEqual( a, 3 )\n" +
        "do test.assertEqual( b, 3 )\n" +
        "do test.assertEqual( c, 3 )\n" +
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
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("WriteSuccess") );
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("WriteFailure") );
    BOOST_CHECK (GlobalsRepository::Instance()->getValue("NotConnected") );
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
    BOOST_CHECK( sa->unloadProgram( (*pg_list.begin())->getName() ) );
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
            BOOST_REQUIRE_MESSAGE( false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_REQUIRE_MESSAGE( false, "Parser returned no state machines to execute." );
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
