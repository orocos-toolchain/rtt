/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:51 CET 2005  logger_test.cpp 

                        logger_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

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
    Logger::log() << " and std::endl." << std::endl;
}
