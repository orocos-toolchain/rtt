/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  logger_test.hpp 

                        logger_test.hpp -  description
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
 
 
#ifndef LOGGERTEST_H
#define LOGGERTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <Logger.hpp>

using namespace RTT;

class LoggerTest : public CppUnit::TestFixture
{
    Logger* logger;
    CPPUNIT_TEST_SUITE( LoggerTest );
    CPPUNIT_TEST( testStartStop );
    CPPUNIT_TEST( testLogEnv );
    CPPUNIT_TEST( testNewLog );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void testStartStop();
    void testLogEnv();
    void testNewLog();
};

#endif
