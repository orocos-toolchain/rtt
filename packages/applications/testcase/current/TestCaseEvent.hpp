/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseEvent.hpp 

                        TestCaseEvent.hpp -  description
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
 

#ifndef TESTCASEEVENT_HPP
#define TESTCASEEVENT_HPP

#include "TestCase.hpp"

#include <corelib/Event.hpp>
#include <corelib/HeartBeatGenerator.hpp>

namespace UnitTesting
{

    using namespace ORO_CoreLib;
    using std::string;

    /**
     * Tests all kinds of Events.
     * @todo The test is currently not working because of failing ConditionVariable
     *       implementation. It just fires two events and concludes that everything
     *       is fine.
     */
    class TestCaseEvent 
        : public TestCase, public EventListenerInterface, public EventCompleterInterface
    {

        public:
            TestCaseEvent( TestSuite* suite );

            ~TestCaseEvent();

            virtual bool initialize();

            virtual  void step();

            virtual void finalize();

            virtual void handleEvent(CallbackInterface*);
            virtual void completeEvent();

        private:
            // Forbidden!!!
            TestCaseEvent();

            /**
             * handleEvent is not called for SYNSYN event.
             */
            string errorNoHandleEventSYNSYN;

            /**
             * completeEvent is not called for SYNSYN event.
             */
            string errorNoCompleteEventSYNSYN;

            /**
             * handleEvent is not called for SYNASYN event.
             */
            string errorNoHandleEventSYNASYN;

            /**
             * completeEvent is not called for SYNASYN event.
             */
            string errorNoCompleteEventSYNASYN;

    /**
     * An event that will be synchronously handled and completed.
     */
     Event event;

    /**
     * An event that will be synchronously handled and asynchronously completed.
     */
     Event eventSA;

    /**
     * True if the SYNSYN event is handled by an EventListener.
     */
     bool eventSYNSYNHandled;

    /**
     * True if the SYNSYN event is completed by an EventCompleter.
     */
     bool eventSYNSYNCompleted;

    /**
     * True if the SYNASYN event is handled by an EventListener.
     */
     bool eventSYNASYNHandled;

    /**
     * Value is true if the SYNASYN event is completed by an EventCompleter.
     */
     //ConditionBool eventSYNASYNCompleted;

    /**
     * The mutex to protect the eventSYNASYNCompleted condition.
     */
     //ORO_OS::Mutex mutexEventSYNASYNCompleted;

    /**
     * The condition variable to get the eventSYNASYNCompleted condition
     */
     //ConditionVariableTimed waitEventSYNASYNCompleted;

    };

}

#endif // TESTCASEEVENT_HPP
