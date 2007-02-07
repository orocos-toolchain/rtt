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

#define EPSILON 0.000000001

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );

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

#define CPPUNIT_ASSERT_EQUAL_EPS(a, b, EPSILON)  if ( !( -EPSILON < a-b && EPSILON > a-b ) ) {  CPPUNIT_ASSERT_EQUAL( a,b); }
		   
			   

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
    CPPUNIT_ASSERT_EQUAL_EPS( long_ns  , TimeService::ticks2nsecs( TimeService::nsecs2ticks( long_ns )), 3 );
    CPPUNIT_ASSERT_EQUAL_EPS( normal_ns, TimeService::ticks2nsecs( TimeService::nsecs2ticks( normal_ns )), 3 );
    CPPUNIT_ASSERT_EQUAL_EPS( small_ns , TimeService::ticks2nsecs( TimeService::nsecs2ticks( small_ns )), 3 );
    CPPUNIT_ASSERT_EQUAL_EPS( long_t  , TimeService::nsecs2ticks( TimeService::ticks2nsecs( long_t )), 3 );
    CPPUNIT_ASSERT_EQUAL_EPS( normal_t, TimeService::nsecs2ticks( TimeService::ticks2nsecs( normal_t )), 3 );
    CPPUNIT_ASSERT_EQUAL_EPS( small_t , TimeService::nsecs2ticks( TimeService::ticks2nsecs( small_t )), 3);
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

    

