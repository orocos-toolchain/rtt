#ifndef DISPATCHTEST_H
#define DISPATCHTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>
#include <corelib/TaskSimulation.hpp>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class DispatchTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( DispatchTest );
    CPPUNIT_TEST( testParseDispatch );
    CPPUNIT_TEST( testDispatchFailure );
    CPPUNIT_TEST( testDispatchCondition );
    CPPUNIT_TEST( testDispatchAnd );
    CPPUNIT_TEST( testDispatchTry );
    CPPUNIT_TEST( testDispatchUntil );
    CPPUNIT_TEST( testDispatchUntilFail );
    CPPUNIT_TEST_SUITE_END();

    //CPPUNIT_TEST( testSendDispatch );

    Parser parser;
    Processor gprocessor;
    Processor mprocessor;
    Processor lprocessor;
    TaskSimulation gtask;
    TaskSimulation mtask;
    TaskSimulation ltask;
    TaskContext gtc;
    TaskContext mtc;
    TaskContext ltc;
    MethodFactoryInterface* createMethodFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    void doDispatch( const std::string& prog, TaskContext* );
    void finishDispatch( TaskContext* , std::string );
public:
    DispatchTest();

    void setUp();
    void tearDown();

    void testParseDispatch();
    void testDispatchCondition();
    void testSendDispatch();
    void testDispatchFailure();
    void testDispatchAnd();
    void testDispatchTry();
    void testDispatchUntil();
    void testDispatchUntilFail();

};

#endif
