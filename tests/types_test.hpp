#ifndef TYPESTEST_H
#define TYPESTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class TypesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TypesTest );
    CPPUNIT_TEST( testEmptyProgram );
    CPPUNIT_TEST( testReturnProgram );
    CPPUNIT_TEST( testTypes );
    CPPUNIT_TEST( testOperators );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext* tc;
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
