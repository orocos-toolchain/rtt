/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseConditionVariableTimed.hpp 

                        TestCaseConditionVariableTimed.hpp -  description
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
 
 

#ifndef TESTCASECONDITIONVARIABLETIMED_HPP
#define TESTCASECONDITIONVARIABLETIMED_HPP


#if 0
#include "TestCase.hpp"
#include <corelib/ConditionVariableTimed.hpp>
#include <corelib/MutexLock.hpp>

#include <corelib/ConditionBool.hpp>


namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * A TestCase to test the ConditionVariableTimed class.
     */

/*    class TestCaseConditionVariableTimed :
                public TestCase
    {

        public:
            /**
             * Constructs a TestCaseConditionVariableTimed object.
             * @param suite  The test suite this test case is a part of.
             */
/*            TestCaseConditionVariableTimed( TestSuite* suite );

            virtual bool initialize();

            virtual  void step();

            virtual void finalize();

            virtual ~TestCaseConditionVariableTimed();

        protected:

            // Forbidden
            TestCaseConditionVariableTimed();

        private:

            /**
             * The mutex for securing access to the condition.
             */

/*            Mutex lock ;
            ConditionBool condition1;
            ConditionBool condition2;
            ConditionVariableTimed cv1;
            ConditionVariableTimed cv2;

        /**
         * The value should be true but is not.
         */
/*        std::string errorValueNotTrue;

        /**
         * The value should be false but is not.
         */
/*        std::string errorValueNotFalse;

};
}
#endif

#endif // TESTCASECONDITIONVARIABLETIMED_HPP

