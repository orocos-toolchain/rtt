#ifndef PROGRAMTEST_H
#define PROGRAMTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/GlobalFactory.hpp>
#include <execution/Factories.hpp>
#include <string>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class ProgramTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ProgramTest );
    CPPUNIT_TEST( testEmptyProgram );
    CPPUNIT_TEST( testReturnProgram );
    CPPUNIT_TEST( testTypes );
    CPPUNIT_TEST( testOperators );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    GlobalFactory factory;
    Processor processor;
    MethodFactoryInterface* createMethodFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
public:

    void setUp();
    void tearDown();

    void testEmptyProgram();
    void testReturnProgram();
    void testTypes();
    void testOperators();
};

#endif
