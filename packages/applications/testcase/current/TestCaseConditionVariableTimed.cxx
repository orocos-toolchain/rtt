#include "corelib/TestCaseConditionVariableTimed.hpp"
#include <corelib/ConditionBool.hpp>

/*namespace UnitTesting
{

    TestCaseConditionVariableTimed::TestCaseConditionVariableTimed( TestSuite* suite ) :
        TestCase( "TestCaseConditionVariableTimed", suite, 1 ),
        cv1( &condition1, &lock , 0.1),cv2( &condition2, &lock , 0.1 ),
        errorValueNotTrue( "The value should be true but is not.\n" ),
        errorValueNotFalse( "The value should be false but is not.\n" )
    {
    }

    bool TestCaseConditionVariableTimed::initialize()
    {
    }

    void TestCaseConditionVariableTimed::step()
    {
        double timeout;
        bool value;

        // Make a ConditionVariableTimed object and test if it's initialized properly.
        value = ( cv1.conditionGet() ) ->evaluate();

        testAssert( !value, errorValueNotFalse );


        // Make a ConditionVariableTimed object and test if a value is set properly.
        {

            MutexLock locker( lock );
            condition2.setTrue();

            cv2.broadcast();
        }

        // The next evaluate() on the ConditionBool object should be true.
        testAssert( &cv2, errorValueNotTrue );
        return true; 
    }

    void TestCaseConditionVariableTimed::finalize()
    {
    }

    TestCaseConditionVariableTimed::~TestCaseConditionVariableTimed()
    {
    }

}*/

