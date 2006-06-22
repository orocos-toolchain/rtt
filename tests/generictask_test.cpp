
#include "generictask_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/FunctionGraph.hpp>
#include <execution/TemplateFactories.hpp>
#include <execution/Ports.hpp>

#include <corelib/SimulationActivity.hpp>
#include <corelib/SimulationThread.hpp>

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
    tc->methodFactory.registerObject("methods", this->createMethodFactory() );
    tc->commandFactory.registerObject("commands", this->createCommandFactory() );
    tc->dataFactory.registerObject("data", this->createDataSourceFactory() );
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

MethodFactoryInterface* Generic_TaskTest::createMethodFactory()
{
    TemplateMethodFactory< Generic_TaskTest >* dat =
        newMethodFactory( this );

    dat->add( "assert", method( &Generic_TaskTest::assertBool, "assert","b","bd") );

    dat->add( "m0", method( &Generic_TaskTest::m0, "M0") );
    dat->add( "m1", method( &Generic_TaskTest::m1, "M1","a","ad") );
    dat->add( "m2", method( &Generic_TaskTest::m2, "M2","a","ad","a","ad") );
    dat->add( "m3", method( &Generic_TaskTest::m3, "M3","a","ad","a","ad","a","ad") );
    dat->add( "m4", method( &Generic_TaskTest::m4, "M4","a","ad","a","ad","a","ad","a","ad") );
    return dat;
}

DataSourceFactoryInterface* Generic_TaskTest::createDataSourceFactory()
{
    TemplateDataSourceFactory< Generic_TaskTest >* dat =
        newDataSourceFactory( this );

    dat->add( "d0", data( &Generic_TaskTest::d0, "d0") );
    dat->add( "d1", data( &Generic_TaskTest::d1, "d1","a","ad") );
    dat->add( "d2", data( &Generic_TaskTest::d2, "d2","a","ad","a","ad") );
    dat->add( "d3", data( &Generic_TaskTest::d3, "d3","a","ad","a","ad","a","ad") );
    dat->add( "d4", data( &Generic_TaskTest::d4, "d4","a","ad","a","ad","a","ad","a","ad") );

    return dat;
}

CommandFactoryInterface* Generic_TaskTest::createCommandFactory()
{
    TemplateCommandFactory< Generic_TaskTest >* dat =
        newCommandFactory( this );

    dat->add( "c00", command( &Generic_TaskTest::cd0, &Generic_TaskTest::cn0, "c0d") );
    dat->add( "c10", command( &Generic_TaskTest::cd1, &Generic_TaskTest::cn0, "c1d","a","ad") );
    dat->add( "c11", command( &Generic_TaskTest::cd1, &Generic_TaskTest::cn1, "c1d","a","ad") );
    dat->add( "c20", command( &Generic_TaskTest::cd2, &Generic_TaskTest::cn0, "c2d","a","ad","a","ad") );
    dat->add( "c21", command( &Generic_TaskTest::cd2, &Generic_TaskTest::cn1, "c2d","a","ad","a","ad") );
    dat->add( "c22", command( &Generic_TaskTest::cd2, &Generic_TaskTest::cn2, "c2d","a","ad","a","ad") );
    dat->add( "c30", command( &Generic_TaskTest::cd3, &Generic_TaskTest::cn0, "c3d","a","ad","a","ad","a","ad") );
    dat->add( "c31", command( &Generic_TaskTest::cd3, &Generic_TaskTest::cn1, "c3d","a","ad","a","ad","a","ad") );
    dat->add( "c33", command( &Generic_TaskTest::cd3, &Generic_TaskTest::cn3, "c3d","a","ad","a","ad","a","ad") );
    dat->add( "c40", command( &Generic_TaskTest::cd4, &Generic_TaskTest::cn0, "c4d","a","ad","a","ad","a","ad","a","ad") );
    dat->add( "c41", command( &Generic_TaskTest::cd4, &Generic_TaskTest::cn1, "c4d","a","ad","a","ad","a","ad","a","ad") );
    dat->add( "c44", command( &Generic_TaskTest::cd4, &Generic_TaskTest::cn4, "c4d","a","ad","a","ad","a","ad","a","ad") );
    return dat;
}

void Generic_TaskTest::testMethodsC()
{
    MethodC mc;
    double r = 0.0;
    mc = tc->methods()->create("methods", "m0").ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -1.0 );

    mc = tc->methods()->create("methods", "m2").argC(1).argC(1.0).ret( r );
    CPPUNIT_ASSERT( mc.execute() );
    CPPUNIT_ASSERT( r == -3.0 );

    mc = tc->methods()->create("methods", "m3").ret( r ).argC(1).argC(1.0).argC(true);
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
    CommandC cc = tc->commands()->create("commands","c00");
    CommandC c20 = tc->commands()->create("commands","c20").argC(1).argC(1.0);
    CommandC c32 = tc->commands()->create("commands","c31").argC(1).argC(1.0).argC('a');
    CommandC c33 = tc->commands()->create("commands","c33").argC(1).argC(1.0).argC('a');
    CommandC c44 = tc->commands()->create("commands","c44").argC(1).argC(1.0).argC('a').argC(true);
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
    tc->attributes()->addProperty( &d1);
#ifdef OROPKG_GEOMETRY
    using namespace ORO_Geometry;
    Property<Frame> f1("f1", "descf1", Frame::Identity());
    tc->attributes()->addProperty( &f1 );
#endif

#if 0    
    CPPUNIT_ASSERT(tc->writeProperties("Generic_TaskTest_Properties.cpf"));
    CPPUNIT_ASSERT( tc->readProperties("Generic_TaskTest_Properties.cpf"));
#endif
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
