#include "TestCaseSimple.hpp"

#include <os/fosi.h>
#include <iostream>

namespace UnitTesting
{
    using namespace std;

    TestCaseSimple::TestCaseSimple( TestSuite* suite, WriteInterface* output ) 
        : TestCase( "TestCaseSimple", suite, 13 ), out( output ), stepCounter( 0 ),
          errorAssert( "The assert construction doesn't work.\n" ),
          errorStep( "Step doesn't work.\n" )
    {
    }

    TestCaseSimple::~TestCaseSimple()
    {
        cout <<"TestCaseSimple::~TestCaseSimple"<<endl;
    }

    bool TestCaseSimple::initialize()
    {
        char message[ 50 ] = "Initialize TestCaseSimple...\n";
        out->write( message, strlen( message ) + 1 );
        return true;
    }

    void TestCaseSimple::step()
    {
        rtos_printf(".");
        stepCounter++;
        // Test if the assert construction works.
        isSet = true;
        testAssert( isSet, errorAssert );

    }

    void TestCaseSimple::finalize()
    {
        testAssert( stepCounter == 13, errorStep );
        char message[ 50 ] = "Finalize TestCaseSimple...\n";
        out->write( message, strlen( message ) + 1 );
    }

}
