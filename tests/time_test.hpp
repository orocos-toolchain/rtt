/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:13 CET 2005  time_test.hpp 

                        time_test.hpp -  description
                           -------------------
    begin                : Fri February 11 2005
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
 
 

#ifndef TIMETEST_H
#define TIMETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <corelib/Time.hpp>
#include <corelib/TimeService.hpp>
#include <string>

class TimeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TimeTest );
    CPPUNIT_TEST( testSecondsConversion );
    CPPUNIT_TEST( testTicksConversion );
    CPPUNIT_TEST( testTimeProgress );
    CPPUNIT_TEST_SUITE_END();

    ORO_CoreLib::TimeService* hbg;
    double small_S, normal_S, long_S;
    ORO_CoreLib::TimeService::ticks small_t, normal_t, long_t;
    ORO_CoreLib::nsecs small_ns, normal_ns, long_ns;
public:
    void setUp();
    void tearDown();

    void testSecondsConversion();
    void testTicksConversion();
    void testTimeProgress();
};

#endif  // TIMETEST_H
