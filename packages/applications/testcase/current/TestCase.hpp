/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCase.hpp 

                        TestCase.hpp -  description
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
 
 

#ifndef TESTCASE_HPP
#define TESTCASE_HPP

#include <corelib/RunnableInterface.hpp>
#include <corelib/ConditionInterface.hpp>
//#include <corelib/ConditionVariableInterface.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/Event.hpp>

#include "TestSuite.hpp"
#include "TestCaseInterface.hpp"

#include <string>
#include <vector>

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * TestCase. Inherit from this class to implement your own tests.
     * A test can be cleaned up and reinitialized many times.
     */

    class TestCase :
                public TestCaseInterface,
                public RunnableInterface
    {

        public:
            /**
             * Constructs a TestCase with a specified name and initialize
             * the test case in kernel space (new).
             * @param name  The name of the test case.
             * @param suite  The test suite this test case is a part of.
             * @param nSteps The number of steps this TestCase has to 
             *                  execute.
             */
            TestCase( const std::string& name, TestSuite* suite, int nSteps );

            /**
             * Get the name of this test case.
             * @return   The name of the test case.
             */
            virtual const char* caseNameGet();

            /**
             * Start the test case.
             * @param ei        Event that has to warn the TestRunner 
             *                  that the TestCase has completed.
             */
            virtual void start( EventOperationInterface* ei );

            /**
             * Initialize the test case. Overload this function with your 
             * own test code.
             */
            virtual bool initialize();

            /**
             * This function will be executed periodically. Overload this
             * function with your own test code.
             */
            virtual  void step();

            /**
             * Cleanup the test case after execution. Overload this function 
             * with your own test code.
             */
            virtual void finalize();

            /**
             * Get all signaled errors.
             * @return   The vector returned only lives during the 
             *     lifetime of the instance of this class.
             */
            virtual std::vector<const std::string*>* errorsGet();

            /**
             * Get the number of errors encountered during this test.
             */
            virtual int errorsNumber();

            /**
             * Get the event of this test case.
             */
            virtual EventOperationInterface* eventGet();

            /**
             * Return the number of steps the test case has to run.
             */
            virtual int numberOfSteps();

            /**
             * Do cleanup in kernel space (delete) and destroy the test case.
             */
            virtual ~TestCase();

        protected:

            // Forbidden
            TestCase();

            /**
             * Signal an error description.
             * @param description The description of the error.
             */
            virtual void error( const std::string& description );

            /**
             * Signal an error description if a boolean variable is false.
             * @param condition  The variable to check.
             * @param error  The description of the error that gets signaled if <condition> is false.
             */
            virtual void testAssert( bool condition, const std::string& errorStr );

            /**
             * Signal an error description if a condition is false.
             * @param condition  The condition to check.
             * @param error  The description of the error that gets signaled if <condition> is false.
             */
            virtual void testAssert( ConditionInterface* condition, const std::string& errorStr );

            /**
             * Signal an error description if the condition of a condition variable is false.
             * @param cv  The condition variable to check.
             * @param error  The description of the error that gets signaled if the condition of <cv> is false.
             */
            //virtual void testAssert( ConditionVariableInterface* cv, const std::string& errorStr );

            /**
             * The name of this test case.
             */
            std::string caseName;

            /**
             * The number of steps the test case has to run. This has to be set individually by each subclass of TestCase.
             */
            const int numberSteps;

            /**
             * The error descriptions.
             */
            std::vector<const std::string*> errors;

        private:
            /**
             * Pointer to the event needed to warn that the test case has completed his execution.
             */
            EventOperationInterface* caseEvent;

            /**
             * The thread this test case has to execute.
             */
            class Task : public TaskPreemptible
            {
                TestCase* testCase;
                int stepsToGo;
                bool stopped;
                public:
                bool start();
                 void step();
                bool stop();
                Task(secs s, nsecs ns, TestCase* t );
                virtual ~Task();
            } task;

    };

}

#endif // TESTCASE_HPP

