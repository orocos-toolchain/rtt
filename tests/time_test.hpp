
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
