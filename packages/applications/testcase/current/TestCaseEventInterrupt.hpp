/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  TestCaseEventInterrupt.hpp 

                        TestCaseEventInterrupt.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
