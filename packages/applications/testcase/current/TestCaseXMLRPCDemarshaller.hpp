/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseXMLRPCDemarshaller.hpp 

                        TestCaseXMLRPCDemarshaller.hpp -  description
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
 
 

#ifndef TESTCASESIMPLE_HPP
#define TESTCASESIMPLE_HPP

#include "TestCase.hpp"
//#include <iostream>
#include "Property.hpp"
#include "XMLRPCDemarshaller.hpp"

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * A simple TestCase to test the TestCase class ;-)
     */

    class TestCaseXMLRPCDemarshaller :
                public TestCase
    {

        public:
            /**
             * Constructs a TestCaseXMLRPCDemarshaller object.
             * @param suite  The test suite this test case is a part of.
             */
            TestCaseXMLRPCDemarshaller( TestSuite* suite );

            /**
             * Initialise the simple test case, that is, give a message.
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
            virtual ~TestCaseXMLRPCDemarshaller();

        protected:

            // Forbidden
            TestCaseXMLRPCDemarshaller();

        private:

            /**
             * Counter for the number of steps.
             */
            int stepCounter;

            /**
             * The assert function doesn't work.
             */
            std::string errorAssert;

            /**
             * Step doesn't work.
             */
            std::string errorStep;

            /**
             * Test flag.
             */
            bool isSet;
    
			PropertyBag bag;

    };

}

#endif // TESTCASESIMPLE_HPP

