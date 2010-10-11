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


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

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
        updatecount=0;
    }

    bool configureHook() {
        BOOST_CHECK( mTaskState <= Stopped );
        BOOST_CHECK( getTargetState() == Stopped );
        didconfig = true;
        return validconfig;
    }

    bool startHook() {
        BOOST_CHECK( mTaskState == Stopped);
        BOOST_CHECK( getTargetState() == Running );
        didstart = true;
        return validstart;
    }

    void stopHook() {
        BOOST_CHECK( mTaskState >= Running || mTaskState == Exception);
        BOOST_CHECK( getTargetState() == Stopped || getTargetState() == Exception );
        didstop = true;
    }

    void cleanupHook() {
        BOOST_CHECK( mTaskState == Stopped || mTaskState == Exception);
        BOOST_CHECK( getTargetState() == PreOperational || getTargetState() == Exception );
        didcleanup = true;
    }

    void exceptionHook() {
        BOOST_CHECK( mTaskState == Exception);
        BOOST_CHECK( getTargetState() == Exception );
        didexcept = true;
        if (do_throw3)
            throw A();
    }

    void updateHook() {
        BOOST_CHECK( mTaskState == Running );
        BOOST_CHECK( getTargetState() == Running );
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
    }

    void errorHook() {
        BOOST_CHECK( mTaskState == RunTimeError );
        BOOST_CHECK( getTargetState() == RunTimeError );
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
    bool didupdate,diderror,didexcept;
    bool do_fatal, do_error, do_throw,do_throw2,do_throw3, do_trigger;
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

    // this is still zero because we used the
    // C++ function start() instead of the operation start().
    // The Operation would trigger an EE run and hence, updateHook().
    BOOST_CHECK_EQUAL( trigtc.updatecount, 0 );

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
    StatesTC pertc;
    pertc.setPeriod( 0.1 );
    BOOST_CHECK_EQUAL( pertc.getPeriod(), 0.1 );
    BOOST_CHECK( pertc.trigger() == false );
    BOOST_CHECK( pertc.configure() == true );
    BOOST_CHECK( pertc.trigger() == false );
    BOOST_CHECK( pertc.start() == true );
    BOOST_CHECK( pertc.trigger() == false );
}

/**
 * Tests the setPeriod function
 */
BOOST_AUTO_TEST_CASE( testSetPeriod )
{
    // Check periodic TC ( rejects trigger() ):
    StatesTC pertc;
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

BOOST_AUTO_TEST_CASE( testExecutionEngine)
{
    // no owner:
    ExecutionEngine ee1(0);
    ExecutionEngine ee2(0);

    // test setActivity:
    BOOST_CHECK( tsim->stop() );
    BOOST_CHECK( tsim->run(&ee1) );
    BOOST_CHECK(tsim->start() );
    BOOST_CHECK( SimulationThread::Instance()->run(5) );

    // this also tests setActivity:
    BOOST_CHECK( tsim->stop() );
    BOOST_CHECK( tsim->run(&ee2) );
    BOOST_CHECK(tsim->start() );
    BOOST_CHECK( SimulationThread::Instance()->run(5) );

    {
        TaskCore tc1(&ee2);
        TaskCore tc2(&ee2);

        // run with two children.
        BOOST_CHECK( SimulationThread::Instance()->run(5) );
    }
    // children removed again:
    BOOST_CHECK( SimulationThread::Instance()->run(5) );
    BOOST_CHECK(tsim->stop() );
    tsim->run(0);
}

BOOST_AUTO_TEST_SUITE_END()

