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
#include <Timer.hpp>
#include <iostream>

#define EPSILON 0.000000002

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );

using namespace std;
using namespace RTT;
using namespace boost;

void 
TimeTest::setUp()
{
    hbg = TimeService::Instance();
    long_ns = 9007199254740992LL;       // == 2^53
    //long_ns = 4503599627370496LL;       // == 2^52
    //long_ns = 123456789123456789LL; // 1.234...e17 ns == approx 4 years, but double can not represent this.
    long_S  = 9007199.254740992;
    //long_S  = 4.503599627370496e14;
    normal_ns = 1000000000; // 1s
    normal_S = 1.0; // 1s
    small_ns = 1;  //  1ns
    small_S = 1e-9; //  1ns
    small_t = 1;
    normal_t = 1000000000; // 1e9 ticks
    long_t  = long_ns;     // == 2^53
}


void 
TimeTest::tearDown()
{
    hbg->enableSystemClock( true );
}

#define CPPUNIT_ASSERT_EQUAL_EPS(a, b, eps)  if ( !( -(eps) < ((a)-(b)) && (eps) > ((a)-(b)) ) ) {  CPPUNIT_ASSERT_EQUAL( a,b); }	   

void 
TimeTest::testSecondsConversion()
{
    // Test one way
    CPPUNIT_ASSERT_EQUAL( long_ns  , Seconds_to_nsecs(long_S)  );
    CPPUNIT_ASSERT_EQUAL( normal_ns, Seconds_to_nsecs(normal_S));
    CPPUNIT_ASSERT_EQUAL( small_ns , Seconds_to_nsecs(small_S) );

    // Test other way
    CPPUNIT_ASSERT_EQUAL( long_S  , nsecs_to_Seconds(long_ns));
    CPPUNIT_ASSERT_EQUAL( normal_S, nsecs_to_Seconds(normal_ns));
    CPPUNIT_ASSERT_EQUAL( small_S , nsecs_to_Seconds(small_ns));

    // Test invariance of conversions :
    CPPUNIT_ASSERT_EQUAL( long_ns  , Seconds_to_nsecs( nsecs_to_Seconds(long_ns) ));
    CPPUNIT_ASSERT_EQUAL( normal_ns, Seconds_to_nsecs( nsecs_to_Seconds(normal_ns) ));
    CPPUNIT_ASSERT_EQUAL( small_ns , Seconds_to_nsecs( nsecs_to_Seconds(small_ns) ));
    CPPUNIT_ASSERT_EQUAL( long_S  , nsecs_to_Seconds( Seconds_to_nsecs(long_S) ));
    CPPUNIT_ASSERT_EQUAL( normal_S, nsecs_to_Seconds( Seconds_to_nsecs(normal_S) ));
    CPPUNIT_ASSERT_EQUAL( small_S , nsecs_to_Seconds( Seconds_to_nsecs(small_S) ));
}

void 
TimeTest::testTicksConversion()
{
    // Test ticks conversion invariance :
    // Allow rounding error of '2'. ( < 3 )
    int margin = 3;
    CPPUNIT_ASSERT_EQUAL_EPS( long_ns  , TimeService::ticks2nsecs( TimeService::nsecs2ticks( long_ns )), margin );
    CPPUNIT_ASSERT_EQUAL_EPS( normal_ns, TimeService::ticks2nsecs( TimeService::nsecs2ticks( normal_ns )), margin );
    CPPUNIT_ASSERT_EQUAL_EPS( small_ns , TimeService::ticks2nsecs( TimeService::nsecs2ticks( small_ns )), margin );
    CPPUNIT_ASSERT_EQUAL_EPS( long_t  , TimeService::nsecs2ticks( TimeService::ticks2nsecs( long_t )), margin * 2);
    CPPUNIT_ASSERT_EQUAL_EPS( normal_t, TimeService::nsecs2ticks( TimeService::ticks2nsecs( normal_t )), margin * 2);
    CPPUNIT_ASSERT_EQUAL_EPS( small_t , TimeService::nsecs2ticks( TimeService::ticks2nsecs( small_t )), margin * 2);
}

void 
TimeTest::testTimeProgress()
{
    // A time measurement takes time :
    TimeService::ticks t = hbg->getTicks();
    CPPUNIT_ASSERT( t !=  hbg->getTicks() );
    CPPUNIT_ASSERT( 0 !=  hbg->ticksSince(t) );
    CPPUNIT_ASSERT( 0 !=  hbg->secondsSince(t) );

    // With Re-init of t :
    t = 0;
    CPPUNIT_ASSERT_EQUAL( TimeService::ticks(0) , hbg->getTicks( t ) );
    t = 0;
    // CPPUNIT_ASSERT_EQUAL( Seconds(0.0) , hbg->getSeconds( t ) );

    // Stop Time Progress:
    hbg->enableSystemClock( false );
    t = hbg->getTicks();
    CPPUNIT_ASSERT_EQUAL( t ,  hbg->getTicks() );
    CPPUNIT_ASSERT_EQUAL( TimeService::ticks(0) ,  hbg->ticksSince(t) );
    // CPPUNIT_ASSERT_EQUAL( Seconds(0.0) ,  hbg->secondsSince(t) );

    Seconds change_S  = 0.123456789;

    hbg->secondsChange( change_S );
    CPPUNIT_ASSERT( t !=  hbg->getTicks() ); // ticks must have changed
    CPPUNIT_ASSERT( -EPSILON < (change_S - hbg->secondsSince(t)) &&
                     EPSILON > (change_S - hbg->secondsSince(t)) );

    // Restart Time Progress
    hbg->enableSystemClock( true );
    CPPUNIT_ASSERT( t !=  hbg->getTicks() );
    CPPUNIT_ASSERT( TimeService::ticks(0) !=  hbg->ticksSince(t) );
    CPPUNIT_ASSERT( Seconds(0.0) !=  hbg->secondsSince(t) );
    
}

struct TestTimer
    : public Timer
{
    std::vector< std::pair<Timer::TimerId, Seconds> > occured;
    TimeService::Seconds mstart;
    TestTimer()
        :Timer(32, ORO_SCHED_RT, OS::HighestPriority)
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

void TimeTest::testTimers()
{
    TestTimer timer;
    Seconds now = hbg->secondsSince( 0 );
    // Test arming
    CPPUNIT_ASSERT( timer.arm(0, 0.5) );
    CPPUNIT_ASSERT( timer.arm(1, 0.6) );
    CPPUNIT_ASSERT( timer.arm(2, 0.5) );

    CPPUNIT_ASSERT( timer.arm(3, 0.8) );
    CPPUNIT_ASSERT( timer.arm(3, 0.9) );

    CPPUNIT_ASSERT( timer.isActive( 0 ) );
    CPPUNIT_ASSERT( timer.isActive( 1 ) );
    CPPUNIT_ASSERT( timer.isActive( 2 ) );
    CPPUNIT_ASSERT( timer.isActive( 3 ) );

    sleep(1);

    // Test clearing
    CPPUNIT_ASSERT( !timer.isActive( 0 ) );
    CPPUNIT_ASSERT( !timer.isActive( 1 ) );
    CPPUNIT_ASSERT( !timer.isActive( 2 ) );
    CPPUNIT_ASSERT( !timer.isActive( 3 ) );

    // Test sequence
    CPPUNIT_ASSERT( timer.occured.size() == 4 );
    CPPUNIT_ASSERT( timer.occured[0].first == 0 );
    CPPUNIT_ASSERT( timer.occured[1].first == 2 );
    CPPUNIT_ASSERT( timer.occured[2].first == 1 );
    CPPUNIT_ASSERT( timer.occured[3].first == 3 );

    // Test timeliness
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[0].second, now+0.5, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[1].second, now+0.5, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[2].second, now+0.6, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[3].second, now+0.9, 0.1 );

    // Test wrong parameters.
    CPPUNIT_ASSERT( timer.arm(4, -0.1) == false);
    CPPUNIT_ASSERT( timer.arm(500, 0.1) == false);

    timer.occured.clear();

    // Test resize.
    CPPUNIT_ASSERT( timer.arm(10, 0.5) );
    timer.setMaxTimers( 5 ); // clears the timer
    sleep(1);
    CPPUNIT_ASSERT( timer.occured.size() == 0 );
}

void TimeTest::testTimerPeriod()
{
    TestTimer timer;
    Seconds now = hbg->secondsSince( 0 );
    // Test starting periodics
    CPPUNIT_ASSERT( timer.startTimer(0, 0.1) );
    CPPUNIT_ASSERT( timer.startTimer(1, 0.6) );
    CPPUNIT_ASSERT( timer.startTimer(2, 0.5) );

    CPPUNIT_ASSERT( timer.startTimer(3, 0.5) );
    CPPUNIT_ASSERT( timer.startTimer(3, 0.2) );

    CPPUNIT_ASSERT( timer.isActive( 0 ) );
    CPPUNIT_ASSERT( timer.isActive( 1 ) );
    CPPUNIT_ASSERT( timer.isActive( 2 ) );
    CPPUNIT_ASSERT( timer.isActive( 3 ) );

    sleep(1);

    // Test clearing
    CPPUNIT_ASSERT( timer.killTimer( 0 ) );
    CPPUNIT_ASSERT( timer.killTimer( 1 ) );
    CPPUNIT_ASSERT( timer.killTimer( 2 ) );
    CPPUNIT_ASSERT( timer.killTimer( 3 ) );
    CPPUNIT_ASSERT( !timer.isActive( 0 ) );
    CPPUNIT_ASSERT( !timer.isActive( 1 ) );
    CPPUNIT_ASSERT( !timer.isActive( 2 ) );
    CPPUNIT_ASSERT( !timer.isActive( 3 ) );

    // Test sequence
    //CPPUNIT_ASSERT( timer.occured.size() == 4 ); hard to estimate
    CPPUNIT_ASSERT( timer.occured[0].first == 0 );
    CPPUNIT_ASSERT( timer.occured[1].first == 0 );
    CPPUNIT_ASSERT( timer.occured[2].first == 3 );
    CPPUNIT_ASSERT( timer.occured[3].first == 0 );
    CPPUNIT_ASSERT( timer.occured[4].first == 0 );
    CPPUNIT_ASSERT( timer.occured[5].first == 3 );

    CPPUNIT_ASSERT( timer.occured[6].first == 0 );
    CPPUNIT_ASSERT( timer.occured[7].first == 2 );

    CPPUNIT_ASSERT( timer.occured[8].first == 0 );
    CPPUNIT_ASSERT( timer.occured[9].first == 1 );
    CPPUNIT_ASSERT( timer.occured[10].first == 3 );
    CPPUNIT_ASSERT( timer.occured[11].first == 0 );
    CPPUNIT_ASSERT( timer.occured[12].first == 0 );
    CPPUNIT_ASSERT( timer.occured[13].first == 3 );
    CPPUNIT_ASSERT( timer.occured[14].first == 0 );

    // Test timeliness
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[0].second, now+0.1, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[1].second, now+0.2, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[2].second, now+0.2, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[3].second, now+0.3, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[4].second, now+0.4, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[5].second, now+0.4, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[6].second, now+0.5, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[7].second, now+0.5, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[8].second, now+0.6, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[9].second, now+0.6, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[10].second, now+0.6, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[11].second, now+0.7, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[12].second, now+0.8, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[13].second, now+0.8, 0.1 );
    CPPUNIT_ASSERT_EQUAL_EPS( timer.occured[14].second, now+0.9, 0.1 );

    // Test wrong parameters.
    CPPUNIT_ASSERT( timer.startTimer(4, -0.1) == false);
    CPPUNIT_ASSERT( timer.startTimer(500, 0.1) == false);

    timer.occured.clear();

    // Test resize.
    CPPUNIT_ASSERT( timer.startTimer(10, 0.5) );
    timer.setMaxTimers( 5 ); // clears the timer
    sleep(1);
    CPPUNIT_ASSERT( timer.occured.size() == 0 );
}
