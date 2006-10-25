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
#include <TaskBrowser.hpp>
#include <GenericTaskContext.hpp>
#include <ProgramProcessor.hpp>
#include <StateMachineProcessor.hpp>
#ifdef OROPKG_GEOMETRY
#include <geometry/GeometryToolkit.hpp>
#include <geometry/frames_io.h>
using namespace ORO_Geometry;
#endif
using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TypesTest );


void 
TypesTest::setUp()
{
    tc =  new GenericTaskContext( "root" );
    tc->addObject( this->createMethodFactory() );
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


    TaskObject* TypesTest::createMethodFactory()
    {
        TaskObject* to = new TaskObject("test");
        to->methods()->addMethod( method("assert", &TypesTest::assertBool, this),
                                  "Assert", "bool", "");
        to->methods()->addMethod( method("assertMsg", &TypesTest::assertMsg, this),
                                     "Assert message", "bool", "", "text", "text" );
#ifdef OROPKG_GEOMETRY
        to->methods()->addMethod( method("equalFrames", &TypesTest::equalFrames, this),
                                     "Assert equal frames", "f1", "", "f2", "" );
        to->methods()->addMethod( method("equalVectors", &TypesTest::equalVectors, this),
                                     "Assert equal vectors", "v1", "", "v2", "" );
        to->methods()->addMethod( method("equalVFrames", &TypesTest::equalVFrames, this),
                                     "Assert equal frames", "f1", "", "f2", "" );
        to->methods()->addMethod( method("equalVVectors", &TypesTest::equalVVectors, this),
                                     "Assert equal vectors", "v1", "", "v2", "" );
        to->methods()->addMethod( method("equalWrench", &TypesTest::equalWrench, this),
                                     "Assert equal wrench", "v1", "", "v2", "", "v3","","v4","" );
#endif
        return to;
    }

#ifdef OROPKG_GEOMETRY

bool TypesTest::equalWrench(const Wrench f1, Wrench& f2, const Wrench& f3, Wrench f4)
{
    //cout <<"equalWrench("<< f1 <<", "<< f2 <<", "<< f3 <<", "<< f4 <<")"<<endl;
    return f1 == f2 && f2 == f3 && f3 == f4 && f1.force != Vector::Zero() && f1.torque != Vector::Zero();
}

// Test passing const and by reference
bool TypesTest::equalFrames(const Frame f1, Frame& f2)
{
    //cout <<"equalFrames("<< f1 <<", "<< f2 <<")"<<endl;
    return f1 == f2 && f1.p != Vector::Zero() && f1.M != Rotation::Identity();
}

bool TypesTest::equalVectors(const Vector f1, Vector& f2)
{
    //cout <<"equalVectors("<< f1 <<", "<< f2 <<")"<<endl;
    return f1 == f2 && f1 != Vector::Zero();
}

// Test passing const& and by value
bool TypesTest::equalVFrames(const Frame& f1, Frame f2)
{
    //cout <<"equalVFrames("<< f1 <<", "<< f2 <<")"<<endl;
    return f1 == f2 && f1.p != Vector::Zero() && f1.M != Rotation::Identity();
}

bool TypesTest::equalVVectors(const Vector& f1, Vector f2)
{
    //cout <<"equalVVectors("<< f1 <<", "<< f2 <<")"<<endl;
    return f1 == f2 && f1 != Vector::Zero();
}
#endif

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
        "const int ic = i2\n" +
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
        "var vector v = vector(0.1,0.2,0.3)\n"+
        "var rotation r = rotation(0.3,0.2,0.1) \n"+
        "var frame f = frame(v,r) \n"+
        "const vector vc = vector(0.1,0.2,0.3)\n"+
        "const rotation rc = rotation(0.3,0.2,0.1)\n"+
        "const frame fc = frame(vc,rc)\n"+
        "do test.equalFrames(f, f)\n"+
        "do test.equalVectors(v, v)\n"+
        // 40
        "do test.equalVFrames(f, f)\n"+
        "do test.equalVVectors(v, v)\n"+
        "var wrench w = wrench(vector(1.0,2.0,3.0),vector(0.1,0.2,0.3))\n"+
        "do test.equalWrench(wrench(vector(1.0,2.0,3.0),vector(0.1,0.2,0.3)),w,wrench(vector(1.0,2.0,3.0),vector(0.1,0.2,0.3)),wrench(vector(1.0,2.0,3.0),vector(0.1,0.2,0.3)))\n"+
        "do test.equalFrames(fc, f)\n"+
        "do test.equalVectors(vc, v)\n"+
        "do test.equalVFrames(fc, f)\n"+
        "do test.equalVVectors(vc, v)\n"+
#endif
#ifndef ORO_EMBEDDED
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
#endif
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
#ifdef OROPKG_GEOMETRY
        "var vector v = vector(0.,0.,0.)\n"+
        "var rotation r = rotation(0.,0.,0.)\n"+
        "do test.assert( Frame == Frame ) \n"+
        "do test.assert( Rotation == rotation(0., 45., 60. ) ) \n"+
        // -> = 20
        "set v = vector(0.,2.,4.)\n"+
        "set r = rotation(0.,45.,60.) \n"+
        "set Frame = frame( v, r )\n"+
        "set Rotation = r\n"+
        "do test.assert( Frame.p.x == frame(v,r).p.x ) \n"+
        "do test.assert( Rotation.roll == rotation(0., 45., 60. ).roll ) \n"+
        "do test.assert( Frame.p.x == Collection.Frame.p.x ) \n"+
        "do test.assert( Collection.Rotation.pitch == rotation(0., 45., 60. ).pitch ) \n"+
#endif
        "}";

    Property<PropertyBag> pb("Collection","");
    Property<double> pd1("Double1","",1.234);
    Property<double> pd2("Double2","",2.234);
    Property<double> pd3("Double3","",3.234);

    Property< std::vector<double> > pv("V","",std::vector<double>(4, 4.0) );
#ifdef OROPKG_GEOMETRY
    Property<Frame> pf("Frame","", Frame::Identity() );
    Property<Rotation> pr("Rotation","", Rotation::RPY(0.0, 45.0, 60.0) );
    tc->properties()->addProperty( &pf );
    tc->properties()->addProperty( &pr );
    pb.value().add( &pf );
    pb.value().add( &pr );
#endif

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

    

