#ifndef LOGGERTEST_H
#define LOGGERTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/Logger.hpp>

using namespace ORO_CoreLib;

class LoggerTest : public CppUnit::TestFixture
{
    Logger* logger;
    CPPUNIT_TEST_SUITE( LoggerTest );
    CPPUNIT_TEST( testStartStop );
    CPPUNIT_TEST( testLogEnv );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void testStartStop();
    void testLogEnv();
};

#endif
