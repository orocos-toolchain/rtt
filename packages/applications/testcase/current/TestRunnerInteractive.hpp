/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestRunnerInteractive.hpp 

                        TestRunnerInteractive.hpp -  description
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
 
/* XXX An interactive test runner is not needed, but we can reuse part of the functionality
*     in the applications package later.
*/
#ifndef TESTRUNNERINTERACTIVE_HPP
#define TESTRUNNERINTERACTIVE_HPP

#include <corelib/EventListenerInterface.hpp>
#include <corelib/EventCompleterInterface.hpp>
#include <os/PeriodicThread.hpp>

#include "TestRunner.hpp"
#include "ObservableReadInterface.hpp"
#include "TestCase.hpp"

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    /**
     * This class executes TestCase objects in an interactive manner. Only one TestCase can be executed at a time.
     * It accepts the follwing commands, where <name> is the name of a TestCase:
     *
     * start <name>  Start the TestCase named <name>.
     * stop   Stop the running TestCase.
     * list   List the available tests.
     * error <name>  List the errors of the TestCase named <name>.
     * errors  List the errors of all executed tests.
     * summary  Message how many tests failed.
     */

    class TestRunnerInteractive :
                public TestRunner,
                public EventListenerInterface,
                public EventCompleterInterface
    {

        public:
            /**
             * Constructs a TestRunnerInteractive object.
             * @param in  The input for the test runner.
             * @param out  The output of the test runner.
             * @param suite  The suite containg the test cases that have to be executed.
             */
            TestRunnerInteractive( ObservableReadInterface* in,
                                   WriteInterface* out,
                                   TestSuite* suite );

            /**
             * Destroys a TestRunnerInteractive object.
             */
            ~TestRunnerInteractive();

            /**
             * Read the command string from the ObservableReadInterface.
             */
            void handleEvent( EventInterface* ei );

            /**
             * Parse the command string and react appropiately.
             */
            void completeEvent( EventInterface* ei );

        private:
            /**
             * This is the source of the runner's commands.
             */
            ObservableReadInterface* input;

            /**
             * The runner's messages go here.
             */
            WriteInterface* output;

            /**
             * The test suite that has to be executed.
             */
            TestSuite* testSuite;

            /**
             * The thread to execute a test.
             */
            ORO_OS::PeriodicThread* thread;

            /**
             * The length of the command string buffer.
             */
            int bufSize;

            /**
             * The buffer for the command strings.
             */
        char* buf;

            /**
             * The size of the last read command string.
             */
            int cmdSize;

            /**
             * Print a message describing which tests can be executed.
             */
            void printWelcomeMsg();

            /**
             * Start a test.
             * @param name  The name of the test to be runned.
             */
            void startTest( const char* name );

            /**
             * Start a test.
             * @param test  The test to be runned.
             */
            void startTest( TestCase* test );

            /**
             * Stop the running test.
             */
            void stopTest();

            /**
             * Run all tests.
             */ 
            //void runTests();

            /**
             * List all tests.
             */
            void listTests();

            /**
             * Print the errors of a test.
             * @param name  The name of the test.
             */
            void printErrors( const char* name );

            /**
             * Print the errors of a test.
             * @param test  The test.
             */
            void printErrors( TestCase* test );

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
