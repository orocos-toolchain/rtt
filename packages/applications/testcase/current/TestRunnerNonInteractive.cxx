
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




