#ifndef STATETEST_H
#define STATETEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>
#include <corelib/TaskSimulation.hpp>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class StateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( StateTest );
    CPPUNIT_TEST( testParseState );
    //CPPUNIT_TEST( testStateFailure );
    CPPUNIT_TEST( testStateChildren );
//     CPPUNIT_TEST( testStateAnd );
//     CPPUNIT_TEST( testStateTry );
//     CPPUNIT_TEST( testStateUntil );
//     CPPUNIT_TEST( testStateUntilFail );
    CPPUNIT_TEST_SUITE_END();

    Parser parser;
    Processor gprocessor;
    TaskContext gtc;
    TaskSimulation gtask;
    MethodFactoryInterface* createMethodFactory();
    CommandFactoryInterface* createCommandFactory();
    DataSourceFactoryInterface* createDataSourceFactory();
    bool assertBool( bool );
    bool assertMsg( bool, const std::string& msg);
    int increase();
    void reset();
    void doState( const std::string& prog, TaskContext*, bool test=true );
    void finishState( TaskContext* , std::string );

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int i;
public:
    StateTest();

    void setUp();
    void tearDown();

    void testParseState();
    void testStateFailure();
    void testStateChildren();
    void testStateAnd();
    void testStateTry();
    void testStateUntil();
    void testStateUntilFail();

};

#endif
