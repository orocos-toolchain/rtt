/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestRunnerInteractive.cxx 

                        TestRunnerInteractive.cxx -  description
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

#include "corelib/TestRunnerInteractive.hpp"

/* XXX An interactive test runner is not needed, but we can reuse part of the functionality
 *     in the applications package later.
 */

namespace UnitTesting
{

    TestRunnerInteractive::TestRunnerInteractive(
        ObservableReadInterface* in,
        WriteInterface* out,
        TestSuite* suite ) :
        input( in ), output( out ), testSuite( test ), bufSize(128)
    {
        buf = new char[ bufSize ];
        input.addHandler( this, this );
        // TODO: Make a new thread?
        printWelcomeMsg();
    }

    TestRunnerInteractive::~TestRunnerInteractive()
    {
        input.removeHandler( this );
        delete[] buf;

        if ( thread )
            delete thread;
    }

    void TestRunnerInteractive::handleEvent( EventInterface* ei )
    {
        cmdSize = in.read( buf, bufSize );
        buf[ cmdSize ] = '\0';

        if ( cmdSize > 0 )
        {
            ei->complete( this );
        }
    }

    void TestRunnerInteractive::completeEvent( EventInterface* ei )
    {
        char arg[ 30 ];

        if ( sscanf( buf, "start %s", arg ) )
        {
            startTest( arg );
        }

        else if ( strcmp( buf, "stop" == 0 ) )
        {
            stopTest();
        }

        else if ( strcmp( buf, "list" == 0 ) )
        {
            listTests();
        }

        else if ( sscanf( buf, "error %s", arg ) )
        {
            printErrors( arg );
        }

        else if ( strcmp( buf, "errors" == 0 ) )
        {
            printErrors();
        }

        else if ( strcmp( buf, "summary" == 0 ) )
        {
            printSummary();
        }

    }

    void TestRunnerInteractive::printWelcomeMessage()
    {
        const char * name;
        char welcome[] = "The following tests will be executed:";
        output.write( welcome, strlen( welcome ) + 1 );

        listTests();
    }

    void TestRunnerInteractive::listTests()
    {
        testSuite.reset();

        while ( !testSuite.atEnd() )
        {
            name = testSuite.getNextTest() ->nameGet();
            output.write( name, strlen( name ) + 1 );
        }
    }

    void TestRunnerInteractive::startTest( const char* name )
    {
        startTest( testSuite.testGet( name ) );
    }

    void TestRunnerInteractive::startTest( TestCase* test )
    {
        if ( thread )
        {
            char[] message = "A test case is running. Stop it first.";
            output.write( message, strlen( message ) + 1 );
        }

        else
        {
            // TODO: avoid using a new thread?
            thread = new PeriodicThread( test );
            thread->taskNameSet( test->caseNameGet() );
            thread->start();
        }
    }

    void TestRunnerInteractive::stopTest()
    {
        thread->terminate();
        // TODO: Delete thread?
        delete thread;
    }

    /*void runTests()
    {
     testSuite.reset();
     while(!testSuite.atEnd())
     {
      runTest(testSuite.getNextTest());
     }
    }*/

    void TestRunnerInteractive::printErrors( const char* name )
    {
        printErrors( testSuite.testGet( name ) );
    }

    void TestRunnerInteractive::printErrors( TestCase* test )
    {
        vector<string*>* errors = test->errorsGet();
        const char* errorStr;

        for ( vector<string*>::iterator i = errors->begin(); i != errors->end(); i++ )
        {
            errorStr = ( *i ) ->data();
            output.write( errorStr, strlen( errorStr ) + 1 );
        }

        sprintf( errorStr, "Number of errors: %d", test->errorsNumber() );
        output.write( errorStr, strlen( errorStr ) + 1 );
    }

    void TestRunnerInteractive::printErrors()
    {
        const char * name;
        TestCase* test;

        testSuite.reset();

        while ( !testSuite.atEnd() )
        {
            test = testSuite.getNextTest();
            name = test->caseNameGet();
            output.write( name, strlen( name ) + 1 );
            printErrors( test );
        }

        printSummary();
    }

    void TestRunnerInteractive::printSummary()
    {
        int failed = testSuite.testsFailed();
        int total = testSuite, testsNumber();

        if ( failed )
        {
            char message[];
            sprintf( message, "%d out of %d tests FAILED.", failed, total );
            output.write( message, strlen( message ) + 1 );
        }

        else
        {
            char message[];
            sprintf( message, "All %d tests PASSED.", total );
            output.write( message, strlen( message ) + 1 );
        }
    }

}




