/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseHeartBeatGenerator.hpp 

                        TestCaseHeartBeatGenerator.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

        static const int Test_Steps = 1000;
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
