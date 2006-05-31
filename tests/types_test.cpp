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
#include <execution/FunctionGraph.hpp>
#include <execution/TemplateFactories.hpp>
#include <corelib/SimulationActivity.hpp>
#include <corelib/SimulationThread.hpp>
#ifdef OROPKG_GEOMETRY
#include <geometry/GeometryToolkit.hpp>
using namespace ORO_Geometry;
#endif
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TypesTest );


void 
TypesTest::setUp()
{
    tc =  new TaskContext( "root" );
    tc->methodFactory.registerObject("test", this->createMethodFactory() );
    tsim = new SimulationActivity( 0.001, tc->engine() );

#ifdef OROPKG_GEOMETRY
    Toolkit::Import( GeometryToolkit );
#endif
}


void 
TypesTest::tearDown()
{
    tsim->stop();
    SimulationThread::Instance()->stop();
    delete tc;
    delete tsim;
}


bool TypesTest::assertBool( bool b) { 
    return b;
}
bool TypesTest::assertMsg( bool b, const std::string& msg) {
    return b;
}


    MethodFactoryInterface* TypesTest::createMethodFactory()
    {
        // Add the data of the EE:
        TemplateMethodFactory< TypesTest >* dat =
            newMethodFactory( this );

        dat->add( "assert", method( &TypesTest::assertBool,
                                  "Assert", "bool", "") );
        dat->add( "assertMsg", method( &TypesTest::assertMsg,
                                     "Assert message", "bool", "", "text", "text" ) );
#ifdef OROPKG_GEOMETRY
        dat->add( "equalFrames", method( &TypesTest::equalFrames,
                                     "Assert equal frames", "f1", "", "f2", "" ) );
        dat->add( "equalVectors", method( &TypesTest::equalVectors,
                                     "Assert equal vectors", "v1", "", "v2", "" ) );
#endif
        return dat;
    }

#ifdef OROPKG_GEOMETRY
bool TypesTest::equalFrames(const Frame& f1, Frame& f2)
{
    return f1 == f2;
}

bool TypesTest::equalVectors(const Vector& f1, Vector& f2)
{
    return f1 == f2;
}
#endif

void TypesTest::testEmptyProgram()
{
    string prog = "";
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc );
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT( false );
        }
    if ( !pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
}

void TypesTest::testReturnProgram()
{
    string prog = "program x { return \n }";
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc);
    }
    catch( const file_parse_exception& exc )
        {
            CPPUNIT_ASSERT_MESSAGE(exc.what(), false );
        }
    if ( pg_list.empty() )
        {
            CPPUNIT_ASSERT( false );
        }
    // execute
    executePrograms(pg_list);
}

void TypesTest::testTypes()
{
    // XXX
    // for some reason, we can not compare the double6D's one fails
    // to parse, the others assert false, thus inequality.
    string prog = string("program x {\n") +
        "var int i = -1, j = 10, k; set k = 20\n" +
        "do test.assert( i == -1 ) ; do test.assert( j == 10 ); do test.assert(k == 20)\n" +
        "var double d = 10.0\n"+
        "do test.assert( d == 10.0 )\n" +
        "var bool b = false\n"+
        "do test.assert( b == false )\n" +
        "var string s=\"string\"\n"+
        "do test.assert( s == \"string\" )\n" +
#ifdef OROPKG_GEOMETRY
        "var double6d d6 = double6d(0.1,0.2,0.3,0.4,0.5,0.6)\n"+
        // 10
//         "do test.assert( d6 == d6 )\n" +
//         "do test.assert( d6 == double6d(0.1,0.2,0.3,0.4,0.5,0.6) )\n" + // fails to parse
        "var double6d d6_2 = double6d(0.01)\n"+
//         "do test.assert( d6_2 == d6_2 )\n" +
//         "do test.assert( double6d(0.01) == double6d(0.01) )\n" +
//         "do test.assert( d6_2 == double6d(0.01) )\n" +
#endif
        "const int ic = i\n" +
        "do test.assert( ic == 0 )\n" + // i was null at parse time !
        "const double dc = 10.0\n"+     // evaluate 10.0 at parse time
        "do test.assert( dc == 10.0 )\n" +
        "const bool bc = true && false\n"+
        "do test.assert( bc == false )\n" +
        "const string sc= \"hello\"\n"+
        "do test.assert( sc == \"hello\" )\n" +
#ifdef OROPKG_GEOMETRY
        "const double6d d6c = double6d(0.1,0.2,0.3,0.4,0.5,0.6)\n" +
        // 20
//         "do test.assert( d6c == d6 )\n" +
        "set d6[0]=1.0\n"+
        "do test.assert( d6[0] == 1.0 )\n" +
        "set d6[1]=d6[0]\n"+
        "do test.assert( d6[1] == 1.0 )\n" +
        "set d6[2]=d6[0]\n"+
        "do test.assert( d6[2] == 1.0 )\n" +
        "set d6[3]=d6[0]\n"+
        "do test.assert( d6[3] == 1.0 )\n" +
        "set d6[4]=d6[0]\n"+
        "do test.assert( d6[4] == 1.0 )\n" +
        // 30
        "set d6[5]=d6[0]\n"+
        "do test.assert( d6[5] == 1.0 )\n" +
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.) \n"+
        "var frame f = frame(v,r) \n"+
        "const vector vc = v\n"+
        "const rotation rc = r\n"+
        "const frame fc = f\n"+
        "do test.equalFrames(fc, f)\n"+
        "do test.equalVectors(vc, v)\n"+
#endif
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
        "do test.assert( str[10] == '\\0' )\n"+
        "}";
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
    // execute
    executePrograms(pg_list);
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
#ifdef OROPKG_GEOMETRY
        "var double6d d6 = double6d(0.,1.,2.,3.,4.,5.)\n"+
        "var double6d d6_2 = double6d(0.0)\n"+
        "set d = d+1.0-1.0-2.0*1.0/2.0*0.0/i \n"+
#endif
//         "do test.assert( d == 10.0 )\n" +
        "set b = b\n or\n b\n and\n true\n && false\n || true\n"+
        "do test.assert( b == false )\n" +
//         "set s = s+\"abc\"\n"+
#ifdef OROPKG_GEOMETRY
        "set d6 = (d6 + double6d(2.) ) * d6 - d6\n"+
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.) \n"+
        "var frame f = frame(v,r) \n"+
#endif
        "}";
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
    // execute
    executePrograms(pg_list);
}

void TypesTest::executePrograms(const Parser::ParsedPrograms& pg_list )
{
    tc->getExecutionEngine()->getProgramProcessor()->loadProgram( *pg_list.begin() );
    tsim->start();
    CPPUNIT_ASSERT( (*pg_list.begin())->start() );
    SimulationThread::Instance()->start(1000);
    sleep(1);
    tsim->stop();
    SimulationThread::Instance()->stop();
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
    CPPUNIT_ASSERT( (*pg_list.begin())->stop() );
    tc->engine()->programs()->unloadProgram( (*pg_list.begin())->getName() );
}

void TypesTest::testProperties()
{
    string prog = string("program x {\n") +
        "do test.assert( task.Double1 == 1.234 )\n" +
        "do test.assert( task.Double2 == 2.234 )\n" +
        "do test.assert( task.Double3 == 3.234 )\n" +
        "do test.assert( task.Collection.Double1 == 1.234 )\n" +
        "do test.assert( task.Collection.Double3 == 3.234 )\n" +
        "do test.assert( task.Collection.Collection.Double1 == 1.234 )\n" +
        "do test.assert( task.Collection.Collection.Collection.Double3 == 3.234 )\n" +
        "set task.Double1 = 4.321\n" +
        "set task.Double2 = task.Double1\n" +
        // -> = 10
        "set task.Collection.Double3 = 0.3\n" +
        "do test.assert( task.Double1 == 4.321 )\n" +
        "do test.assert( task.Double2 == 4.321 )\n" +
        "do test.assert( task.Double3 == 0.3 )\n" +
        "set task.Collection.Collection.Collection.Double3 = 3.0\n" +
        "do test.assert( task.Double3 == 3.0 )\n" +
#ifdef OROPKG_GEOMETRY
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.)\n"+
        "do test.assert( task.Frame == task.Frame ) \n"+
        "do test.assert( task.Rotation == rotation(0., 45., 60. ) ) \n"+
        // -> = 20
        "set v = vector(0.,2.,4.)\n"+
        "set r = rotation(0.,45.,60.) \n"+
        "set task.Frame = frame( v, r )\n"+
        "set task.Rotation = r\n"+
        "do test.assert( task.Frame.p.x == frame(v,r).p.x ) \n"+
        "do test.assert( task.Rotation.roll == rotation(0., 45., 60. ).roll ) \n"+
        "do test.assert( task.Frame.p.x == task.Collection.Frame.p.x ) \n"+
        "do test.assert( task.Collection.Rotation.pitch == rotation(0., 45., 60. ).pitch ) \n"+
#endif
        "}";

    Property<PropertyBag> pb("Collection","");
    Property<double> pd1("Double1","",1.234);
    Property<double> pd2("Double2","",2.234);
    Property<double> pd3("Double3","",3.234);
#ifdef OROPKG_GEOMETRY
    Property<Frame> pf("Frame","", Frame::Identity() );
    Property<Rotation> pr("Rotation","", Rotation::RPY(0.0, 45.0, 60.0) );
    tc->attributeRepository.addProperty( &pf );
    tc->attributeRepository.addProperty( &pr );
    pb.value().add( &pf );
    pb.value().add( &pr );
#endif

    pb.value().add( &pd1 );
    pb.value().add( &pd3 );
    pb.value().add( &pb ); // yep, recursive !
        
    tc->attributeRepository.addProperty( &pd1 );
    tc->attributeRepository.addProperty( &pd2 );
    tc->attributeRepository.addProperty( &pd3 );
    tc->attributeRepository.addProperty( &pb );

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
    // execute
    executePrograms(pg_list);
    CPPUNIT_ASSERT_EQUAL( 4.321, pd1.get() );
    CPPUNIT_ASSERT_EQUAL( 3.0, pd3.get() );
}

    

