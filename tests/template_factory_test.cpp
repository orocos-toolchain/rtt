
#include "template_factory_test.hpp"
#include <unistd.h>
#include <iostream>
#include <execution/ProgramGraph.hpp>
#include <execution/TemplateFactories.hpp>
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
    tc =  new TaskContext( "root", &processor);
    tc->methodFactory.registerObject("methods", this->createMethodFactory() );
    tc->commandFactory.registerObject("commands", this->createCommandFactory() );
    tc->dataFactory.registerObject("data", this->createDataSourceFactory() );
}


void 
Template_FactoryTest::tearDown()
{
    if ( tc->getPeer("programs") )
        delete tc->getPeer("programs");
    delete tc;
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
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, tc);
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
    CPPUNIT_ASSERT( (*pg_list.begin())->executeAll() );
    delete *pg_list.begin();
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
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, tc);
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
    CPPUNIT_ASSERT( (*pg_list.begin())->executeAll() );
    delete *pg_list.begin();
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
    stringstream progs(prog);
    std::vector<ProgramGraph*> pg_list;
    try {
        pg_list = parser.parseProgram( progs, tc);
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
    CPPUNIT_ASSERT( (*pg_list.begin())->executeAll() );
    delete *pg_list.begin();
}

