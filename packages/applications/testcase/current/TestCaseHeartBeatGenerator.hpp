/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseHeartBeatGenerator.hpp 

                        TestCaseHeartBeatGenerator.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef TESTCASEHEARTBEATGENERATOR_HPP
#define TESTCASEHEARTBEATGENERATOR_HPP

#include <corelib/HeartBeatGenerator.hpp>

#include "TestCase.hpp"

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * Test event interrupts.
     */

    class TestCaseHeartBeatGenerator :
                public TestCase
    {

        public:
            TestCaseHeartBeatGenerator( TestSuite* suite );

            ~TestCaseHeartBeatGenerator();

            virtual bool initialize();

            virtual  void step();

            virtual void finalize();

        private:
            // Forbidden!!!
            TestCaseHeartBeatGenerator();

            /**
             * A pointer to the HeartBeatGenerator object we are testing.
             */
            HeartBeatGenerator *hbg;

            /**
             * The tick of the HeartBeat clock doesn't advance.
             */
            std::string errorWrongTimeGet;

            /**
             * Relative time is wrong.
             */
            std::string errorWrongRelativeTimeGet;

        static const int Test_Steps = 100;
        /**
         * The cumulative time over the TestCase
         */
        HeartBeatGenerator::ticks t[ Test_Steps ];

        /**
         * The first time measured in the TestCase
         */
        HeartBeatGenerator::ticks mark;

        int steps;
    };

}

#endif // TESTCASEHEARTBEATGENERATOR_HPP
