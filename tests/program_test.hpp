#ifndef PROGRAMTEST_H
#define PROGRAMTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <execution/Parser.hpp>
#include <execution/Processor.hpp>
#include <execution/TaskContext.hpp>
#include <execution/Factories.hpp>
#include <string>
#include <corelib/TaskSimulation.hpp>

using namespace ORO_CoreLib;
using namespace ORO_Execution;

class ProgramTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ProgramTest );
    CPPUNIT_TEST( testParseProgram );
    CPPUNIT_TEST( testProgramFailure );
    CPPUNIT_TEST( testProgramCondition );
    CPPUNIT_TEST( testProgramAnd );
    CPPUNIT_TEST( testProgramTry );
    CPPUNIT_TEST( testProgramUntil );
    CPPUNIT_TEST( testProgramUntilFail );
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
    void doProgram( const std::string& prog, TaskContext*, bool test=true );
    void finishProgram( TaskContext* , std::string );
    void loopProgram( FunctionGraph*);

    bool true_genCom() { return true; }
    bool false_genCom() { return false; }
    bool true_gen() const { return true; }
    bool false_gen() const { return false; }

    bool bool_gen( bool b ) const { return b; }
    int i;
public:
    ProgramTest();

    void setUp();
    void tearDown();

    void testParseProgram();
    void testProgramCondition();
    void testProgramFailure();
    void testProgramAnd();
    void testProgramTry();
    void testProgramUntil();
    void testProgramUntilFail();

};

#endif
