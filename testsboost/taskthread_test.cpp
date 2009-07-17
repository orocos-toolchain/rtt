/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:26 CEST 2005  taskthread_test.cpp

                        taskthread_test.cpp -  description
                           -------------------
    begin                : Tue April 05 2005
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




#include "taskthread_test.hpp"
#include <unistd.h>
#include <iostream>

#include <Activities.hpp>
#include <TimerThread.hpp>
#include <SimulationThread.hpp>
#include <os/MainThread.hpp>
#include <Logger.hpp>
#include <rtt-config.h>

using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


using namespace RTT;

struct A {};

template<class T>
struct TestActivity
    : public T
{
    bool result, _dothrow;
    bool init, stepped, fini;

    ActivityInterface* owner;

    TestActivity(int prio, double per, bool fail, bool dothrow = false)
        :T(prio,per), _dothrow(dothrow)
    {
        this->reset(fail);
    }

    bool initialize() {
        init    = true;
        return result;
    }
    void step() {
        stepped = true;
#ifndef ORO_EMBEDDED
        if ( _dothrow )
            throw A();
#endif
    }
    void finalize() {
        fini   = true;
    }

    void reset(bool fail) {
        result = fail;
        init = false;
        stepped = false;
        fini = false;
    }
};

struct TestRunner
    : public RunnableInterface
{
    bool result;
    bool init, stepped, fini;
    bool looped, broke;

    TestRunner(bool fail)
    {
        this->reset(fail);
    }

    bool initialize() {
        init    = true;
        return result;
    }
    void step() {
        stepped = true;
    }

    void loop() {
        looped = true;
    }

    bool breakLoop() {
        broke = true;
        return true;
    }

    void finalize() {
        fini   = true;
    }

    void reset(bool fail) {
        result = fail;
        init = false;
        stepped = false;
        fini = false;
        looped = false;
        broke = false;
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


void
ActivitiesThreadTest::setUp()
{
    t_task_np = new TestActivity<PeriodicActivity>(3, 0.01, true );
    t_task_np_bad = new TestActivity<PeriodicActivity>(3, 0.01, true, true );
    t_task_p = new TestActivity<PeriodicActivity>(3, 0.032, true );
}


void
ActivitiesThreadTest::tearDown()
{
    delete t_task_np;
    delete t_task_np_bad;
    delete t_task_p;
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( ActivitiesThreadTestSuite, ActivitiesThreadTest )

BOOST_AUTO_TEST_CASE(testPeriodic )
{
    // Test periodic task sequencing...

    PeriodicActivity mtask( 15, 0.01 );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() );
    BOOST_CHECK_EQUAL( 0.01, mtask.thread()->getPeriod() );

    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    OS::CheckPriority( rtsched, bprio );

    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    PeriodicActivity m2task( 15, 0.01 );
    BOOST_CHECK( mtask.thread() == m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isRunning() == true );
    BOOST_CHECK( m2task.isRunning() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isRunning() == true );

    sleep(1);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( m2task.isRunning() == true );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isRunning() == false );

    // Different Scheduler (don't test if invalid priorities)
    bprio = 15;
    rtsched = ORO_SCHED_RT;
    if ( OS::CheckPriority( rtsched, bprio ) ) {
        PeriodicActivity m3task(ORO_SCHED_OTHER, 15, 0.01);
        bprio = 15;
        rtsched = ORO_SCHED_OTHER;
        if ( OS::CheckPriority( rtsched, bprio ) ) {
            BOOST_CHECK( mtask.thread() != m3task.thread() );
            BOOST_CHECK_EQUAL( ORO_SCHED_OTHER, m3task.thread()->getScheduler() );
        }
    }

    // Starting thread if thread not running
    BOOST_CHECK( mtask.thread()->stop() );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK( mtask.start() );
    BOOST_CHECK( mtask.isRunning() == true );
    BOOST_CHECK( mtask.thread()->isRunning() == true);
}

BOOST_AUTO_TEST_CASE( testNonPeriodic )
{
    // Test periodic task sequencing...

    NonPeriodicActivity mtask( 15 );

    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    OS::CheckPriority( rtsched, bprio );

    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    NonPeriodicActivity m2task( 15 );
    BOOST_CHECK( mtask.thread() != m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isActive() == true );
    BOOST_CHECK( m2task.isActive() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isActive() == true );

    sleep(1);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( m2task.isActive() == true );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isActive() == false );

    // Different Scheduler
    bprio = 15;
    rtsched = ORO_SCHED_OTHER;
    if ( OS::CheckPriority( rtsched, bprio ) ) {
        NonPeriodicActivity m3task(ORO_SCHED_OTHER, 15);
        BOOST_CHECK( mtask.thread() != m3task.thread() );
        BOOST_CHECK_EQUAL( ORO_SCHED_OTHER, m3task.thread()->getScheduler() );
    }
}

BOOST_AUTO_TEST_CASE( testSlave )
{
    // Test slave activities
    TestRunner r(true);

    // without master, NP.
    SlaveActivity mtask(&r);
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.isPeriodic() == false );
    BOOST_CHECK( mtask.getPeriod() == 0.0 );
    BOOST_CHECK( mtask.execute() == false );
    BOOST_CHECK( mtask.thread() == OS::MainThread::Instance() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( r.init == true );

    BOOST_CHECK( mtask.isActive() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.start() == false );

    // calls loop()
    BOOST_CHECK( mtask.execute() );
    BOOST_CHECK( r.looped == true );
    BOOST_CHECK( mtask.execute() );

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( r.fini == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.stop() == false );

    BOOST_CHECK( mtask.execute() == false );

    r.reset(true);

    // with periodic master:
    SlaveActivity mslave( t_task_np, &r );
    BOOST_CHECK( mslave.isActive() == false );
    BOOST_CHECK( mslave.isRunning() == false );
    BOOST_CHECK( mslave.isPeriodic() == true );
    BOOST_CHECK_EQUAL( mslave.getPeriod(), t_task_np->getPeriod() );
    BOOST_CHECK( mslave.execute() == false );
    BOOST_CHECK( mslave.thread() == t_task_np->thread() );

    BOOST_CHECK( !mslave.start() );
    BOOST_CHECK( t_task_np->start() );
    BOOST_CHECK( mslave.start() );
    BOOST_CHECK( r.init == true );
    BOOST_CHECK( mslave.isActive() );
    BOOST_CHECK( mslave.isRunning() );

    // calls step()
    BOOST_CHECK( mslave.execute() );
    BOOST_CHECK( r.stepped == true );
    BOOST_CHECK( mslave.execute() );
    BOOST_CHECK( !mslave.start() );

    // stopping...
    BOOST_CHECK( mslave.stop() );
    BOOST_CHECK( r.fini == true );
    BOOST_CHECK( !mslave.isActive() );
    BOOST_CHECK( !mslave.isRunning() );

    r.reset(true);

    // periodic:
    SlaveActivity mslave_p(0.001, &r);
    BOOST_CHECK( mslave_p.isActive() == false );
    BOOST_CHECK( mslave_p.isRunning() == false );
    BOOST_CHECK( mslave_p.isPeriodic() == true );
    BOOST_CHECK( mslave_p.getPeriod() == 0.001 );
    BOOST_CHECK( mslave_p.execute() == false );
    BOOST_CHECK( mslave_p.thread() == OS::MainThread::Instance() );

    BOOST_CHECK( mslave_p.start() );
    BOOST_CHECK( r.init == true );
    BOOST_CHECK( mslave_p.isActive() );
    BOOST_CHECK( mslave_p.isRunning() );
    BOOST_CHECK( mslave_p.execute() );
    BOOST_CHECK( r.stepped == true );
    BOOST_CHECK( !mslave_p.start() );

    // stopping...
    BOOST_CHECK( mslave_p.stop() );
    BOOST_CHECK( r.fini == true );
    BOOST_CHECK( !mslave_p.isActive() );
    BOOST_CHECK( !mslave_p.isRunning() );
    BOOST_CHECK( !mslave_p.execute() );

}

BOOST_AUTO_TEST_CASE( testScheduler )
{
    int rtsched = ORO_SCHED_OTHER;
    int bprio = 15;

    OS::CheckPriority( rtsched, bprio );
    TimerThreadPtr tt = TimerThread::Instance(rtsched, bprio, 0.0123);
    BOOST_CHECK( tt != 0 );

    BOOST_CHECK( tt->isRunning() == false );

    BOOST_CHECK_EQUAL( 0.0123, tt->getPeriod());

    BOOST_CHECK_EQUAL( bprio, tt->getPriority());
    BOOST_CHECK_EQUAL( rtsched, tt->getScheduler());

    // different scheduler, different thread.
    rtsched = ORO_SCHED_RT;
    bprio = 15;
    if ( OS::CheckPriority( rtsched, bprio ) ) {
        TimerThreadPtr tt2 = TimerThread::Instance(rtsched, bprio, 0.0123);
        BOOST_CHECK( tt2 != 0 );
        BOOST_CHECK( tt2 != tt );
        BOOST_CHECK_EQUAL( rtsched, tt2->getScheduler());

        tt = TimerThread::Instance(bprio, 0.0123); // ORO_SCHED_RT is the default.
        BOOST_CHECK( tt == tt2 );
    }
}


BOOST_AUTO_TEST_CASE( testThreadConfig )
{
    int rtsched = ORO_SCHED_RT;
    int bprio = 15;
    TimerThreadPtr tt = TimerThread::Instance(bprio, 0.0123);

    BOOST_CHECK( tt->isRunning() == false );

    BOOST_CHECK_EQUAL( 0.0123, tt->getPeriod());

    // only do this if valid priority/scheduler range:
    if ( OS::CheckPriority( rtsched, bprio ) == true)
    {
        BOOST_CHECK_EQUAL( bprio, tt->getPriority());

        // different priority, different thread.
        TimerThreadPtr tt2 = TimerThread::Instance(bprio - 1, 0.0123);
        BOOST_CHECK( tt2 != 0 );
        BOOST_CHECK( tt2 != tt );

        // different period, different thread.
        TimerThreadPtr tt3 = TimerThread::Instance(bprio, 0.123);
        BOOST_CHECK( tt3 != 0 );
        BOOST_CHECK( tt3 != tt );
        BOOST_CHECK( tt3 != tt2 );
    }

    tt = TimerThread::Instance(bprio, 0.0123);
    BOOST_CHECK( tt != 0 );
    BOOST_CHECK( tt == TimerThread::Instance(bprio,0.0123) );

    // switching hard/soft
    // do not ASSERT since the ret-value may be false...
    if ( tt->setScheduler(ORO_SCHED_OTHER) )
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_OTHER );
    if ( tt->setScheduler(ORO_SCHED_RT) )
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_RT );
    tt->setScheduler(ORO_SCHED_OTHER);
    tt->setScheduler(ORO_SCHED_RT);
    tt->setScheduler(ORO_SCHED_OTHER);
    tt->setScheduler(ORO_SCHED_RT);
    if ( tt->setPriority( 4 ) ) {
        BOOST_CHECK_EQUAL( tt->getPriority(), 4 );

        // even if the priority was changed after construction,
        // the thread can be found:
        BOOST_CHECK( tt == TimerThread::Instance(4,0.0123) );
    }

    BOOST_CHECK( tt->start() );

    sleep(1);

    // prints annoying warning messages...
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel(Logger::Critical);
    BOOST_CHECK( tt->setScheduler(ORO_SCHED_RT) == false );
    BOOST_CHECK( tt->setScheduler(ORO_SCHED_OTHER) == false );
    Logger::log().setLogLevel( ll );
    BOOST_CHECK( tt->setPeriod(0.3) == false );

    // reconfigure periodicity
    BOOST_CHECK( tt->stop() );
    BOOST_CHECK( tt->setPeriod(0.3) );
    BOOST_CHECK_EQUAL( Seconds_to_nsecs(0.3), tt->getPeriodNS() );

    // some quick start/stops.
    BOOST_CHECK( tt->start() );
    BOOST_CHECK( tt->stop() );
    BOOST_CHECK( tt->start() );

}

#ifndef ORO_EMBEDDED
BOOST_AUTO_TEST_CASE( testExceptionRecovery )
{
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel( Logger::Never );
    BOOST_CHECK(t_task_np->start());
    BOOST_CHECK(t_task_np_bad->start());
    BOOST_CHECK(t_task_p->start());

    sleep(10);
    // thread should stop :
    Logger::log().setLogLevel( ll );
    BOOST_CHECK( !t_task_np_bad->thread()->isRunning() );

    BOOST_CHECK( !t_task_np->isRunning() );
    BOOST_CHECK( !t_task_np_bad->isRunning() );
    BOOST_CHECK( t_task_p->isRunning() );

    BOOST_CHECK( t_task_np->init );
    BOOST_CHECK( t_task_np_bad->init );
    BOOST_CHECK( t_task_p->init );

    BOOST_CHECK( t_task_np->stepped );
    BOOST_CHECK( t_task_np_bad->stepped );
    BOOST_CHECK( t_task_p->stepped );

    // must be stopped since bad throwed an exception
    BOOST_CHECK( t_task_np->fini );
    BOOST_CHECK( t_task_np_bad->fini );

    t_task_p->stop();

    // see if we recovered ok :
    BOOST_CHECK( t_task_np_bad->thread()->start() );

    BOOST_CHECK(t_task_np->start());

    sleep(1);
    t_task_p->reset(true);
    BOOST_CHECK( t_task_np->isRunning() );
    BOOST_CHECK( t_task_np->init );
    BOOST_CHECK( t_task_np->stepped );

    BOOST_CHECK(t_task_np->stop());
    BOOST_CHECK( t_task_np->fini );
}
#endif

BOOST_AUTO_TEST_SUITE_END()

void ActivitiesThreadTest::testAddAllocate()
{
    BOOST_CHECK( t_task_np->run( t_run_allocate ) );
}

void ActivitiesThreadTest::testRemoveAllocate()
{
    BOOST_CHECK( t_task_np->run( 0 ) );
}
