/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseEvent.cxx 

                        TestCaseEvent.cxx -  description
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
