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
