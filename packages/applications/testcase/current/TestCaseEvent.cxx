#include "TestCaseEvent.hpp"

namespace UnitTesting
{

    TestCaseEvent::TestCaseEvent( TestSuite* suite ) :
        TestCase( "TestCaseEvent", suite, 1 ), 
        errorNoHandleEventSYNSYN( "handleEvent is not called for SYNSYN event.\n" ),
        errorNoCompleteEventSYNSYN( "completeEvent is not called for SYNSYN event.\n" ),
        errorNoHandleEventSYNASYN( "handleEvent is not called for SYNASYN event.\n" ),
        errorNoCompleteEventSYNASYN( "completeEvent is not called for SYNASYN event.\n" ),
        event( Event::SYNSYN ),eventSA( Event::SYNASYN )
        {    }

    TestCaseEvent::~TestCaseEvent()
    {
    }

    bool TestCaseEvent::initialize()
    {
        // Registering objects to be notified on event...
        event.addHandler( this, this );
        eventSA.addHandler( this, this );
        return true;
    }

    void TestCaseEvent::step()
    {
        /*
         * Firing SYNSYN event...
         */
        event.fire();
        /*
         * Firing SYNASYN event...
         */
        eventSA.fire();
    }

    void TestCaseEvent::finalize()
    {
        /*
        // Check if the event is handled and completed.
        testAssert( eventSYNSYNHandled, errorNoHandleEventSYNSYN );
        testAssert( eventSYNSYNCompleted, errorNoCompleteEventSYNSYN );

        // Check if the event is handled.
        testAssert( eventSYNASYNHandled, errorNoHandleEventSYNASYN );
        // Test completion of the SYNASYN event with a ConditionVariableTimed object.        
        testAssert( &waitEventSYNASYNCompleted, errorNoCompleteEventSYNASYN );
        */
    }

    void TestCaseEvent::handleEvent( CallbackInterface* ei )
    {
        ei->complete();
    }

    void TestCaseEvent::completeEvent()
    {
    }
}
