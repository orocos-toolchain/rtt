/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  TestCaseEventInterrupt.cxx 

                        TestCaseEventInterrupt.cxx -  description
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
#include "TestCaseEventInterrupt.hpp"

#include <iostream>

#include <pkgconf/system.h>

// this will not work on any platform, should probe irqs etc
#ifdef OROINT_OS_KERNEL
void hw_init()
{
    parport_init();
}

#else

void hw_init()
{
    //    outb_p(inb_p(0x37A) | 0x10, 0x37A);
    std::cout << "Hardware *not* initialized!\n";
}

#endif

namespace UnitTesting
{
    static ORO_OS::EventInterrupt* tester;
    static ParallelTop* top;
    static ParallelBottom* bottom;

    /**
     * True if the event interrupt is handled by an EventListener.
     */
    static bool eventInterruptHandled = false;

    /**
     * True if the event interrupt is completed by an EventListener.
     */
    static bool eventInterruptCompleted = false;

    TestCaseEventInterrupt::TestCaseEventInterrupt( TestSuite* suite ) :
        TestCase( "TestCaseEventInterrupt", suite, 1 ),
        errorNoHandleEventInterrupt( "handleEvent is not called for event interrupt.\n" ),
        errorNoCompleteEventInterrupt( "completeEvent is not called for event interrupt.\n" )
    {
        tester = new ORO_OS::EventInterrupt();
        top = new ParallelTop();
        bottom = new ParallelBottom();
        //timeToWait.tv_sec = 0;
        //timeToWait.tv_nsec = 999999999;
    }

    TestCaseEventInterrupt::~TestCaseEventInterrupt()
    {
        delete tester;
        delete top;
        delete bottom;
    }

    bool TestCaseEventInterrupt::initialize()
    {
        tester->IRQSet( 7 );
        tester->addHandler( top, bottom );
        tester->hardwareInitSet( hw_init );
        tester->IRQRequest();
        return true;
    }

    void TestCaseEventInterrupt::step()
    {
        // the interrupt should be running now
        // dont do this in your actual programs, it's cheating
        // tester->interrupt( 666 );
    }

    void TestCaseEventInterrupt::finalize()
    {
        // TODO: use condition variables, as in TestCaseEvent, and do the asserts in step().
        testAssert( eventInterruptHandled, errorNoHandleEventInterrupt );
        testAssert( eventInterruptCompleted, errorNoCompleteEventInterrupt );
        tester->fire();
        tester->disable();
        tester->IRQFree();
    }

    void ParallelTop::handleInterrupt( int irq, void* dev_id, struct pt_regs* regs )
    {
        eventInterruptHandled = true;
        ( ( ORO_OS::EventInterrupt* ) dev_id ) ->complete( this );
    }

    void ParallelBottom::completeInterrupt()
    {
        eventInterruptCompleted = true;
    }

}
