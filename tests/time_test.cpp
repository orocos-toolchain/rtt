
#include <corelib/Time.hpp>
#include <corelib/PriorityThread.hpp>
#include <corelib/RunnableInterface.hpp>
#include <corelib/TaskSimulation.hpp>
#include <corelib/SimulationThread.hpp>

#include "time_test.hpp"
#include <boost/bind.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );

using namespace ORO_CoreLib;
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
}

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
    CPPUNIT_ASSERT_EQUAL( long_ns  , TimeService::ticks2nsecs( TimeService::nsecs2ticks( long_ns )) );
    CPPUNIT_ASSERT_EQUAL( normal_ns, TimeService::ticks2nsecs( TimeService::nsecs2ticks( normal_ns )) );
    CPPUNIT_ASSERT_EQUAL( small_ns , TimeService::ticks2nsecs( TimeService::nsecs2ticks( small_ns )) );
    CPPUNIT_ASSERT_EQUAL( long_t  , TimeService::nsecs2ticks( TimeService::ticks2nsecs( long_t )) );
    CPPUNIT_ASSERT_EQUAL( normal_t, TimeService::nsecs2ticks( TimeService::ticks2nsecs( normal_t )) );
    CPPUNIT_ASSERT_EQUAL( small_t , TimeService::nsecs2ticks( TimeService::ticks2nsecs( small_t )));
}

void 
TimeTest::testTimeProgress()
{
    // A time measurement takes time :
    TimeService::ticks t = hbg->ticksGet();
    Seconds s;
    CPPUNIT_ASSERT( t !=  hbg->ticksGet() );
    CPPUNIT_ASSERT( 0 !=  hbg->ticksSince(t) );
    CPPUNIT_ASSERT( 0 !=  hbg->secondsSince(t) );

    // With Re-init of t :
    t = 0;
    CPPUNIT_ASSERT_EQUAL( TimeService::ticks(0) , hbg->ticksGet( t ) );
    t = 0;
    CPPUNIT_ASSERT_EQUAL( Seconds(0.0) , hbg->secondsGet( t ) );

    // Stop Time Progress:
    hbg->enableSystemClock( false );
    t = hbg->ticksGet();
    CPPUNIT_ASSERT_EQUAL( t ,  hbg->ticksGet() );
    CPPUNIT_ASSERT_EQUAL( TimeService::ticks(0) ,  hbg->ticksSince(t) );
    CPPUNIT_ASSERT_EQUAL( Seconds(0.0) ,  hbg->secondsSince(t) );

    Seconds change_S  = 0.123456789;
    nsecs   change_ns = 123456789;

    hbg->secondsChange( change_S );
    CPPUNIT_ASSERT( t !=  hbg->ticksGet() ); // ticks must have changed
    CPPUNIT_ASSERT_EQUAL( change_ns,  TimeService::ticks2nsecs( hbg->ticksSince(t) ) );
    CPPUNIT_ASSERT_EQUAL( change_S,  hbg->secondsSince(t) );

    // Restart Time Progress
    hbg->enableSystemClock( true );
    CPPUNIT_ASSERT( t !=  hbg->ticksGet() );
    CPPUNIT_ASSERT( TimeService::ticks(0) !=  hbg->ticksSince(t) );
    CPPUNIT_ASSERT( Seconds(0.0) !=  hbg->secondsSince(t) );
    
}

    

