/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseEventInterrupt.hpp 

                        TestCaseEventInterrupt.hpp -  description
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
 
#ifndef TESTCASEEVENTINTERRUPT_HPP
#define TESTCASEEVENTINTERRUPT_HPP

#include "TestCase.hpp"
#include <os/fosi.h>

#include <os/EventInterrupt.hpp>
#include <os/ISRInterface.hpp>
#include <os/DSRInterface.hpp>

extern "C" void hw_init( void );

#ifdef __KERNEL__
extern "C" void parport_init( void );
#endif

namespace UnitTesting
{

    //using namespace std;

    using namespace ORO_CoreLib;


    class ParallelTop : public ORO_OS::ISRInterface
    {

        public:

            virtual void handleInterrupt( int irq, void* dev_id, struct pt_regs* regs );
    };

    class ParallelBottom : public ORO_OS::DSRInterface
    {
            virtual void completeInterrupt();
    };


    /**
     * Test event interrupts.
     */

    class TestCaseEventInterrupt :
                public TestCase
    {

        public:
            TestCaseEventInterrupt( TestSuite* suite );

            ~TestCaseEventInterrupt();

            virtual bool initialize();

            virtual  void step();

            virtual void finalize();

        private:
            // Forbidden!!!
            TestCaseEventInterrupt();

            /**
             * handleEvent is not called for event interrupt.
             */
            std::string errorNoHandleEventInterrupt;

            /**
             * completeEvent is not called for event interrupt.
             */
            std::string errorNoCompleteEventInterrupt;


    };

}

#endif // TESTCASEEVENTINTERRUPT_HPP
