/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:26:02 CEST 2006  generictask_test.cpp

                        generictask_test_3.cpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unit.hpp"

#include <iostream>

#include <TaskContext.hpp>
#include <extras/SlaveActivity.hpp>
#include <extras/SequentialActivity.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SimulationThread.hpp>

#include <boost/function_types/function_type.hpp>
#include <OperationCaller.hpp>

using namespace std;
using namespace RTT;
using namespace RTT::detail;

struct A {};


// Test TaskContext states.
class StatesTC
    : public RTT::TaskContext
{
public:
    StatesTC()
        : TaskContext("TC", PreOperational)
    {
        BOOST_CHECK( this->getTaskState() == TaskContext::PreOperational );
        BOOST_CHECK( this->getTargetState() == TaskContext::PreOperational );

        this->resetFlags();
        validconfig = true;
        validstart = true;
        do_error = false;
        do_fatal = false;
        do_throw=false;
        do_throw2=false;
        do_throw3=false;
        do_trigger=false;
        do_breakUH=false;
        do_block=false;
        do_checks=true;
        do_stop=false;
    }

    void resetFlags()
    {
        didconfig = false;
        didstart = false;
        didstop = false;
        didcleanup = false;
        didupdate = false;
        diderror = false;
        didexcept = false;
        didbreakUH = false;
        updatecount=0;
        do_stop=false;
    }

    bool configureHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState <= Stopped );
            BOOST_CHECK( getTargetState() == Stopped );
        }
        didconfig = true;
        return validconfig;
    }

    bool startHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState == Stopped);
            BOOST_CHECK( getTargetState() == Running );
        }
        didstart = true;
        return validstart;
    }

    void stopHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState >= Running || mTaskState == Exception);
            BOOST_CHECK( getTargetState() == Stopped || getTargetState() == Exception );
        }
        didstop = true;
    }

    void cleanupHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState == Stopped || mTaskState == Exception);
            BOOST_CHECK( getTargetState() == PreOperational || getTargetState() == Exception );
        }
        didcleanup = true;
    }

    void exceptionHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState == Exception);
            BOOST_CHECK( getTargetState() == Exception );
        }
        didexcept = true;
        if (do_throw3)
            throw A();
    }

    void updateHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState == Running );
            BOOST_CHECK( getTargetState() == Running );
        }
        didupdate = true;
        updatecount++;
        if (do_fatal)
            this->fatal();
        if (do_error)
            this->error();
        if (do_throw)
            throw A();
        if (do_trigger) {
            this->trigger();
            do_trigger = false; //avoid endless loop
        }
        if (do_block)
            usleep(1*1000*1000);
        if (do_stop)
            this->stop();
    }

    bool breakUpdateHook() {
        didbreakUH = true;
        return do_breakUH;
    }

    void errorHook() {
        if (do_checks) {
            BOOST_CHECK( mTaskState == RunTimeError );
            BOOST_CHECK( getTargetState() == RunTimeError );
        }
        diderror = true;
        if (do_fatal)
            this->fatal();
        if (do_throw2)
            throw A();
    }

    bool validconfig, didconfig;
    bool validstart, didstart;
    bool didstop;
    bool didcleanup;
    bool didupdate,diderror,didexcept, didbreakUH;
    bool do_fatal, do_error, do_throw,do_throw2,do_throw3, do_trigger, do_breakUH, do_block, do_checks, do_stop;
    int  updatecount;
};

/**
 * Fixture.
 */
class TaskStates_Test
{
public:
    TaskContext* tc;
    StatesTC* stc;
    ActivityInterface* tsim;
    ActivityInterface* stsim;

public:
    TaskStates_Test()
    {
        tc =  new TaskContext( "root", TaskContext::Stopped );
        stc = new StatesTC();
        tc->setActivity( new SimulationActivity(0.001) );
        stc->setActivity( new SimulationActivity(0.001) );
        tsim = tc->getActivity();
        stsim = stc->getActivity();
        SimulationThread::Instance()->stop();
    }

    ~TaskStates_Test()
    {
        tsim->stop();
        stsim->stop();
        delete tc;
        delete stc;
    }
};


// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE(  TaskStatesSuite,  TaskStates_Test )


/**
 * Tests the initial period of a TC after setting an activity.
 */
BOOST_AUTO_TEST_CASE( testPeriod)
{
    // check unconfigured TC
    TaskContext pertc("PerTC");
    BOOST_CHECK( pertc.getPeriod() == 0.0 );
    BOOST_CHECK( pertc.isActive() );

    // check periodic TC
    pertc.setActivity( new SlaveActivity(1.0) );
    BOOST_CHECK( pertc.engine()->getActivity()->getPeriod() == 1.0 );
    BOOST_CHECK( pertc.getPeriod() == 1.0 );

    // check non periodic TC
    pertc.setActivity( new SlaveActivity(0.0) );
    BOOST_CHECK( pertc.engine()->getActivity()->getPeriod() == 0.0 );
    BOOST_CHECK( pertc.getPeriod() == 0.0 );
}

/**
 * Tests the trigger() from updateHook().
 */
BOOST_AUTO_TEST_CASE( testTrigger )
{
    // Check default TC
    StatesTC trigtc;
    trigtc.do_checks = false;
    BOOST_CHECK( trigtc.getPeriod() == 0.0 );
    BOOST_CHECK( trigtc.isActive() );
    BOOST_CHECK( trigtc.trigger() == true );
    usleep(100*1000);
    // don't call updatehook when not running:
    BOOST_CHECK_EQUAL( trigtc.updatecount, 0 );

    BOOST_CHECK( trigtc.configure() );
    BOOST_CHECK( trigtc.trigger() == true );
    usleep(100*1000);
    // don't call updatehook when not running:
    BOOST_CHECK_EQUAL( trigtc.updatecount, 0 );
    BOOST_CHECK( trigtc.start() );
    BOOST_CHECK( trigtc.didstart == true );
    usleep(100*1000);

    // test updateHook() after start():
    BOOST_CHECK_EQUAL( trigtc.updatecount, 1 );

    trigtc.resetFlags();

    // trigger() after start():
    BOOST_CHECK( trigtc.trigger() );
    usleep(100*1000);
    BOOST_CHECK( trigtc.didstart == false );
    BOOST_CHECK_EQUAL( trigtc.updatecount, 1 );

    // again:
    BOOST_CHECK( trigtc.trigger() );
    usleep(100*1000);
    BOOST_CHECK_EQUAL( trigtc.updatecount, 2 );

    // this calls trigger() twice: once ourselves,
    // and once from updateHook().
    trigtc.do_trigger = true;
    BOOST_CHECK( trigtc.trigger() );
    usleep(100*1000);
    BOOST_CHECK_EQUAL( trigtc.updatecount, 4 );
    BOOST_CHECK( trigtc.do_trigger == false );

    // Check periodic TC ( rejects trigger() ):
    TaskContext pertc("test");
    pertc.setPeriod( 0.1 );
    BOOST_CHECK_EQUAL( pertc.getPeriod(), 0.1 );
    BOOST_CHECK( pertc.trigger() == false );
    BOOST_CHECK( pertc.configure() == true );
    BOOST_CHECK( pertc.trigger() == false );
    BOOST_CHECK( pertc.start() == true );
    BOOST_CHECK( pertc.trigger() == false );
}

/**
 * Tests the breakUpdateHook() calling from stop().
 */
BOOST_AUTO_TEST_CASE( testBreakUpdateHook )
{
    // Check default TC + blocking updateHook
    StatesTC breaktc;
    breaktc.do_checks = false;
    BOOST_CHECK( breaktc.getPeriod() == 0.0 );
    BOOST_CHECK( breaktc.isActive() );
    breaktc.do_block = true;
    BOOST_CHECK( breaktc.configure() );
    BOOST_CHECK( breaktc.start() );
    usleep(100*1000);
    // we're blocking + breakUpdateHook() == false, so stop() fails:
    BOOST_CHECK_EQUAL( breaktc.didupdate, true );
    BOOST_CHECK_EQUAL( breaktc.stop(), false );
    BOOST_CHECK_EQUAL( breaktc.didbreakUH, true);
    BOOST_CHECK_EQUAL( breaktc.didstop, false);
    breaktc.didbreakUH = false;
    breaktc.do_breakUH = true;
    breaktc.do_trigger = true;
    // we're blocking *and* implementing breakUpdateHook():
    BOOST_CHECK_EQUAL( breaktc.stop(), true );
    BOOST_CHECK_EQUAL( breaktc.didbreakUH, true);
    BOOST_CHECK_EQUAL( breaktc.didstop, true);
}

/**
 * Tests the calling stop() from updateHook().
 */
BOOST_AUTO_TEST_CASE( testStopInUpdateHook )
{
    // Check default TC + stop in UH
    StatesTC breaktc;
    breaktc.do_checks = false;
    breaktc.do_stop = true;
    BOOST_CHECK( breaktc.getPeriod() == 0.0 );
    BOOST_CHECK( breaktc.isActive() );
    BOOST_CHECK( breaktc.configure() );
    BOOST_CHECK( breaktc.start() );
    usleep(100*1000);
    // check a failed stop() from UH:
    breaktc.do_breakUH = false;
    BOOST_CHECK_EQUAL( breaktc.didupdate, true );
    BOOST_CHECK_EQUAL( breaktc.didbreakUH, true);
    BOOST_CHECK_EQUAL( breaktc.didstop, false);
    BOOST_CHECK_EQUAL( breaktc.isRunning(), true);

    // check a succesful stop() from UH:
    breaktc.do_breakUH = true;
    breaktc.didbreakUH = false;
    breaktc.didstop = false;
    breaktc.didupdate = false;
    BOOST_CHECK( breaktc.trigger() );
    usleep(100*1000);
    BOOST_CHECK_EQUAL( breaktc.didupdate, true );
    BOOST_CHECK_EQUAL( breaktc.didbreakUH, true);
    BOOST_CHECK_EQUAL( breaktc.didstop, true);
    BOOST_CHECK_EQUAL( breaktc.isRunning(), false);
}

/**
 * Tests the setPeriod function
 */
BOOST_AUTO_TEST_CASE( testSetPeriod )
{
    TaskContext pertc("test");
    BOOST_CHECK( pertc.setPeriod( 0.1 ) );
    BOOST_CHECK_EQUAL( pertc.getPeriod(), 0.1 );
    BOOST_CHECK( pertc.configure() == true);
    BOOST_CHECK( pertc.setPeriod( 0.2 ) );
    BOOST_CHECK_EQUAL( pertc.getPeriod(), 0.2 );
    BOOST_CHECK( pertc.start() == true );

    BOOST_CHECK( pertc.setPeriod( 0.5 ) );
    BOOST_CHECK_EQUAL( pertc.getPeriod(), 0.5 );
}

/**
 * Tests the normal TC states.
 */
BOOST_AUTO_TEST_CASE( testTCStates)
{
    // Test the states of a Default TC, no user methods.
    BOOST_CHECK( tc->getTaskState() == TaskContext::Stopped );

    // Configure in Stop
    BOOST_CHECK( tc->isConfigured() == true );
    BOOST_CHECK( tc->isRunning() == false );
    BOOST_CHECK( tc->isActive() == true );
    BOOST_CHECK( tc->configure() == true );

    // Stop to Running
    BOOST_CHECK( tc->start() == true );
    BOOST_CHECK( tc->isRunning() == true );
    BOOST_CHECK( tc->isActive() == true );
    BOOST_CHECK( tc->configure() == false );
    BOOST_CHECK( tc->start() == false );
    BOOST_CHECK( tc->cleanup() == false );

    // Running to Stop
    BOOST_CHECK( tc->stop() == true );
    BOOST_CHECK( tc->isRunning() == false);
    BOOST_CHECK( tc->isActive() == true );
    BOOST_CHECK( tc->stop() == false );
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->isConfigured() == true );

    // Engine Stop to Active to Stop
    BOOST_CHECK( tc->engine()->getActivity()->stop() == true);
    BOOST_CHECK( tc->isActive() == false);
    BOOST_CHECK( tc->activate() == true );
    BOOST_CHECK( tc->isActive() == true);
    BOOST_CHECK( tc->isRunning() == false );
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->isConfigured() == true );

    // Stop to PreOp
    BOOST_CHECK( tc->cleanup() == true );
    BOOST_CHECK( tc->isConfigured() == false);
    BOOST_CHECK( tc->isRunning() == false);

    // PreOp to stop
    BOOST_CHECK( tc->configure() == true );
    BOOST_CHECK( tc->isConfigured() == true);
    BOOST_CHECK( tc->isRunning() == false);
    BOOST_CHECK( tc->isActive() == true );

}

/**
 * Tests if every hook was called.
 */
BOOST_AUTO_TEST_CASE( testSpecialTCStates)
{
    // Test the states of a Specially crafted TC, requiring configure etc.
    BOOST_CHECK( stc->getTaskState() == TaskContext::PreOperational );

    // PreOperational state:
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == true );
    // test flags
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    stc->resetFlags();

    // Stopped state:
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isRunning() == true );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    stc->resetFlags();


    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == true );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    stc->resetFlags();

    // Back to stopped
    BOOST_CHECK( stc->stop() == true );
    BOOST_CHECK( stc->didstop == true );
    BOOST_CHECK( stc->isRunning() == false );
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didcleanup == false );
    stc->resetFlags();

    // Reconfigure in stopped
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == true );
    // test flags
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didstop == false );
    BOOST_CHECK( stc->didcleanup == false );
    stc->resetFlags();

    // Stopped to PreOp state:
    BOOST_CHECK( stc->cleanup() == true );
    BOOST_CHECK( stc->didcleanup == true );
    BOOST_CHECK( stc->isConfigured() == false);
    // test flags
    BOOST_CHECK( stc->didconfig == false );
    BOOST_CHECK( stc->didstart == false );
    BOOST_CHECK( stc->didupdate == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->didstop == false );
    stc->resetFlags();
}

/**
 * Tests the Error states
 */
BOOST_AUTO_TEST_CASE( testFailingTCStates)
{
    // Test the states of a TC failing in transitions
    stc->validconfig = false;
    stc->validstart = false;

    // Failed configure() in PreOperational state:
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == true );
    stc->resetFlags();

    // Retry:
    stc->validconfig = true;
    BOOST_CHECK( stc->configure() == true );
    BOOST_CHECK( stc->didconfig == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == true );
    stc->resetFlags();

    // Failed start() in Stopped state:
    BOOST_CHECK( stc->start() == false );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isActive() == true );
    BOOST_CHECK( stc->isConfigured() == true );
    stc->resetFlags();

    // Retry:
    stc->validstart = true;
    BOOST_CHECK( stc->start() == true );
    BOOST_CHECK( stc->didstart == true );
    BOOST_CHECK( stc->isConfigured() == true );
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->isActive() == true );
    stc->resetFlags();

    // Error state by calling error().
    stc->do_error = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->inRunTimeError() == true );
    BOOST_CHECK( stc->diderror == true );
    BOOST_CHECK( stc->didexcept == false );
    BOOST_CHECK( stc->isActive() == true );  // still active
    stc->resetFlags();
    stc->do_error = false;
    stc->recover();
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->isRunning() == true );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didupdate == true );

    // Error state by throwing in updateHook()
    stc->do_throw = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->inRunTimeError() == false );
    BOOST_CHECK( stc->inException() == true );
    BOOST_CHECK( stc->didexcept == true );
    BOOST_CHECK( stc->isActive() == true );  // still active
    stc->resetFlags();
    stc->do_throw = false;
    stc->recover();
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->isConfigured() == false );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didupdate == false );
    stc->resetFlags();
    stc->configure();
    stc->start();

    // Fatal Error state by throwing in exceptionHook()
    stc->do_error = false;
    stc->do_throw = true;
    stc->do_throw3 = true;
    // Running state / updateHook :
    SimulationThread::Instance()->run(1);
    BOOST_CHECK( stc->inRunTimeError() == false );
    BOOST_CHECK( stc->inFatalError() == true );
    BOOST_CHECK( stc->diderror == false );
    BOOST_CHECK( stc->didexcept == true );
    BOOST_CHECK( stc->didstop == true );
    BOOST_CHECK( stc->didcleanup == true );
    BOOST_CHECK( stc->isActive() == false );
    BOOST_CHECK( stc->isRunning() == false );
    BOOST_CHECK( stc->isConfigured() == false );
    stc->resetFlags();

    // Component stuck in fatal error state.
    BOOST_CHECK( stc->configure() == false );
    BOOST_CHECK( stc->start() == false );
}

class calling_error_does_not_override_a_stop_transition_Task : public RTT::TaskContext
{
public:
    calling_error_does_not_override_a_stop_transition_Task()
        : TaskContext("test") {}
    void updateHook() { error(); }
    void errorHook() {
        while(getTargetState() != Stopped);
        error();
        trigger();
    }
};
BOOST_AUTO_TEST_CASE(calling_error_does_not_override_a_stop_transition)
{
    for (int i = 0; i < 100; ++i)
    {
        calling_error_does_not_override_a_stop_transition_Task task;
        task.start();
        while(!task.inRunTimeError()) {
            usleep(100);
        }
        task.stop();
        BOOST_REQUIRE_EQUAL(RTT::TaskContext::Stopped, task.getTaskState());
        BOOST_REQUIRE_EQUAL(RTT::TaskContext::Stopped, task.getTargetState());
    }
}

class calling_recover_does_not_override_a_stop_transition_Task : public RTT::TaskContext
{
public:
    bool mRecovered;
    TaskState mTargetState;
    calling_recover_does_not_override_a_stop_transition_Task()
        : TaskContext("test"), mRecovered(true) {} // true is an error
    void updateHook() { error(); }
    void errorHook() {
        while(getTargetState() != Stopped);
        mRecovered = recover();
        trigger();
    }
};
BOOST_AUTO_TEST_CASE(calling_recover_does_not_override_a_stop_transition)
{
    for (int i = 0; i < 100; ++i)
    {
        calling_recover_does_not_override_a_stop_transition_Task task;
        task.start();
        while(!task.inRunTimeError()) {
            usleep(100);
        }
        task.stop();
        BOOST_REQUIRE_EQUAL(RTT::TaskContext::Stopped, task.getTaskState());
        BOOST_REQUIRE_EQUAL(RTT::TaskContext::Stopped, task.getTargetState());
        BOOST_REQUIRE(!task.mRecovered);
    }
}

class errorHook_is_not_called_after_an_exit_transition_Task : public RTT::TaskContext
{
public:
    TimeService::ticks lastErrorHook;
    TimeService::ticks lastStopHook;

    errorHook_is_not_called_after_an_exit_transition_Task()
        : TaskContext("test") {}
    void updateHook() { error(); }
    void errorHook() {
        usleep(100);
        lastErrorHook = TimeService::Instance()->getTicks();
        trigger();
    }
    void stopHook() {
        lastStopHook = TimeService::Instance()->getTicks();
        usleep(100);
    }
};
BOOST_AUTO_TEST_CASE(testErrorHook_is_not_called_during_stop)
{
    for (int i = 0; i < 100; ++i)
    {
        errorHook_is_not_called_after_an_exit_transition_Task task;
        task.start();
        while(!task.inRunTimeError()) {
            usleep(100);
        }
        task.stop();
        BOOST_REQUIRE(task.lastErrorHook < task.lastStopHook);
    }
}

BOOST_AUTO_TEST_SUITE_END()

