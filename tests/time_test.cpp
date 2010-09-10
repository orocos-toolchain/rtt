/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:13 CET 2005  time_test.cpp

                        time_test.cpp -  description
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


#include "time_test.hpp"
#include <boost/bind.hpp>
#include <os/Timer.hpp>
#include <rtt-detail-fwd.hpp>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define EPSILON 0.000000002

// Registers the fixture into the 'registry'
//CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );

using namespace std;
using namespace RTT;
using namespace RTT::detail;
using namespace boost;

TimeTest::TimeTest()
{
    hbg = TimeService::Instance();
    long_ns = 9007199254740992LL;       // == 2^53
    //long_ns = 4503599627370496LL;       // == 2^52
    //long_ns = 123456789123456789LL; // 1.234...e17 ns == approx 4 years, but double can not represent this.
    long_S  = 9007199.254740992;
    //long_S  = 4.503599627370496e14;
    normal_ns = 1000000000; // 1s
    normal_S = 1.0; // 1s
    small_ns = 10;  //  10ns
    small_S = 10e-9; //  10ns
    small_t = 10;
    normal_t = 1000000000; // 1e9 ticks
    long_t  = long_ns;     // == 2^53
}

TimeTest::~TimeTest()
{
    hbg->enableSystemClock( true );
}

struct TestTimer
    : public Timer
{
    std::vector< std::pair<Timer::TimerId, Seconds> > occured;
    TimeService::Seconds mstart;
    TestTimer()
        :Timer(32, ORO_SCHED_RT, os::HighestPriority)
    {
        occured.reserve(100);
        mstart = TimeService::Instance()->secondsSince(0);
    }
    void timeout(Timer::TimerId id)
    {
        Seconds now = TimeService::Instance()->secondsSince( 0 );
        occured.push_back( std::make_pair(id, now) );
        //cout << "Occured: "<< id <<" on " << now - mstart <<"\n";
    }

    ~TestTimer()
    {
        cout.flush();
    }
};

BOOST_FIXTURE_TEST_SUITE( TimeTestSuite, TimeTest )

BOOST_AUTO_TEST_CASE( testSecondsConversion )
{
    // Test one way
    BOOST_REQUIRE_EQUAL( long_ns  , Seconds_to_nsecs(long_S)  );
    BOOST_REQUIRE_EQUAL( normal_ns, Seconds_to_nsecs(normal_S));
    BOOST_REQUIRE_EQUAL( small_ns , Seconds_to_nsecs(small_S) );

    // Test other way
    BOOST_REQUIRE_EQUAL( long_S  , nsecs_to_Seconds(long_ns));
    BOOST_REQUIRE_EQUAL( normal_S, nsecs_to_Seconds(normal_ns));
    BOOST_REQUIRE_EQUAL( small_S , nsecs_to_Seconds(small_ns));

    // Test invariance of conversions :
    BOOST_REQUIRE_EQUAL( long_ns  , Seconds_to_nsecs( nsecs_to_Seconds(long_ns) ));
    BOOST_REQUIRE_EQUAL( normal_ns, Seconds_to_nsecs( nsecs_to_Seconds(normal_ns) ));
    BOOST_REQUIRE_EQUAL( small_ns , Seconds_to_nsecs( nsecs_to_Seconds(small_ns) ));
    BOOST_REQUIRE_EQUAL( long_S  , nsecs_to_Seconds( Seconds_to_nsecs(long_S) ));
    BOOST_REQUIRE_EQUAL( normal_S, nsecs_to_Seconds( Seconds_to_nsecs(normal_S) ));
    BOOST_REQUIRE_EQUAL( small_S , nsecs_to_Seconds( Seconds_to_nsecs(small_S) ));
}

BOOST_AUTO_TEST_CASE( testTicksConversion )
{
    // Test ticks conversion invariance :
    // margin is in % rounding error.
    int margin = 1;
#if defined( OROCOS_TARGET_LXRT) || defined(OROCOS_TARGET_XENOMAI)
    int small_margin = 25; // 25% of 8ns : allow a two-off.
#else
    int small_margin = 10; // 10% of 8ns : allow a one-off.
#endif

    // I'm removing the small conversions because they test more the underlying RTOS than Orocos and the underlying RTOS
    // isn't fixing this for years...
    BOOST_REQUIRE_CLOSE( (double)long_ns  , (double)TimeService::ticks2nsecs( TimeService::nsecs2ticks( long_ns )), margin );
    BOOST_REQUIRE_CLOSE( (double)normal_ns, (double)TimeService::ticks2nsecs( TimeService::nsecs2ticks( normal_ns )), margin );
    //BOOST_REQUIRE_CLOSE( (double)small_ns , (double)TimeService::ticks2nsecs( TimeService::nsecs2ticks( small_ns )), small_margin );
    BOOST_REQUIRE_CLOSE( (double)long_t  , (double)TimeService::nsecs2ticks( TimeService::ticks2nsecs( long_t )), margin );
    BOOST_REQUIRE_CLOSE( (double)normal_t, (double)TimeService::nsecs2ticks( TimeService::ticks2nsecs( normal_t )), margin );
    //BOOST_REQUIRE_CLOSE( (double)small_t , (double)TimeService::nsecs2ticks( TimeService::ticks2nsecs( small_t )), small_margin );
}

BOOST_AUTO_TEST_CASE( testTimeProgress )
{
    // A time measurement takes time :
    TimeService::ticks t = hbg->getTicks();
    usleep(100000);
    BOOST_CHECK( t !=  hbg->getTicks() );
    BOOST_CHECK( 0 !=  hbg->ticksSince(t) );
    BOOST_CHECK( 0 !=  hbg->secondsSince(t) );

    // With Re-init of t :
    t = 0;
    BOOST_REQUIRE_EQUAL( TimeService::ticks(0) , hbg->getTicks( t ) );
    t = 0;
    // BOOST_REQUIRE_EQUAL( Seconds(0.0) , hbg->getSeconds( t ) );

    // Stop Time Progress:
    hbg->enableSystemClock( false );
    t = hbg->getTicks();
    BOOST_REQUIRE_EQUAL( t ,  hbg->getTicks() );
    BOOST_REQUIRE_EQUAL( TimeService::ticks(0) ,  hbg->ticksSince(t) );
    // BOOST_REQUIRE_EQUAL( Seconds(0.0) ,  hbg->secondsSince(t) );

    Seconds change_S  = 0.123456789;

    hbg->secondsChange( change_S );
    BOOST_CHECK( t !=  hbg->getTicks() ); // ticks must have changed
    BOOST_CHECK( -EPSILON < (change_S - hbg->secondsSince(t)) &&
                     EPSILON > (change_S - hbg->secondsSince(t)) );

    // Restart Time Progress
    hbg->enableSystemClock( true );
    BOOST_CHECK( t !=  hbg->getTicks() );
    BOOST_CHECK( TimeService::ticks(0) !=  hbg->ticksSince(t) );
    BOOST_CHECK( Seconds(0.0) !=  hbg->secondsSince(t) );

}

BOOST_AUTO_TEST_CASE( testTimers )
{
    TestTimer timer;
    Seconds now = hbg->secondsSince( 0 );
    // Test arming
    BOOST_CHECK( timer.arm(0, 0.5) );
    BOOST_CHECK( timer.arm(1, 0.6) );
    BOOST_CHECK( timer.arm(2, 0.5) );

    BOOST_CHECK( timer.arm(3, 0.8) );
    BOOST_CHECK( timer.arm(3, 0.9) );

    BOOST_CHECK( timer.isArmed( 0 ) );
    BOOST_CHECK( timer.isArmed( 1 ) );
    BOOST_CHECK( timer.isArmed( 2 ) );
    BOOST_CHECK( timer.isArmed( 3 ) );

    sleep(1);

    // Test clearing
    BOOST_CHECK( !timer.isArmed( 0 ) );
    BOOST_CHECK( !timer.isArmed( 1 ) );
    BOOST_CHECK( !timer.isArmed( 2 ) );
    BOOST_CHECK( !timer.isArmed( 3 ) );

    // Test sequence
    BOOST_CHECK( timer.occured.size() == 4 );
    BOOST_CHECK( timer.occured[0].first == 0 );
    BOOST_CHECK( timer.occured[1].first == 2 );
    BOOST_CHECK( timer.occured[2].first == 1 );
    BOOST_CHECK( timer.occured[3].first == 3 );

    // Test timeliness
    BOOST_REQUIRE_CLOSE( timer.occured[0].second, now+0.5, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[1].second, now+0.5, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[2].second, now+0.6, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[3].second, now+0.9, 0.1 );

    // Test wrong parameters.
    BOOST_CHECK( timer.arm(4, -0.1) == false);
    BOOST_CHECK( timer.arm(500, 0.1) == false);

    timer.occured.clear();

    // Test resize.
    BOOST_CHECK( timer.arm(10, 0.5) );
    timer.setMaxTimers( 5 ); // clears the timer
    sleep(1);
    BOOST_CHECK( timer.occured.size() == 0 );
}

BOOST_AUTO_TEST_CASE( testTimerPeriod )
{
    TestTimer timer;
    Seconds now = hbg->secondsSince( 0 );
    // Test starting periodics
    BOOST_CHECK( timer.startTimer(0, 0.1) );
    BOOST_CHECK( timer.startTimer(1, 0.6) );
    BOOST_CHECK( timer.startTimer(2, 0.5) );

    BOOST_CHECK( timer.startTimer(3, 0.5) );
    BOOST_CHECK( timer.startTimer(3, 0.2) );

    BOOST_CHECK( timer.isArmed( 0 ) );
    BOOST_CHECK( timer.isArmed( 1 ) );
    BOOST_CHECK( timer.isArmed( 2 ) );
    BOOST_CHECK( timer.isArmed( 3 ) );

    sleep(1);

    // Test clearing
    BOOST_CHECK( timer.killTimer( 0 ) );
    BOOST_CHECK( timer.killTimer( 1 ) );
    BOOST_CHECK( timer.killTimer( 2 ) );
    BOOST_CHECK( timer.killTimer( 3 ) );
    BOOST_CHECK( !timer.isArmed( 0 ) );
    BOOST_CHECK( !timer.isArmed( 1 ) );
    BOOST_CHECK( !timer.isArmed( 2 ) );
    BOOST_CHECK( !timer.isArmed( 3 ) );

    // Test sequence
    //BOOST_CHECK( timer.occured.size() == 4 ); hard to estimate
    BOOST_CHECK( timer.occured[0].first == 0 );
    BOOST_CHECK( timer.occured[1].first == 0 );
    BOOST_CHECK( timer.occured[2].first == 3 );
    BOOST_CHECK( timer.occured[3].first == 0 );
    BOOST_CHECK( timer.occured[4].first == 0 );
    BOOST_CHECK( timer.occured[5].first == 3 );

    BOOST_CHECK( timer.occured[6].first == 0 );
    BOOST_CHECK( timer.occured[7].first == 2 );

    BOOST_CHECK( timer.occured[8].first == 0 );
    BOOST_CHECK( timer.occured[9].first == 1 );
    BOOST_CHECK( timer.occured[10].first == 3 );
    BOOST_CHECK( timer.occured[11].first == 0 );
    BOOST_CHECK( timer.occured[12].first == 0 );
    BOOST_CHECK( timer.occured[13].first == 3 );
    BOOST_CHECK( timer.occured[14].first == 0 );

    // Test timeliness
    BOOST_REQUIRE_CLOSE( timer.occured[0].second, now+0.1, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[1].second, now+0.2, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[2].second, now+0.2, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[3].second, now+0.3, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[4].second, now+0.4, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[5].second, now+0.4, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[6].second, now+0.5, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[7].second, now+0.5, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[8].second, now+0.6, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[9].second, now+0.6, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[10].second, now+0.6, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[11].second, now+0.7, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[12].second, now+0.8, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[13].second, now+0.8, 0.1 );
    BOOST_REQUIRE_CLOSE( timer.occured[14].second, now+0.9, 0.1 );

    // Test wrong parameters.
    BOOST_CHECK( timer.startTimer(4, -0.1) == false);
    BOOST_CHECK( timer.startTimer(500, 0.1) == false);

    timer.occured.clear();

    // Test resize.
    BOOST_CHECK( timer.startTimer(10, 0.5) );
    timer.setMaxTimers( 5 ); // clears the timer
    sleep(1);
    BOOST_CHECK( timer.occured.size() == 0 );
}

BOOST_AUTO_TEST_SUITE_END()
