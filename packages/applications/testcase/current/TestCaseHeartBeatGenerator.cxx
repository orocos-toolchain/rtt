#include "TestCaseHeartBeatGenerator.hpp"

namespace UnitTesting
{

    using namespace std;
    
    TestCaseHeartBeatGenerator::TestCaseHeartBeatGenerator( TestSuite* suite ) :
        TestCase( "TestCaseHeartBeatGenerator", suite, 10 ),
        errorWrongTimeGet( "The tick of the HeartBeat clock doesn't advance.\n" ),
        errorWrongRelativeTimeGet( "Relative time is wrong.\n" ), t1(0), mark(0)
    {
        hbg = HeartBeatGenerator::Instance();
    }

    TestCaseHeartBeatGenerator::~TestCaseHeartBeatGenerator()
    {
    }

    bool TestCaseHeartBeatGenerator::initialize()
    {
        mark = hbg->ticksGet();
        return true;
    }

    void TestCaseHeartBeatGenerator::step()
    {
        t1 = hbg->ticksGet();

        // TODO: Add an event to the HeartBeatGenerator and test if it's fired.
        //   This is tested now in TestCaseEvent.

    }

    void TestCaseHeartBeatGenerator::finalize()
    {
        HeartBeatGenerator::ticks relative = hbg->ticksGet( mark);

        //cout << "Ticks " << long( t1 ) << " " << long( mark ) << endl;
        //cout << "Relative " << long( relative ) << endl;

        testAssert( t1 > mark, errorWrongTimeGet );
        testAssert( relative > 0, errorWrongRelativeTimeGet );
    }

}
