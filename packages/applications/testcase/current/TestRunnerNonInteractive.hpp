/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestRunnerNonInteractive.hpp 

                        TestRunnerNonInteractive.hpp -  description
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
 
 

#ifndef TESTRUNNERNONINTERACTIVE_HPP
#define TESTRUNNERNONINTERACTIVE_HPP

#include <os/ComponentThreaded.hpp>
#include <corelib/Event.hpp>
#include <corelib/EventListenerInterface.hpp>
#include <corelib/EventCompleterInterface.hpp>
#include "TestRunner.hpp"
#include <corelib/WriteInterface.hpp>
#include "TestSuite.hpp"
#include "TestCaseInterface.hpp"

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * This class executes TestCase objects in an non-interactive manner. 
     * All test cases will be executed, and a summary of the executed tests
     * wil be given.
     */

    class TestRunnerNonInteractive :
                public TestRunner,
                public EventListenerInterface,
                public EventCompleterInterface
    {

        public:
            /**
             * Constructs a TestRunnerNonInteractive object.
             * @param out  The output of the test runner.
             * @param suite  The suite containg the test cases that have to be executed.
             * @param finishEvent The event that will be fired when the TestRunner has finished.
             */
            TestRunnerNonInteractive( WriteInterface* out, TestSuite* suite, EventOperationInterface* finishEvent = 0 );

            /**
             * Destroys a TestRunnerNonInteractive object.
             */
            virtual ~TestRunnerNonInteractive();

            /**
             * Run all tests.
             */
            void startTests();

            /**
             * This function will be called by a test case after execution. 
             * Complete the event the test case has fired.
             */
            void handleEvent( CallbackInterface* ei );

            /**
             * Start the next test case. If all test cases are executed, 
             * print all errors and fire the event from the caller of the
             * TestRunner object to signal that the test runner has finished.
             */
            void completeEvent( );

        private:

            /**
             * The runner's messages go here.
             */
            WriteInterface* output;

            /**
             * The test suite that has to be executed.
             */
            TestSuite* testSuite;

            /**
             * The event used to warn the TestRunner that a test has completed.
             */
            Event* event;

            /**
             * The event that the TestRunner uses to warn that it has finished it's work.
             */
            EventOperationInterface* fEvent;

            /**
             * Print a message describing which tests will be executed.
             */
            void printWelcomeMessage();

            /**
             * Start a test.
             * @param test  The test to be executed.
             * @pre   <thread> isn't running already.
             */
            void startTest( TestCaseInterface* test );

            /**
             * Do a test and fire completion if necessary.
             */
            void doTest();
            
            /**
             * Stop a running test.
             * @deprecated
             */ 
            //void stopTest(ComponentThreaded* thread);

            /**
             * List all tests.
             */
            void listTests();

            /**
             * Print the errors of a test.
             * @param test  The test.
             */
            void printErrors( TestCaseInterface* test );

            /**
             * Print the errors of the tests.
             */
            void printErrors();

            /**
             * Print a summary of the results of the tests.
             */
            void printSummary();

    };

}

#endif
