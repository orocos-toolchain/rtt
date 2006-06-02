/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:41 CET 2006  template_factory_test.cpp 

                        template_factory_test.cpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#include "template_factory_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/FunctionGraph.hpp>
#include <execution/TemplateFactories.hpp>
#include <corelib/DataSourceAdaptor.hpp>
#include <execution/DataSourceGenerator.hpp>

#include <corelib/SimulationActivity.hpp>
#include <corelib/SimulationThread.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
using namespace ORO_Geometry;
#endif

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( Template_FactoryTest );


void 
Template_FactoryTest::setUp()
{
    tc =  new TaskContext( "root" );
    tc->methods()->registerObject("methods", this->createMethodFactory() );
    tc->methods()->registerObject("umethods", this->createUserMethodFactory() );
    tc->commands()->registerObject("commands", this->createCommandFactory() );
    tc->datasources()->registerObject("data", this->createDataSourceFactory() );
    tc->events()->addEvent("FloatEvent", &t_event_float);
    tsim = new SimulationActivity(0.001, tc->engine() );
    event_proc = new EventProcessor();
}


void 
Template_FactoryTest::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tsim->stop();
    SimulationThread::Instance()->stop();
    delete tc;
    delete tsim;
    delete event_proc;
}

bool Template_FactoryTest::assertBool( bool b) { 
    return b;
}

MethodFactoryInterface* Template_FactoryTest::createMethodFactory()
{
    TemplateMethodFactory< Template_FactoryTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &Template_FactoryTest::assertBool, "assert","b","bd") );

    dat->add( "m0", method( &Template_FactoryTest::m0, "M0") );
    dat->add( "m1", method( &Template_FactoryTest::m1, "M1","a","ad") );
    dat->add( "m2", method( &Template_FactoryTest::m2, "M2","a","ad","a","ad") );
    dat->add( "m3", method( &Template_FactoryTest::m3, "M3","a","ad","a","ad","a","ad") );
    dat->add( "m4", method( &Template_FactoryTest::m4, "M4","a","ad","a","ad","a","ad","a","ad") );
    return dat;
}

MethodFactoryInterface* Template_FactoryTest::createUserMethodFactory()
{
    TemplateMethodFactory< Template_FactoryTest >* dat =
        newMethodFactory( this );

    dat->add( "umd", method( &Template_FactoryTest::umd, "umd", "D0", "D0") );
    dat->add( "umv", method( &Template_FactoryTest::umv, "umv", "V0", "V0") );
    dat->add( "umcv", method( &Template_FactoryTest::umcv, "umv", "V0", "V0") );
    dat->add( "umcrv", method( &Template_FactoryTest::umcrv, "umv", "V0", "V0") );
    dat->add( "umrv", method( &Template_FactoryTest::umrv, "umv", "V0", "V0") );
    return dat;
}

DataSourceFactoryInterface* Template_FactoryTest::createDataSourceFactory()
{
    TemplateDataSourceFactory< Template_FactoryTest >* dat =
        newDataSourceFactory( this );

    dat->add( "d0", data( &Template_FactoryTest::d0, "d0") );
    dat->add( "d1", data( &Template_FactoryTest::d1, "d1","a","ad") );
    dat->add( "d2", data( &Template_FactoryTest::d2, "d2","a","ad","a","ad") );
    dat->add( "d3", data( &Template_FactoryTest::d3, "d3","a","ad","a","ad","a","ad") );
    dat->add( "d4", data( &Template_FactoryTest::d4, "d4","a","ad","a","ad","a","ad","a","ad") );

    return dat;
}

CommandFactoryInterface* Template_FactoryTest::createCommandFactory()
{
    TemplateCommandFactory< Template_FactoryTest >* dat =
        newCommandFactory( this );

    dat->add( "c00", command( &Template_FactoryTest::cd0, &Template_FactoryTest::cn0, "c0d") );
    dat->add( "c10", command( &Template_FactoryTest::cd1, &Template_FactoryTest::cn0, "c1d","a","ad") );
    dat->add( "c11", command( &Template_FactoryTest::cd1, &Template_FactoryTest::cn1, "c1d","a","ad") );
    dat->add( "c20", command( &Template_FactoryTest::cd2, &Template_FactoryTest::cn0, "c2d","a","ad","a","ad") );
    dat->add( "c21", command( &Template_FactoryTest::cd2, &Template_FactoryTest::cn1, "c2d","a","ad","a","ad") );
    dat->add( "c22", command( &Template_FactoryTest::cd2, &Template_FactoryTest::cn2, "c2d","a","ad","a","ad") );
    dat->add( "c30", command( &Template_FactoryTest::cd3, &Template_FactoryTest::cn0, "c3d","a","ad","a","ad","a","ad") );
    dat->add( "c31", command( &Template_FactoryTest::cd3, &Template_FactoryTest::cn1, "c3d","a","ad","a","ad","a","ad") );
    dat->add( "c33", command( &Template_FactoryTest::cd3, &Template_FactoryTest::cn3, "c3d","a","ad","a","ad","a","ad") );
    return dat;
}

void Template_FactoryTest::executePrograms(const Parser::ParsedPrograms& pg_list )
{
    tc->getExecutionEngine()->getProgramProcessor()->loadProgram( *pg_list.begin() );
    CPPUNIT_ASSERT( tsim->start() );
    CPPUNIT_ASSERT( (*pg_list.begin())->start() );
    CPPUNIT_ASSERT( SimulationThread::Instance()->run(1000) );
    CPPUNIT_ASSERT( (*pg_list.begin())->stop() );
    tsim->stop();
    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " Program error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        CPPUNIT_ASSERT_MESSAGE( errormsg.str(), false );
    }
}


void Template_FactoryTest::testMethods()
{
    string prog = std::string("program x { ")
        +" var double r = 0.0\n"
        +" set r = methods.m0\n"
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
        +"}";
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
    executePrograms( pg_list );
}

void Template_FactoryTest::testUserMethods()
{
    string prog = std::string("program x {\n")
#ifdef OROPKG_GEOMETRY
        +" var double6d din = double6d(0.0,1.0,2.0,3.0,4.0,5.0)\n"
        +" var double6d dout\n"
        +" set dout = umethods.umd(din)\n"
        +" do methods.assert( din == dout )\n"
#endif
        +" var array vin = array(4)\n"
        +" for (var int i=0; i !=4; set i = i+1 )\n"
        +"      set vin[i] = 1.0*i\n"
        +" var array vout1\n"
        +" var array vout2\n"
        +" var array vout3\n"
        +" var array vout4\n"
        //+" set vout1 = umethods.umv(vin)\n"
        //+" do methods.assert( vin == vout1 )\n"
        //+" set vout2 = umethods.umcv(vin)\n"
        //+" do methods.assert( vin == vout2 )\n"
        +" set vout3 = umethods.umcrv(vin)\n"
        +" do methods.assert( vin == vout3 )\n"
        //+" set vout4 = umethods.umrv(vin)\n"
        //+" do methods.assert( vin == vout4 )\n"
        +"}";
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
    executePrograms( pg_list );
}

void Template_FactoryTest::testData()
{
    string prog = string("program x { ")
        +" var double r = 0.0\n"
        +" set r = data.d0\n"
        +" set r = data.d0()\n"
        +" do methods.assert( r == 1.0 )\n"
        +" set r = data.d1(1)\n"
        +" do methods.assert( r == 2.0 )\n"
        +" set r = data.d2(1, 1.0)\n"
        +" do methods.assert( r == 3.0 )\n"
        +" set r = data.d3(1, 1.0, true)\n"
        +" do methods.assert( r == 4.0 )\n"
        +" set r = data.d4(1, 1.0, true, \"hello\")\n"
        +" do methods.assert( r == 5.0 )\n"
        +"}";
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
        executePrograms( pg_list );
}

void Template_FactoryTest::testCommands()
{
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
        +"}";
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
        executePrograms( pg_list );
}

void Template_FactoryTest::testManual()
{
    // test manual invocation of factory methods by users:

    // C++ POD :
    vector<DataSourceBase::shared_ptr> res;
    res = GenerateDataSource()( 1 );
    CPPUNIT_ASSERT( res.size() == 1 );
    res = GenerateDataSource()(1, 1.0 );
    CPPUNIT_ASSERT( res.size() == 2 );
    res = GenerateDataSource()(1, 1.0, true );
    CPPUNIT_ASSERT( res.size() == 3 );
    res = GenerateDataSource()(1, 1.0, true, "c" );
    CPPUNIT_ASSERT( res.size() == 4 );

    // C++ DataSources :
    DataSourceBase::shared_ptr empty1;
    DataSource<double>::shared_ptr empty2;
    AssignableDataSource<bool>::shared_ptr empty3;
    ValueDataSource<string>::shared_ptr empty4;

    res = GenerateDataSource()( empty1.get() );
    CPPUNIT_ASSERT( res.size() == 1 );
    res = GenerateDataSource()( empty1.get(), empty2.get() );
    CPPUNIT_ASSERT( res.size() == 2 );
    res = GenerateDataSource()( empty1.get(), empty2.get(), empty3.get() );
    CPPUNIT_ASSERT( res.size() == 3 );
    res = GenerateDataSource()( empty1.get(), empty2.get(), empty3.get(), empty4.get() );
    CPPUNIT_ASSERT( res.size() == 4 );
}


int Template_FactoryTest::float_listener(float a, float b)
{
    Logger::log() << Logger::Debug << "float_listener "<< a<<", "<<b<<Logger::endl;
    float_sum += a + b;
    return 1;
}

int Template_FactoryTest::float_completer(float a, float b)
{
    Logger::log() << Logger::Debug << "float_completer "<< a<<", "<<b<<Logger::endl;
    float_sub -= (a + b);
    return 1; // ignored...
}


void Template_FactoryTest::testEventC()
{
    float_sum = 0;
    float_sub = 0;
    // use CompletionProcessor for completer
    CompletionProcessor::Instance()->stop();
    ConnectionC cc = tc->events()->setupConnection("FloatEvent");
    cc.callback( this, &Template_FactoryTest::float_listener );
    cc.callback( this, &Template_FactoryTest::float_completer, CompletionProcessor::Instance()->getEventProcessor() );
    Handle h = cc.handle();

    h.connect();

    EventC mevent = tc->events()->setupEmit("FloatEvent").argC(float(1.0)).argC(float(4.0));
    mevent.emit();
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );
    CPPUNIT_ASSERT_EQUAL( float(0.0),  float_sub );

    float a = 10.0, b = 5.0;
    mevent = tc->events()->setupEmit("FloatEvent").arg(a).arg(b);
    mevent.emit();
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );
    CPPUNIT_ASSERT_EQUAL( float(0.0),  float_sub );

    CompletionProcessor::Instance()->start();
    
    h.disconnect();
    float_sum = 0;
    float_sub = 0;

    // use event processor
    event_proc->initialize();

    cc = tc->events()->setupConnection("FloatEvent").callback( this, &Template_FactoryTest::float_listener);
    cc.callback( this, &Template_FactoryTest::float_completer, event_proc, ORO_CoreLib::EventProcessor::OnlyLast );
    h = cc.handle();

    h.connect();

    // simulate overrun :
    mevent = tc->events()->setupEmit("FloatEvent").argC(float(1.0)).argC(float(4.0));
    mevent.emit();
    CPPUNIT_ASSERT_EQUAL( float(5.0), float_sum );

    mevent = tc->events()->setupEmit("FloatEvent").arg(a).arg(b);
    mevent.emit();
    CPPUNIT_ASSERT_EQUAL( float(20.0), float_sum );

    event_proc->step();
    event_proc->finalize();
    // asyn handlers should reach only last total.
    CPPUNIT_ASSERT_EQUAL( float(-15.0), float_sub );
    h.disconnect();
}

