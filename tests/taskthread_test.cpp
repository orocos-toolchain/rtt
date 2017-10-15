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


#include "unit.hpp"

#include "taskthread_test.hpp"

#include <iostream>

#include <extras/Activities.hpp>
#include <extras/TimerThread.hpp>
#include <extras/SimulationThread.hpp>
#include <os/MainThread.hpp>
#include <Logger.hpp>
#include <rtt-config.h>

using namespace std;
using namespace RTT;

struct A {};

template<class T>
struct TestActivity
    : public T
{
    bool result, _dothrow;
    bool init, stepped, looped, fini;
    bool wasrunning, wasactive;

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
        wasrunning = this->isRunning();
        wasactive = this->isActive();
#ifndef ORO_EMBEDDED
        if ( _dothrow )
            throw A();
#endif
    }

    void loop();

    void finalize() {
        fini   = true;
    }

    void reset(bool fail) {
        result = fail;
        init = false;
        stepped = false;
        looped = false;
        fini = false;
        wasrunning=false;
        wasactive=false;
    }
};

template <class T>
void TestActivity<T>::loop() {
    looped = true;
    T::loop();
}

template <>
void TestActivity<PeriodicActivity>::loop() {
    looped = true;
}

struct TestRunner
    : public RunnableInterface
{
    bool result;
    bool init, stepped, fini;
    bool looped, broke;
    bool wasrunning, wasactive;
    bool worked, wreason;

    TestRunner(bool fail)
    {
        this->reset(fail);
    }

    bool initialize() {
        init    = true;
        BOOST_CHECK(getActivity()->isActive());
        BOOST_CHECK(!getActivity()->isRunning());
        return result;
    }

    void step() {
        stepped = true;
        wasrunning=getActivity()->isRunning();
        wasactive=getActivity()->isActive();
    }

    void work(RunnableInterface::WorkReason reason) {
        worked = true;
        wreason = reason;
    }

    void loop() {
        looped = true;
        wasrunning=getActivity()->isRunning();
        wasactive=getActivity()->isActive();
    }

    bool breakLoop() {
        broke = true;
        return true;
    }

    void finalize() {
        fini   = true;
        BOOST_CHECK(getActivity()->isActive());
        BOOST_CHECK(!getActivity()->isRunning());
    }

    void reset(bool fail) {
        result = fail;
        init = false;
        stepped = false;
        fini = false;
        looped = false;
        broke = false;
        wasrunning = false;
        wasactive = false;
        worked = false;
        wreason = false;
    }
};


void
ActivitiesThreadTest::setUp()
{
    t_task_np = new TestActivity<PeriodicActivity>(3, 0.01, true );           // will be stopped if np_bad throws
    t_task_np_bad = new TestActivity<PeriodicActivity>(3, 0.01, true, true ); // throws
    t_task_p = new TestActivity<PeriodicActivity>(3, 0.032, true );           // does not throw
    t_task_a = new TestActivity<Activity>(3, 0.01, true, true );              // throws
}


void
ActivitiesThreadTest::tearDown()
{
    delete t_task_np;
    delete t_task_np_bad;
    delete t_task_p;
    delete t_task_a;
}

// Registers the fixture into the 'registry'
BOOST_FIXTURE_TEST_SUITE( ActivitiesThreadTestSuite, ActivitiesThreadTest )

BOOST_AUTO_TEST_CASE( testPeriodicActivity )
{
    // Test periodic task sequencing...

    TestActivity<PeriodicActivity> mtask( 15, 0.01, true );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK_EQUAL( 0.01, mtask.thread()->getPeriod() );

    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    os::CheckPriority( rtsched, bprio );

    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    TestActivity<PeriodicActivity> m2task( 15, 0.01, true );
    BOOST_CHECK( mtask.thread() == m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isRunning() == true );
    BOOST_CHECK( mtask.thread()->isRunning() == true );
    BOOST_CHECK( m2task.isRunning() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isRunning() == true );

    usleep(100000);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( m2task.isRunning() == true );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isRunning() == false );

    // check if the activity actually ran
    BOOST_CHECK( mtask.looped == false );
    BOOST_CHECK( mtask.stepped == true );
    BOOST_CHECK( m2task.looped == false );
    BOOST_CHECK( m2task.stepped == true );

    // Different Scheduler (don't test if invalid priorities)
    bprio = 15;
    rtsched = ORO_SCHED_RT;
    if ( os::CheckPriority( rtsched, bprio ) ) {
        PeriodicActivity m3task(ORO_SCHED_OTHER, 15, 0.01);
        bprio = 15;
        rtsched = ORO_SCHED_OTHER;
        if ( os::CheckPriority( rtsched, bprio ) ) {
            BOOST_CHECK( mtask.thread() != m3task.thread() );
            BOOST_CHECK_EQUAL( ORO_SCHED_OTHER, m3task.thread()->getScheduler() );
        }
    }

    // Different CPU affinity
    unsigned cpu_affinity = 1; // first CPU only
    if ( mtask.thread()->getCpuAffinity() != cpu_affinity ) {
        PeriodicActivity m4task(ORO_SCHED_OTHER, 15, 0.01, cpu_affinity);
        BOOST_CHECK( mtask.thread() != m4task.thread() );
        BOOST_CHECK_EQUAL( cpu_affinity, m4task.thread()->getCpuAffinity() );
    }

    // Starting thread if thread not running
    BOOST_CHECK( mtask.thread()->stop() );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK( mtask.start() );
    BOOST_CHECK( mtask.isRunning() == true );
    BOOST_CHECK( mtask.thread()->isRunning() == true);
}

BOOST_AUTO_TEST_CASE( testActivityNonPeriodic )
{
    // Test non-periodic task sequencing...

    TestActivity<Activity> mtask( 15, 0.0, true );
    usleep(100000);
    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    os::CheckPriority( rtsched, bprio );

    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    TestActivity<Activity> m2task( 15, 0.0, true );
    BOOST_CHECK( mtask.thread() != m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isActive() == true );
    BOOST_CHECK( m2task.isActive() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isActive() == true );

    usleep(100000);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( m2task.isActive() == true );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isActive() == false );

    // check if the activity actually ran
    BOOST_CHECK( mtask.looped == true );
    BOOST_CHECK( mtask.stepped == true );
    BOOST_CHECK( m2task.looped == true );
    BOOST_CHECK( m2task.stepped == true );
}

BOOST_AUTO_TEST_CASE( testActivityPeriodic )
{
    // Test periodic task sequencing...

    TestActivity<Activity> mtask( 15, 0.01, true );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.thread()->isRunning() == false );
    BOOST_CHECK_EQUAL( 0.01, mtask.thread()->getPeriod() );

    // Adapt priority levels to OS.
    int bprio = 15, rtsched = ORO_SCHED_RT;
    os::CheckPriority( rtsched, bprio );

    BOOST_CHECK_EQUAL( bprio, mtask.thread()->getPriority() );
    BOOST_CHECK_EQUAL( rtsched, mtask.thread()->getScheduler() );

    TestActivity<Activity> m2task( 15, 0.01, true );
    BOOST_CHECK( mtask.thread() != m2task.thread() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( mtask.isRunning() == true );
    BOOST_CHECK( mtask.thread()->isRunning() == true );
    BOOST_CHECK( m2task.isRunning() == false );
    BOOST_CHECK( m2task.start() == true );
    BOOST_CHECK( m2task.isRunning() == true );

    usleep(100000);

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( m2task.isRunning() == true );
    BOOST_CHECK( m2task.stop() == true );
    BOOST_CHECK( m2task.isRunning() == false );

    // check if the activity actually ran
    //BOOST_CHECK( mtask.looped == false );
    BOOST_CHECK( mtask.stepped == true );
    //BOOST_CHECK( m2task.looped == false );
    BOOST_CHECK( m2task.stepped == true );
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
    BOOST_CHECK( mtask.thread() == os::MainThread::Instance() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( r.init == true );

    BOOST_CHECK( mtask.isActive() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.start() == false );

    // calls loop()
    BOOST_CHECK( mtask.execute() );
    BOOST_CHECK( r.looped == true );
    BOOST_CHECK( r.worked == true );
    BOOST_CHECK( r.wreason == RunnableInterface::TimeOut );
    BOOST_CHECK( mtask.execute() );
    BOOST_CHECK( r.wasrunning );
    BOOST_CHECK( r.wasactive );

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
    BOOST_CHECK( r.wasrunning );
    BOOST_CHECK( r.wasactive );
    BOOST_CHECK( r.worked == true );
    BOOST_CHECK( r.wreason == RunnableInterface::TimeOut );
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
    BOOST_CHECK( mslave_p.thread() == os::MainThread::Instance() );

    BOOST_CHECK( mslave_p.start() );
    BOOST_CHECK( r.init == true );
    BOOST_CHECK( mslave_p.isActive() );
    BOOST_CHECK( mslave_p.isRunning() );
    BOOST_CHECK( mslave_p.execute() );
    BOOST_CHECK( r.stepped == true );
    BOOST_CHECK( r.worked == true );
    BOOST_CHECK( r.wreason == RunnableInterface::TimeOut );
    BOOST_CHECK( r.wasrunning );
    BOOST_CHECK( r.wasactive );
    BOOST_CHECK( !mslave_p.start() );

    // stopping...
    BOOST_CHECK( mslave_p.stop() );
    BOOST_CHECK( r.fini == true );
    BOOST_CHECK( !mslave_p.isActive() );
    BOOST_CHECK( !mslave_p.isRunning() );
    BOOST_CHECK( !mslave_p.execute() );
}

BOOST_AUTO_TEST_CASE( testSequential )
{
    // Test sequential activities
    TestRunner r(true);

    SequentialActivity mtask(&r);
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.isPeriodic() == false );
    BOOST_CHECK( mtask.getPeriod() == 0.0 );
    BOOST_CHECK( mtask.execute() == false );
    BOOST_CHECK( mtask.trigger() == false );
    BOOST_CHECK( mtask.thread() == os::MainThread::Instance() );

    // starting...
    BOOST_CHECK( mtask.start() == true );
    BOOST_CHECK( r.init == true );

    BOOST_CHECK( mtask.isActive() == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.start() == false );

    // calls step()
    BOOST_CHECK( mtask.trigger() );
    BOOST_CHECK( r.stepped == true );
    BOOST_CHECK( r.worked == true );
    BOOST_CHECK( r.wreason == RunnableInterface::TimeOut );
    BOOST_CHECK( r.wasrunning );
    BOOST_CHECK( r.wasactive );
    BOOST_CHECK( mtask.execute() == false );

    // stopping...
    BOOST_CHECK( mtask.stop() == true );
    BOOST_CHECK( r.fini == true );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.stop() == false );

    BOOST_CHECK( mtask.execute() == false );
    BOOST_CHECK( mtask.trigger() == false );

    r.reset(false);
    BOOST_CHECK( mtask.start() == false );
    BOOST_CHECK( r.init == true );

    BOOST_CHECK( mtask.isActive() == false );
    BOOST_CHECK( mtask.isRunning() == false );
    BOOST_CHECK( mtask.start() == false );
}

BOOST_AUTO_TEST_CASE( testScheduler )
{
    int rtsched = ORO_SCHED_OTHER;
    int bprio = 15;

    os::CheckPriority( rtsched, bprio );
    TimerThreadPtr tt = TimerThread::Instance(rtsched, bprio, 0.0123);
    BOOST_CHECK( tt != 0 );

    BOOST_CHECK( tt->isRunning() == false );

    BOOST_CHECK_EQUAL( 0.0123, tt->getPeriod());

    BOOST_CHECK_EQUAL( bprio, tt->getPriority());
    BOOST_CHECK_EQUAL( rtsched, tt->getScheduler());

    // different scheduler, different thread.
    rtsched = ORO_SCHED_RT;
    bprio = 15;
    if ( os::CheckPriority( rtsched, bprio ) ) {
        TimerThreadPtr tt2 = TimerThread::Instance(rtsched, bprio, 0.0123);
        BOOST_CHECK( tt2 != 0 );
        BOOST_CHECK( tt2 != tt );
        usleep(100000);
        BOOST_CHECK_EQUAL( rtsched, tt2->getScheduler());

        tt = TimerThread::Instance(bprio, 0.0123); // ORO_SCHED_RT is the default.
        BOOST_CHECK( tt == tt2 );
    }
}

#if !defined( OROCOS_TARGET_MACOSX )
/**
 * Checks if the rtos_task_get_pid function works properly.
 */
BOOST_AUTO_TEST_CASE( testThreadPID )
{
	unsigned int tid = t_task_a->getPid();
	BOOST_CHECK( tid );
	unsigned int mpid = MainThread::Instance()->getPid();
	BOOST_CHECK( mpid );
	BOOST_CHECK_NE( mpid, tid );
//	cout << "PID:" << mpid <<endl;
//	cout << "TID:" << tid << endl;
}
#endif

#if !defined( OROCOS_TARGET_WIN32 )
BOOST_AUTO_TEST_CASE( testThreadConfig )
{
    int rtsched = ORO_SCHED_RT;
    int bprio = 15;
    TimerThreadPtr tt = TimerThread::Instance(bprio, 0.0123);

    // Test creation of new thread, check functions when not running.
    BOOST_CHECK( tt->isRunning() == false );

    BOOST_CHECK_EQUAL( 0.0123, tt->getPeriod());

    // only do this if valid priority/scheduler range:
    if ( os::CheckPriority( rtsched, bprio ) == true)
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
    if ( tt->setScheduler(ORO_SCHED_OTHER) ) {
        usleep(100000);
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_OTHER );
    }
    if ( tt->setScheduler(ORO_SCHED_RT) ) {
        usleep(100000);
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_RT );
    }
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

    // Check functions when running:
    BOOST_CHECK( tt->isRunning() == true );

    // prints annoying warning messages...
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel(Logger::Critical);
#ifndef OROCOS_TARGET_XENOMAI // disabled until 'persistent' scheduler switching is implemented for Xenomai.
    if ( tt->setScheduler(ORO_SCHED_RT) ) {
        usleep(100000);
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_RT );
    }
    if ( tt->setScheduler(ORO_SCHED_OTHER) ) {
        usleep(100000);
        BOOST_CHECK( tt->getScheduler() == ORO_SCHED_OTHER );
    }
#endif
    Logger::log().setLogLevel( ll );

    // reconfigure periodicity when running.
    BOOST_CHECK( tt->setPeriod(0.5) == true );

    // reconfigure periodicity when stopped.
    BOOST_CHECK( tt->stop() );
    BOOST_CHECK( tt->setPeriod(0.3) );
    BOOST_CHECK_EQUAL( Seconds_to_nsecs(0.3), tt->getPeriodNS() );

    // some quick start/stops.
    BOOST_CHECK( tt->start() );
    BOOST_CHECK( tt->stop() );
    BOOST_CHECK( tt->start() );

}
#endif

#if !defined( ORO_EMBEDDED ) && !defined( OROCOS_TARGET_WIN32 )
BOOST_AUTO_TEST_CASE( testExceptionRecovery )
{
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel( Logger::Never );
    BOOST_CHECK(t_task_np->start());
    BOOST_CHECK(t_task_np_bad->start()); // must stop t_task_np too.
    BOOST_CHECK(t_task_p->start());
    BOOST_CHECK(t_task_a->start());

    usleep(100000);

    // thread should stop :
    Logger::log().setLogLevel( ll );
    BOOST_CHECK( !t_task_np_bad->thread()->isRunning() );

    // This sometimes fails because of a bug in gnulinux Thread/fosi:
    // two TimerThreads are created because the first one is not yet
    // running, hence np_bad runs in a different thread than np...
    BOOST_CHECK( !t_task_np->isRunning() );
    BOOST_CHECK( !t_task_np_bad->isRunning() );
    BOOST_CHECK( t_task_p->isRunning() );
    BOOST_CHECK( !t_task_a->isRunning() );

    BOOST_CHECK( t_task_np->init );
    BOOST_CHECK( t_task_np_bad->init );
    BOOST_CHECK( t_task_p->init );
    BOOST_CHECK( t_task_a->init );

    BOOST_CHECK( t_task_np->stepped );
    BOOST_CHECK( t_task_np->wasrunning );
    BOOST_CHECK( t_task_np->wasactive );

    BOOST_CHECK( t_task_np_bad->stepped );
    BOOST_CHECK( t_task_np_bad->wasrunning );
    BOOST_CHECK( t_task_np_bad->wasactive );

    BOOST_CHECK( t_task_p->stepped );
    BOOST_CHECK( t_task_p->wasrunning );
    BOOST_CHECK( t_task_p->wasactive );

    BOOST_CHECK( t_task_a->stepped );
    BOOST_CHECK( t_task_a->wasrunning );
    BOOST_CHECK( t_task_a->wasactive );

    // must be stopped since bad throwed an exception
    BOOST_CHECK( t_task_np->fini );
    BOOST_CHECK( t_task_np_bad->fini );
    BOOST_CHECK( t_task_a->fini );

    t_task_p->stop();

    // see if we recovered ok :
    Logger::log().setLogLevel( Logger::Never );
    // do not start np_bad
    BOOST_CHECK(t_task_np->start());
    BOOST_CHECK(t_task_a->start());

    usleep(100000);
    Logger::log().setLogLevel( ll );
    t_task_p->reset(true);
    BOOST_CHECK( t_task_np->isRunning() );
    BOOST_CHECK( t_task_np->init );
    BOOST_CHECK( t_task_np->stepped );
    BOOST_CHECK( t_task_np->wasrunning );
    BOOST_CHECK( t_task_np->wasactive );

    BOOST_CHECK(t_task_np->stop());
    BOOST_CHECK( t_task_np->fini );
}
#endif

BOOST_AUTO_TEST_SUITE_END()

