/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestMain.hpp 

                        TestMain.hpp -  description
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
 
#ifndef TESTMAIN_HPP
#define TESTMAIN_HPP

#include <corelib/EventInterfaces.hpp>
#include <corelib/EventListenerInterface.hpp>

namespace UnitTesting
{

    using namespace ORO_CoreLib;

    void testTerminate( void );
    int testInit( int );

    /**
     * The return value of the program.
     */
    static int retValue;

    /**
     * A helper class for watching an event which indicates
     * that the tests are completed.
     */
    class TestMain :
                public EventListenerInterface,
                public EventCompleterInterface
    {

        public:

            /**
             * Construct a new TestMain object.
             * @param ei  The event that the TestRunner should
             *    fire when it's finished.
             */
            TestMain( EventInterface* ei );

            /**
             * Destroy the TestMain object.
             */
            ~TestMain();

            /**
             * Handle the event from the TestRunner, that is,
             * call completeEvent.
             */
            void handleEvent( CallbackInterface* event );

            /**
             * Complete the event from the TestRunner, that is,
             * exit the program when the TestRunner has done its
             * work.
             */
            void completeEvent();

            /**
             * true if it's time to quit the application.
             */
            bool timeToQuit();

        private:
            /**
             * The event that the TestRunner uses to warn TestMain that it's finished.
             */
            EventInterface* qEvent;

            /**
             * true if it's time to quit the program.
             */
            bool ttq;
    };

}

#endif // TESTMAIN_HPP
