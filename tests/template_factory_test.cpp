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

#include <iostream>
#include <scripting/FunctionGraph.hpp>
#include <Method.hpp>
#include <Method.hpp>
#include <internal/DataSourceAdaptor.hpp>
#include <internal/DataSourceGenerator.hpp>
#include <scripting/ProgramProcessor.hpp>
#include <interface/ServiceProvider.hpp>
#include <internal/CompletionProcessor.hpp>

#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>
#include <interface/ServiceProvider.hpp>

#include <rtt-config.h>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

void
Template_FactoryTest::setUp()
{
    tc =  new TaskContext( "root" );
    tc->addService( this->createMethodFactory() );
    tc->addService( this->createUserMethodFactory() );
    tc->addService( this->createCommandFactory() );
    t_event_float = RTT::Event<int( float, float )>("FloatEvent");
    BOOST_CHECK( tc->events()->addEvent( &t_event_float, "Description","a1","d1", "a2", "d2" ) );
    tsim = new SimulationActivity(0.001, tc->engine() );
    event_proc = new EventProcessor();
    act.run(event_proc);
    SimulationThread::Instance()->stop();
}


void
Template_FactoryTest::tearDown()
{
//     if ( tc->getPeer("programs") )
//         delete tc->getPeer("programs");
    tsim->stop();
    act.stop();
    delete tc;
    delete tsim;
    delete event_proc;
}

bool Template_FactoryTest::assertBool( bool b) {
    return b;
}

ServiceProvider* Template_FactoryTest::createMethodFactory()
{
    ServiceProvider* dat = new ServiceProvider("methods");

    dat->addOperation("assert", &Template_FactoryTest::assertBool, this).doc("assert").arg("b", "bd");
    dat->addOperation("m0", &Template_FactoryTest::m0, this).doc("M0");
    dat->addOperation("m1", &Template_FactoryTest::m1, this).doc("M1").arg("a", "ad");
    dat->addOperation("m2", &Template_FactoryTest::m2, this).doc("M2").arg("a", "ad").arg("a", "ad");
    dat->addOperation("m3", &Template_FactoryTest::m3, this).doc("M3").arg("a", "ad").arg("a", "ad").arg("a", "ad");
    dat->addOperation("m4", &Template_FactoryTest::m4, this).doc("M4").arg("a", "ad").arg("a", "ad").arg("a", "ad").arg("a", "ad");
    return dat;
}

ServiceProvider* Template_FactoryTest::createUserMethodFactory()
{
    ServiceProvider* dat = new ServiceProvider("umethods");

    dat->addOperation("umd", &Template_FactoryTest::umd, this).doc("umd").arg("D0", "D0");
    dat->addOperation("umv", &Template_FactoryTest::umv, this).doc("umv").arg("V0", "V0");
    dat->addOperation("umcv", &Template_FactoryTest::umcv, this).doc("umv").arg("V0", "V0");
    dat->addOperation("umcrv", &Template_FactoryTest::umcrv, this).doc("umv").arg("V0", "V0");
    dat->addOperation("umrv", &Template_FactoryTest::umrv, this).doc("umv").arg("V0", "V0");
    return dat;
}

ServiceProvider* Template_FactoryTest::createCommandFactory()
{
    ServiceProvider* dat = new ServiceProvider("commands");

    dat->addOperation("c00", &Template_FactoryTest::cd0, this).doc("c0d");
addOperation("c00Done", &Template_FactoryTest::cn0, this).doc("Returns true when c00 is done.");
    dat->addOperation("c10", &Template_FactoryTest::cd1, this).doc("c1d").arg("a", "ad");
addOperation("c10Done", &Template_FactoryTest::cn0, this).doc("Returns true when c10 is done.").arg("a", "ad");
    dat->addOperation("c11", &Template_FactoryTest::cd1, this).doc("c1d").arg("a", "ad");
addOperation("c11Done", &Template_FactoryTest::cn1, this).doc("Returns true when c11 is done.").arg("a", "ad");
    dat->addOperation("c20", &Template_FactoryTest::cd2, this).doc("c2d").arg("a", "ad").arg("a", "ad");
addOperation("c20Done", &Template_FactoryTest::cn0, this).doc("Returns true when c20 is done.").arg("a", "ad").arg("a", "ad");
    dat->addOperation("c21", &Template_FactoryTest::cd2, this).doc("c2d").arg("a", "ad").arg("a", "ad");
addOperation("c21Done", &Template_FactoryTest::cn1, this).doc("Returns true when c21 is done.").arg("a", "ad").arg("a", "ad");
    dat->addOperation("c22", &Template_FactoryTest::cd2, this).doc("c2d").arg("a", "ad").arg("a", "ad");
addOperation("c22Done", &Template_FactoryTest::cn2, this).doc("Returns true when c22 is done.").arg("a", "ad").arg("a", "ad");
    dat->addOperation("c30", &Template_FactoryTest::cd3, this).doc("c3d").arg("a", "ad").arg("a", "ad").arg("a", "ad");
addOperation("c30Done", &Template_FactoryTest::cn0, this).doc("Returns true when c30 is done.").arg("a", "ad").arg("a", "ad").arg("a", "ad");
    dat->addOperation("c31", &Template_FactoryTest::cd3, this).doc("c3d").arg("a", "ad").arg("a", "ad").arg("a", "ad");
addOperation("c31Done", &Template_FactoryTest::cn1, this).doc("Returns true when c31 is done.").arg("a", "ad").arg("a", "ad").arg("a", "ad");
    dat->addOperation("c33", &Template_FactoryTest::cd3, this).doc("c3d").arg("a", "ad").arg("a", "ad").arg("a", "ad");
addOperation("c33Done", &Template_FactoryTest::cn3, this).doc("Returns true when c33 is done.").arg("a", "ad").arg("a", "ad").arg("a", "ad");
    return dat;
}

void Template_FactoryTest::executePrograms(const Parser::ParsedPrograms& pg_list )
{
    tc->engine()->programs()->loadProgram( *pg_list.begin() );
    BOOST_CHECK( tsim->start() );
    BOOST_CHECK( (*pg_list.begin())->start() );
    BOOST_CHECK( SimulationThread::Instance()->run(1000) );
    BOOST_CHECK( (*pg_list.begin())->stop() );
    tsim->stop();
    if ( (*pg_list.begin())->inError() ) {
        stringstream errormsg;
        errormsg << " Program error on line " << (*pg_list.begin())->getLineNumber() <<"."<<endl;
        BOOST_CHECK_MESSAGE( false ,errormsg.str());
    }
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

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  Template_FactoryTestSuite,  Template_FactoryTest )

BOOST_AUTO_TEST_CASE( testMethods)
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
            BOOST_CHECK_MESSAGE(false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_CHECK( false );
        }
    // execute
    executePrograms( pg_list );
}
#if !defined(OROCOS_TARGET_WIN32)
BOOST_AUTO_TEST_CASE( testUserMethods)
{
    string prog = std::string("program x {\n")
        +" var array vin = array(4)\n"
        +" for (var int i=0; i !=4; set i = i+1 )\n"
        +"      set vin[i] = 1.0*i\n"
        +" var array vout1\n"
        +" var array vout2\n"
        +" var array vout3\n"
        +" var array vout4\n"
        +" set vout1 = umethods.umv(vin)\n"
        +" do methods.assert( vin == vout1 )\n"
        +" set vout2 = umethods.umcv(vin)\n"
        +" do methods.assert( vin == vout2 )\n"
        +" set vout3 = umethods.umcrv(vin)\n"
        +" do methods.assert( vin == vout3 )\n"
        +" set vout4 = umethods.umrv(vin)\n"
        +" do methods.assert( vin == vout4 )\n"
        +"}";
    Parser::ParsedPrograms pg_list;
    try {
        pg_list = parser.parseProgram( prog, tc);
    }
    catch( const file_parse_exception& exc )
        {
            BOOST_CHECK_MESSAGE( false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_CHECK( false );
        }
    // execute
    executePrograms( pg_list );
}
#endif

BOOST_AUTO_TEST_CASE( testCommands)
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
            BOOST_CHECK_MESSAGE( false , exc.what());
        }
    if ( pg_list.empty() )
        {
            BOOST_CHECK( false );
        }
    // execute
        executePrograms( pg_list );
}

BOOST_AUTO_TEST_CASE( testManual)
{
    // test manual invocation of factory methods by users:

    // C++ POD :
    vector<DataSourceBase::shared_ptr> res;
    res = GenerateDataSource()( 1 );
    BOOST_CHECK( res.size() == 1 );
    res = GenerateDataSource()(1, 1.0 );
    BOOST_CHECK( res.size() == 2 );
    res = GenerateDataSource()(1, 1.0, true );
    BOOST_CHECK( res.size() == 3 );
    res = GenerateDataSource()(1, 1.0, true, "c" );
    BOOST_CHECK( res.size() == 4 );

    // C++ DataSources :
    DataSourceBase::shared_ptr empty1;
    DataSource<double>::shared_ptr empty2;
    AssignableDataSource<bool>::shared_ptr empty3;
    ValueDataSource<string>::shared_ptr empty4;

    res = GenerateDataSource()( empty1.get() );
    BOOST_CHECK( res.size() == 1 );
    res = GenerateDataSource()( empty1.get(), empty2.get() );
    BOOST_CHECK( res.size() == 2 );
    res = GenerateDataSource()( empty1.get(), empty2.get(), empty3.get() );
    BOOST_CHECK( res.size() == 3 );
    res = GenerateDataSource()( empty1.get(), empty2.get(), empty3.get(), empty4.get() );
    BOOST_CHECK( res.size() == 4 );
}

BOOST_AUTO_TEST_CASE( testEventC)
{
    float_sum = 0;
    float_sub = 0;
    // use CompletionProcessor for completer
    CompletionProcessor::Instance()->stop();
    ConnectionC cc = tc->events()->setupConnection("FloatEvent");
    cc.callback( this, &Template_FactoryTest::float_listener );
    Handle h1 = cc.handle();
    cc.callback( this, &Template_FactoryTest::float_completer, CompletionProcessor::Instance() );
    Handle h2 = cc.handle();

    BOOST_CHECK( h1.connect() );
    BOOST_CHECK( h2.connect() );

    // first test ConnectionC callbacks.
    t_event_float(1.0, 4.0);
    BOOST_CHECK_EQUAL( float(5.0), float_sum );
    BOOST_CHECK_EQUAL( float(0.0),  float_sub );
    float_sum = float_sub = 0.0;

    // OK, now test if the EventC emission works.
    EventC mevent;
    BOOST_CHECK_NO_THROW( mevent = tc->events()->setupEmit("FloatEvent").argC(float(1.0)).argC(float(4.0)) );
    BOOST_CHECK( mevent.ready() );
    mevent.emit();
    BOOST_CHECK_EQUAL( float(5.0), float_sum );
    BOOST_CHECK_EQUAL( float(0.0),  float_sub );

    float a = 10.0, b = 5.0;
    BOOST_CHECK_NO_THROW( mevent = tc->events()->setupEmit("FloatEvent").arg(a).arg(b) );
    mevent.emit();
    BOOST_CHECK_EQUAL( float(20.0), float_sum );
    BOOST_CHECK_EQUAL( float(0.0),  float_sub );

    CompletionProcessor::Instance()->start();

    h1.disconnect();
    h2.disconnect();
    float_sum = 0;
    float_sub = 0;

    // use event processor
    BOOST_CHECK(act.start());

    cc = tc->events()->setupConnection("FloatEvent").callback( this, &Template_FactoryTest::float_listener);
    h1 = cc.handle();
    cc.callback( this, &Template_FactoryTest::float_completer, event_proc, EventProcessor::OnlyLast );
    h2 = cc.handle();

    h1.connect();
    h2.connect();

    // simulate overrun :
    mevent = tc->events()->setupEmit("FloatEvent").argC(float(1.0)).argC(float(4.0));
    mevent.emit();
    BOOST_CHECK_EQUAL( float(5.0), float_sum );

    mevent = tc->events()->setupEmit("FloatEvent").arg(a).arg(b);
    mevent.emit();
    BOOST_CHECK_EQUAL( float(20.0), float_sum );

    event_proc->step();
    event_proc->finalize();
    // asyn handlers should reach only last total.
    BOOST_CHECK_EQUAL( float(-15.0), float_sub );
    h1.disconnect();
    h2.disconnect();
}

BOOST_AUTO_TEST_SUITE_END()
