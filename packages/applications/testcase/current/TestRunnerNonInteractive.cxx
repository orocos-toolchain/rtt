/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestRunnerNonInteractive.cxx 

                        TestRunnerNonInteractive.cxx -  description
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

#include "TestRunnerNonInteractive.hpp"

namespace UnitTesting
{

    TestRunnerNonInteractive::TestRunnerNonInteractive( WriteInterface* out, TestSuite* suite, EventOperationInterface* finishEvent ) :
            output( out ), testSuite( suite ), fEvent( finishEvent )
    {
        event = new Event( Event::SYNASYN );
        event->addHandler( this, this );
        printWelcomeMessage();
    }

    TestRunnerNonInteractive::~TestRunnerNonInteractive()
    {
        delete event;
    }

    void TestRunnerNonInteractive::printWelcomeMessage()
    {
        char welcome[] = "The following tests will be executed:\n";
        output->write( welcome, strlen( welcome ) + 1 );

        listTests();
    }

    void TestRunnerNonInteractive::listTests()
    {
        const char * name;
        testSuite->reset();

        while ( !testSuite->atEnd() )
        {
            name = testSuite->getNextTest() ->caseNameGet();
            output->write( name, strlen( name ) + 1 );
            output->write( "\n", 2 );
        }
    }

    void TestRunnerNonInteractive::startTest( TestCaseInterface* test )
    {
        output->write( "Starting ",10 );
        output->write( test->caseNameGet(), strlen(test->caseNameGet())+1 );
        output->write( "\n",2 );
        test->start( event );
    }

    void TestRunnerNonInteractive::startTests()
    {
        testSuite->reset();
        // Start the first test case of <testSuite>.
        doTest();
    }

    void TestRunnerNonInteractive::handleEvent( CallbackInterface* ei )
    {
        ei->complete();
    }

    void TestRunnerNonInteractive::completeEvent( )
    {
        doTest();
    }

    void TestRunnerNonInteractive::doTest()
    {
        if ( !testSuite->atEnd() )
        {
            // Start the next test case of <testSuite>.
            startTest( testSuite->getNextTest() );
        }

        else
        {
            // All test cases done, print errors now.
            printErrors();
            // Warn the caller we have done our business.

            if ( fEvent )
                fEvent->fire();
        }
    }

    void TestRunnerNonInteractive::printErrors( TestCaseInterface* test )
    {
        std::vector<const std::string*>* errors = test->errorsGet();
        const char* errorStr;

        char message[ 50 ];
        sprintf( message, "%d errors in %s\n", test->errorsNumber(), test->caseNameGet() );
        output->write( message, strlen( message ) + 1 );

        for ( std::vector<const std::string*>::iterator i = errors->begin(); i != errors->end(); i++ )
        {
            errorStr = ( *i ) ->c_str();
            output->write( errorStr, strlen( errorStr ) + 1 );
        }

    }

    void TestRunnerNonInteractive::printErrors()
    {
        TestCaseInterface * test;

        testSuite->reset();

        while ( !testSuite->atEnd() )
        {
            test = testSuite->getNextTest();
            printErrors( test );
        }

        printSummary();
    }

    void TestRunnerNonInteractive::printSummary()
    {
        int failed = testSuite->testsFailed();
        int total = testSuite->testsNumber();

        if ( failed )
        {
            char message[ 50 ];
            sprintf( message, "%d out of %d tests FAILED.\n", failed, total );
            output->write( message, strlen( message ) + 1 );
        }

        else
        {
            char message[ 50 ];
            sprintf( message, "All %d tests PASSED.\n", total );
            output->write( message, strlen( message ) + 1 );
        }
    }

}




