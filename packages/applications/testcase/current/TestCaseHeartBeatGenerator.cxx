#include "TestCaseHeartBeatGenerator.hpp"

#include <iostream>

namespace UnitTesting
{

    using namespace std;
    
    TestCaseHeartBeatGenerator::TestCaseHeartBeatGenerator( TestSuite* suite ) :
        TestCase( "TestCaseHeartBeatGenerator", suite, Test_Steps ),
        errorWrongTimeGet( "The tick of the HeartBeat clock doesn't advance monotonically (or at all).\n" ),
        errorWrongRelativeTimeGet( "The relative time (in seconds) of the HeartBeat clock doesn't advance monotonically (or at all).\n" ), mark(0)
    {
        hbg = HeartBeatGenerator::Instance();
    }

    TestCaseHeartBeatGenerator::~TestCaseHeartBeatGenerator()
    {
    }

    bool TestCaseHeartBeatGenerator::initialize()
    {
        mark = hbg->ticksGet();
        steps = 0;
        return true;
    }

    void TestCaseHeartBeatGenerator::step()
    {
        t[steps++] = hbg->ticksGet();
        cout << hbg->secondsSince(mark)<<endl;
    }

    void TestCaseHeartBeatGenerator::finalize()
    {
        bool correct = true;
        for ( int i=0; i < Test_Steps - 2 ; ++i)
            if ( t[i] > t[i+1] )
                 correct = false;
        testAssert( correct, errorWrongTimeGet);
        
        correct = true;
        for ( int i=0; i < Test_Steps - 2 ; ++i)
            if ( hbg->secondsSince(t[i]) < hbg->secondsSince(t[i+1]) )
                correct = false;
        testAssert( correct, errorWrongRelativeTimeGet );
    }

}
