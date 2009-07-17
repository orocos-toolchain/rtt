/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:49 CET 2005  tasks_test.cpp

                        tasks_test.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "tasks_test.hpp"

#include <iostream>

#include <PeriodicActivity.hpp>
#include <TimeService.hpp>

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace boost;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

// Registers the fixture into the 'registry'

using namespace RTT;
using namespace RTT::detail;

#define BOOST_CHECK_EQUAL_MESSAGE(M, v1, v2) BOOST_CHECK_MESSAGE( v1==v2, M)

struct TestOverrun
  : public RTT::OS::RunnableInterface
{
  bool fini;
  bool initialize() { fini = false; return true; }

  void step() {
    sleep(1);
  }

  void finalize() {
    fini = true;
  }
};

struct TestPeriodic
    : public RTT::OS::RunnableInterface
{
    int overfail, underfail, succ;
    bool stepped;

    TimeService::ticks ts;

    TestPeriodic()
        : overfail(0), underfail(0), succ(0), stepped(false)
    {
    }

    bool initialize() {
        this->reset();
        return true;
    }
    void step() {
        if (stepped == false ) {
            ts = TimeService::Instance()->getTicks();
            stepped = true;
        } else {
            TimeService::Seconds s = TimeService::Instance()->secondsSince( ts );
            if ( s < this->getThread()->getPeriod() *0.9 ) { // if elapsed time is smaller than 10% of period, something went wrong
                ++underfail;
                //rtos_printf("UnderFailPeriod: %f \n", s);
            }
            else if ( s > this->getThread()->getPeriod() *1.1 ) { // if elapsed time is smaller than 10% of period, something went wrong
                ++overfail;
                //rtos_printf("OverFailPeriod: %f \n", s);
            }
            else {
                ++succ;
                //rtos_printf("SuccPeriod: %f \n", s);
            }
            ts = TimeService::Instance()->getTicks();
        }
    }
    void finalize() {
        if (overfail || underfail)
            cerr <<"overfail is:"<<overfail<<", underfail is:"<<underfail<< " success is: "<<succ<<endl;
    }

    void reset() {
        overfail = 0;
        underfail = 0;
        succ = 0;
        stepped = false;
    }
};

struct TestRunnableInterface
    : public RunnableInterface
{
    bool result, breakl;
    bool init, stepped, looped, fini;

    TestRunnableInterface(bool res)
    {
        this->reset(res);
    }

    bool initialize() {
        init    = true;
        return result;
    }
    void step() {
        stepped = true;
    }
    void finalize() {
        fini   = true;
    }

    void loop() {
        looped = true;
        while (breakl == false) {
            usleep(500000);
        }
    }

    bool breakLoop() {
        return breakl;
    }

    void reset(bool res) {
        result = res;
        init = false;
        stepped = false;
        fini = false;
        looped = false;
        breakl = true;
    }
};

struct TestAllocate
    : public RunnableInterface
{
    std::vector<std::string> v;
    char*       c;
    std::string s;

    bool initialize() {
        c = 0;
        return true;
    }
    void step() {
        v.resize( 0 );
        v.resize( 1025, std::string("Goodbye Memory") );
        delete[] c;
        c = new char[1025];
        s = "Hello World ";
        s += s;
        s += s;
    }
    void finalize() {
        delete[] c;
        v.resize(0);
    }
};

struct TestSelfRemove
    : public RunnableInterface
{
    int c;
    bool fini;
    bool breakl;
    bool initialize() {
        c = 0;
        fini = false;
        breakl = true;
        return true;
    }
    bool breakLoop() {
        return breakl;
    }
    void loop() {
        this->getActivity()->stop();
    }
    void step() {
        ++c;
        if (c == 5)
            this->getActivity()->stop();
    }
    void finalize() {
        fini = true;
    }
};

void
ActivitiesTest::setUp()
{
    t_task_prio = new PeriodicActivity( 15, 0.01 );
    t_act = new Activity(15, 0.01);

    t_run_int_prio = new TestRunnableInterface(true);
    t_run_int_act  = new TestRunnableInterface(true);
    t_run_int_fail = new TestRunnableInterface(false);

    t_run_allocate = new TestAllocate();
    t_self_remove  = new TestSelfRemove();

    t_run_allocate_act = new TestAllocate();
}


void
ActivitiesTest::tearDown()
{
    delete t_task_prio;
    delete t_act;

    delete t_run_int_prio;
    delete t_run_int_act;
    delete t_run_int_fail;

    delete t_run_allocate;
}

BOOST_FIXTURE_TEST_SUITE( ActivitiesTestSuite, ActivitiesTest )

BOOST_AUTO_TEST_CASE( testFailInit )
{
    t_task_prio->run( t_run_int_fail );

    BOOST_CHECK( !t_task_prio->start() );
    BOOST_CHECK( !t_task_prio->stop() );

    BOOST_CHECK( t_act->run( t_run_int_fail ) );
    t_run_int_fail->reset(false);

    BOOST_CHECK( !t_act->start() );
    BOOST_CHECK( !t_act->stop() );

}

#if !defined( OROCOS_TARGET_WIN32 )
BOOST_AUTO_TEST_CASE( testOverrun )
{
  bool r = false;
  // create
  boost::scoped_ptr<TestOverrun> run( new TestOverrun() );
  boost::scoped_ptr<RTT::OS::PeriodicThread> t( new RTT::OS::PeriodicThread(25,"ORThread", 0.1) );
  //BOOST_CHECK_EQUAL(25,t->getPriority() );
  BOOST_CHECK_EQUAL(0.1,t->getPeriod() );
  t->setMaxOverrun(1);

  t->run( run.get() );

  t->start();
  sleep(2);

  r = !t->isRunning();

  t->run(0);

  BOOST_CHECK_MESSAGE( r, "Failed to detect step overrun in Thread");

  BOOST_CHECK_MESSAGE( run->fini, "Failed to execute finalize in emergencyStop" );

}
#endif

BOOST_AUTO_TEST_CASE( testThread )
{
  bool r = false;
  // create
  boost::scoped_ptr<TestPeriodic> run( new TestPeriodic() );

  boost::scoped_ptr<RTT::OS::ThreadInterface> t( new RTT::OS::Thread(ORO_SCHED_RT, RTT::OS::HighestPriority, 0.1, "PThread") );
  t->run( run.get() );

  r = t->start();
  BOOST_CHECK_MESSAGE( r, "Failed to start Thread");
  r = t->stop();
  BOOST_CHECK_MESSAGE( r, "Failed to stop Thread");
  BOOST_CHECK_MESSAGE( run->stepped == true, "Step not executed" );
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too long !", run->overfail, 0);
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too short!", run->underfail, 0);
  run->reset();
  r = t->start();
  BOOST_CHECK_MESSAGE( r, "Failed to start Thread");
  sleep(1);
  r = t->stop();
  BOOST_CHECK_MESSAGE( r, "Failed to stop Thread" );
  BOOST_CHECK_MESSAGE( run->stepped == true, "Step not executed" );
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too long !", run->overfail, 0);
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too short!", run->underfail, 0);
  t->run(0);
}

BOOST_AUTO_TEST_CASE( testThreads )
{
  bool r = false;
  // create
  boost::scoped_ptr<TestPeriodic> run( new TestPeriodic() );

  boost::scoped_ptr<RTT::OS::ThreadInterface> t( new RTT::OS::PeriodicThread(RTT::OS::HighestPriority,"PThread", 0.1) );
  t->run( run.get() );

  r = t->start();
  BOOST_CHECK_MESSAGE( r, "Failed to start Thread");
  r = t->stop();
  BOOST_CHECK_MESSAGE( r, "Failed to stop Thread");
  BOOST_CHECK_MESSAGE( run->stepped == true, "Step not executed" );
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too long !", run->overfail, 0);
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too short!", run->underfail, 0);
  run->reset();
  r = t->start();
  BOOST_CHECK_MESSAGE( r, "Failed to start Thread");
  sleep(1);
  r = t->stop();
  BOOST_CHECK_MESSAGE( r, "Failed to stop Thread" );
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too long !", run->overfail, 0);
  BOOST_CHECK_EQUAL_MESSAGE("Periodic Failure: period of step() too short!", run->underfail, 0);
  BOOST_CHECK_MESSAGE( run->stepped == true, "Step not executed" );
  t->run(0);
}


BOOST_AUTO_TEST_CASE( testNonPeriodic )
{
    scoped_ptr<TestRunnableInterface> t_run_int_nonper
        ( new TestRunnableInterface(true) );
    // force ordering of scoped_ptr destruction.
    {
        scoped_ptr<NonPeriodicActivity> t_task_nonper
            ( new NonPeriodicActivity( 14 ) );

        BOOST_CHECK( t_task_nonper->run( t_run_int_nonper.get() ) );
        BOOST_CHECK( t_task_nonper->start() );
        testPause();
        BOOST_CHECK( t_run_int_nonper->looped );
        BOOST_CHECK( !t_run_int_nonper->stepped );
        BOOST_CHECK( t_run_int_nonper->init );
        BOOST_CHECK( t_task_nonper->stop() );
        BOOST_CHECK( t_run_int_nonper->fini );
        BOOST_CHECK( !t_task_nonper->isRunning() );
        BOOST_CHECK( t_task_nonper->run( 0 ) );
        BOOST_CHECK( t_task_nonper->start() );
        BOOST_CHECK( t_task_nonper->stop() );
        // stop() should be fully synchronising...
        BOOST_CHECK( !t_task_nonper->isRunning() );
    }
}

BOOST_AUTO_TEST_CASE( testActivityNP )
{
    scoped_ptr<TestRunnableInterface> t_run_int_nonper
        ( new TestRunnableInterface(true) );
    // force ordering of scoped_ptr destruction.
    {
        scoped_ptr<Activity> t_task_nonper
            ( new Activity(15) );

        BOOST_CHECK( t_task_nonper->run( t_run_int_nonper.get() ) );
        BOOST_CHECK( t_task_nonper->start() );
        testPause();
        sleep(1);
        BOOST_CHECK( t_run_int_nonper->looped );
        BOOST_CHECK( t_run_int_nonper->init );
        BOOST_CHECK( t_task_nonper->stop() );
        BOOST_CHECK( t_run_int_nonper->fini );
        BOOST_CHECK( !t_task_nonper->isRunning() );
        BOOST_CHECK( t_task_nonper->run( 0 ) );
        BOOST_CHECK( t_task_nonper->start() );
        BOOST_CHECK( t_task_nonper->stop() );
        // stop() should be fully synchronising...
        BOOST_CHECK( !t_task_nonper->isRunning() );
    }
}

BOOST_AUTO_TEST_CASE( testActivityBreakLoop )
{
    scoped_ptr<TestRunnableInterface> t_run_int_nonper
        ( new TestRunnableInterface(true) );
    // force ordering of scoped_ptr destruction.
    {
        scoped_ptr<Activity> t_task_nonper
            ( new Activity(15) );

        BOOST_CHECK( t_task_nonper->run( t_run_int_nonper.get() ) );
        BOOST_CHECK( t_task_nonper->start() );
        testPause();

        t_run_int_nonper->breakl = false;
        BOOST_CHECK( t_task_nonper->start() );
        testPause();
        BOOST_CHECK( t_task_nonper->stop() == false );

        t_run_int_nonper->breakl = true;
        BOOST_CHECK( t_task_nonper->stop() );
        BOOST_CHECK( t_run_int_nonper->fini );
        BOOST_CHECK( !t_task_nonper->isRunning() );
    }
}

BOOST_AUTO_TEST_CASE( testSelfRemove )
{
    scoped_ptr<TestSelfRemove> t_run_int_nonper
        ( new TestSelfRemove() );
    scoped_ptr<NonPeriodicActivity> t_task_nonper
        ( new NonPeriodicActivity( 14 ) );
    BOOST_CHECK( t_task_nonper->run( t_run_int_nonper.get() ) );
    BOOST_CHECK( t_task_nonper->start() );
    BOOST_CHECK( t_task_prio->run(t_self_remove) );
    BOOST_CHECK( t_task_prio->start() );
    testPause();
    BOOST_CHECK( !t_task_prio->isRunning() );
    BOOST_CHECK( t_self_remove->fini );
    BOOST_CHECK( !t_task_nonper->isRunning() );
    BOOST_CHECK( t_run_int_nonper->fini );
}

BOOST_AUTO_TEST_CASE( testActivityNPSelfRemove )
{
    scoped_ptr<TestSelfRemove> t_run_int_nonper
        ( new TestSelfRemove() );
    scoped_ptr<Activity> t_task_nonper
        ( new Activity( 14 ) );
    BOOST_CHECK( t_task_nonper->run( t_run_int_nonper.get() ) );
    BOOST_CHECK( t_task_nonper->start() );
    testPause();
    BOOST_CHECK( !t_task_nonper->isRunning() );
    BOOST_CHECK( t_run_int_nonper->fini );
}

BOOST_AUTO_TEST_CASE( testActivityPSelfRemove )
{
    scoped_ptr<TestSelfRemove> t_run_int_per
        ( new TestSelfRemove() );
    scoped_ptr<Activity> t_task_per
        ( new Activity( 14, 0.01 ) );
    BOOST_CHECK( t_task_per->run( t_run_int_per.get() ) );
    BOOST_CHECK( t_task_per->start() );
    testPause();
    BOOST_CHECK( !t_task_per->isRunning() );
    BOOST_CHECK( t_run_int_per->fini );
}


BOOST_AUTO_TEST_CASE( testStartStop )
{
    testStart();
    testPause();
    testStop();
}

BOOST_AUTO_TEST_CASE( testRunnableInterface )
{
    testAddRunnableInterface();
    testStart();
    testRunnableInterfaceInit();
    testPause();
    testRunnableInterfaceExecution();
    testStop();
    testRemoveRunnableInterface();
}

BOOST_AUTO_TEST_CASE( testAllocation )
{
    testAddAllocate();
    testStart();
    testPause();
    testStop();
    testRemoveAllocate();
}

BOOST_AUTO_TEST_SUITE_END()

void ActivitiesTest::testAddRunnableInterface()
{
    bool adding_prio = t_task_prio->run( t_run_int_prio );
    BOOST_CHECK( adding_prio );

    bool adding_act = t_act->run( t_run_int_act );
    BOOST_CHECK( adding_act );
}

void ActivitiesTest::testRemoveRunnableInterface()
{
    BOOST_CHECK( t_run_int_prio->fini );
    BOOST_CHECK( t_task_prio->run( 0 ) );

    BOOST_CHECK( t_run_int_act->fini );
    BOOST_CHECK( t_act->run( 0 ) );
}

void ActivitiesTest::testStart()
{
    BOOST_CHECK( t_task_prio->start());
    BOOST_CHECK( t_task_prio->isRunning() );

    BOOST_CHECK( t_act->start());
    BOOST_CHECK( t_act->isRunning() );
}

void ActivitiesTest::testPause()
{
    usleep(100000);
}

void ActivitiesTest::testRunnableInterfaceInit() {
    BOOST_CHECK( t_run_int_prio->init );
    BOOST_CHECK( t_run_int_act->init );
}

void ActivitiesTest::testRunnableInterfaceExecution() {

    BOOST_CHECK( t_run_int_prio->stepped );
    BOOST_CHECK( t_run_int_act->stepped );
}

void ActivitiesTest::testStop()
{
    BOOST_CHECK( t_task_prio->stop());
    BOOST_CHECK( !t_task_prio->isRunning() );

    BOOST_CHECK( t_act->stop());
    BOOST_CHECK( !t_act->isRunning() );
}

void ActivitiesTest::testAddAllocate()
{
    BOOST_CHECK( t_task_prio->run( t_run_allocate ) );

    BOOST_CHECK( t_act->run( t_run_allocate_act ) );
}

void ActivitiesTest::testRemoveAllocate()
{
    BOOST_CHECK( t_task_prio->run( 0 ) );

    BOOST_CHECK( t_act->run( 0 ) );
}

