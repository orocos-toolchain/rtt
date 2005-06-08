#ifndef TEMPLATE_FACTORYTEST_H
#define TEMPLATE_FACTORYTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class Template_FactoryTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Template_FactoryTest );
    CPPUNIT_TEST( testMethods );
    CPPUNIT_TEST( testData );
    CPPUNIT_TEST( testCommands );
    CPPUNIT_TEST( testManual );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    TaskContext* tc;
    Processor processor;
    MethodFactoryInterface* createMethodFactory();
    DataSourceFactoryInterface* createDataSourceFactory();
    CommandFactoryInterface* createCommandFactory();

    double m0() { return -d0(); }
    double m1(int i) { return -d1(i); }
    double m2(int i, double d) { return -d2(i,d); }
    double m3(int i, double d, bool c) { return -d3(i,d,c); }
    double m4(int i, double d, bool c, std::string s) { return -d4(i,d,c,s); }

    double d0() const { return 1.0; }
    double d1(int i) const { return 2.0; }
    double d2(int i, double d) const { return 3.0; }
    double d3(int i, double d, bool c) const { return 4.0; }
    double d4(int i, double d, bool c, std::string s) const { return 5.0; }

    bool cd0()  { return true; }
    bool cd1(int i)  { return true; }
    bool cd2(int i, double d)  { return true; }
    bool cd3(int i, double d, char c)  { return true; }
    bool cn0() const  { return true; }
    bool cn1(int i) const  { return true; }
    bool cn2(int i, double d) const  { return true; }
    bool cn3(int i, double d, char c) const  { return true; }

    bool assertBool( bool );

public:

    void setUp();
    void tearDown();

    void testMethods();
    void testData();
    void testCommands();

    void testManual();
};

#endif
