
#include "logger_test.hpp"
#include <unistd.h>
#include <iostream>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LoggerTest );

using namespace ORO_CoreLib;

class Dummy {};

#define QS 10

void 
LoggerTest::setUp()
{
    logger = Logger::Instance();
}


void 
LoggerTest::tearDown()
{
}

void LoggerTest::testStartStop()
{
    CPPUNIT_ASSERT( logger != 0 );
    CPPUNIT_ASSERT( &Logger::log() != 0 );
}
void LoggerTest::testLogEnv()
{
    Logger::log() << Logger::Debug  << "Debug Level set + text"<< Logger::nl;
    Logger::log() << "Test Log Environment variable : Single line" << Logger::endl;
    Logger::log() << "Test Log Environment variable : Two ";
    Logger::log() << "lines on one line." << Logger::endl;
    Logger::log() << "Test Log Environment variable : Two" << Logger::nl;
    Logger::log() << "lines on two lines." << Logger::endl;

    Logger::log() << "Test Log Environment variable : nl" << Logger::nl;
    Logger::log() << "Test Log Environment variable : flush" << flush;
    sleep(1);
    Logger::log() << " and std::endl." << std::endl;
}
