/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseHeartBeatGenerator.cxx 

                        TestCaseHeartBeatGenerator.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
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
        //cout <<".";
        //cout << hbg->secondsSince(mark)<<endl;
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
