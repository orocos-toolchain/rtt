/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseTiming.hpp 

                        TestCaseTiming.hpp -  description
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
 
 

#ifndef TESTCASETIMING_HPP
#define TESTCASETIMING_HPP

#include "TestCase.hpp"
#include <corelib/WriteCout.hpp>

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * A test that writes a block wave to the parallell port for 5 seconds
     * at 1kHz
     */

    class TestCaseTiming :
                public TestCase
    {

        public:
            /**
             * Constructs a TestCaseTiming object.
             * @param suite  The test suite this test case is a part of.
             */
            TestCaseTiming( TestSuite* suite, WriteInterface* output );

            /**
             * Initialize the simple test case, that is, give a message.
             */
            virtual bool initialize();

            /**
             * This function will be executed periodically. It just gives a message.
             */
            virtual  void step();

            /**
             * Cleanup the test case after execution. It just gives a message.
             */
            virtual void finalize();

            /**
             * Do cleanup in kernel space (delete) and destroy the test case.
             */
            virtual ~TestCaseTiming();

        protected:

            // Forbidden
            TestCaseTiming();

        private:

            /**
             * Stream for output to cout.
             */
            WriteInterface* out;

            /**
             * The output bit.
             */
            unsigned char outbit;

    };

}

#endif // TESTCASETIMING_HPP

