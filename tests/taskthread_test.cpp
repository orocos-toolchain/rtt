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

#include <TimerThread.hpp>
#include <SimulationThread.hpp>
#include <os/MainThread.hpp>
#include <Logger.hpp>
#include <rtt-config.h>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ActivitiesThreadTest );

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
    t_task_np = new TestActivity<PeriodicActivity>(3, 0.001, true );
    t_task_np_bad = new TestActivity<PeriodicActivity>(3, 0.001, true, true );
    t_task_p = new TestActivity<PeriodicActivity>(3, 0.032, true );
}


void 
ActivitiesThreadTest::tearDown()
{
    delete t_task_np;
    delete t_task_np_bad;
    delete t_task_p;
}

void ActivitiesThreadTest::testPeriodic()
{
    // Test periodic task sequencing...

    PeriodicActivity mtask( 15, 0.01 );
    CPPUNIT_ASSERT( mtask.isActive() == false );
    CPPUNIT_ASSERT( mtask.isRunning() == false );
    CPPUNIT_ASSERT( mtask.thread()->isRunning() );
    CPPUNIT_ASSERT_EQUAL( 0.01, mtask.thread()->getPeriod() );
    CPPUNIT_ASSERT_EQUAL( 15, mtask.thread()->getPriority() );

    PeriodicActivity m2task( 15, 0.01 );
    CPPUNIT_ASSERT( mtask.thread() == m2task.thread() );

    // starting...
    CPPUNIT_ASSERT( mtask.start() == true );
    CPPUNIT_ASSERT( mtask.isRunning() == true );
    CPPUNIT_ASSERT( m2task.isRunning() == false );
    CPPUNIT_ASSERT( m2task.start() == true );
    CPPUNIT_ASSERT( m2task.isRunning() == true );

    sleep(1);

    // stopping...
    CPPUNIT_ASSERT( mtask.stop() == true );
    CPPUNIT_ASSERT( mtask.isRunning() == false );
    CPPUNIT_ASSERT( m2task.isRunning() == true );
    CPPUNIT_ASSERT( m2task.stop() == true );
    CPPUNIT_ASSERT( m2task.isRunning() == false );
    
}

void ActivitiesThreadTest::testSlave()
{
    // Test slave activities
    TestRunner r(true);

    // without master, NP.
    SlaveActivity mtask(&r);
    CPPUNIT_ASSERT( mtask.isActive() == false );
    CPPUNIT_ASSERT( mtask.isRunning() == false );
    CPPUNIT_ASSERT( mtask.isPeriodic() == false );
    CPPUNIT_ASSERT( mtask.getPeriod() == 0.0 );
    CPPUNIT_ASSERT( mtask.execute() == false );
    CPPUNIT_ASSERT( mtask.thread() == OS::MainThread::Instance() );

    // starting...
    CPPUNIT_ASSERT( mtask.start() == true );
    CPPUNIT_ASSERT( r.init == true );

    CPPUNIT_ASSERT( mtask.isActive() == true );
    CPPUNIT_ASSERT( mtask.isRunning() == false );
    CPPUNIT_ASSERT( mtask.start() == false );

    // calls loop()
    CPPUNIT_ASSERT( mtask.execute() );
    CPPUNIT_ASSERT( r.looped == true );
    CPPUNIT_ASSERT( mtask.execute() );

    // stopping...
    CPPUNIT_ASSERT( mtask.stop() == true );
    CPPUNIT_ASSERT( r.fini == true );
    CPPUNIT_ASSERT( mtask.isRunning() == false );
    CPPUNIT_ASSERT( mtask.isActive() == false );
    CPPUNIT_ASSERT( mtask.stop() == false );

    CPPUNIT_ASSERT( mtask.execute() == false );

    r.reset(true);

    // with periodic master:
    SlaveActivity mslave( t_task_np, &r );
    CPPUNIT_ASSERT( mslave.isActive() == false );
    CPPUNIT_ASSERT( mslave.isRunning() == false );
    CPPUNIT_ASSERT( mslave.isPeriodic() == true );
    CPPUNIT_ASSERT_EQUAL( mslave.getPeriod(), t_task_np->getPeriod() );
    CPPUNIT_ASSERT( mslave.execute() == false );
    CPPUNIT_ASSERT( mslave.thread() == t_task_np->thread() );

    CPPUNIT_ASSERT( !mslave.start() );
    CPPUNIT_ASSERT( t_task_np->start() );
    CPPUNIT_ASSERT( mslave.start() );
    CPPUNIT_ASSERT( r.init == true );
    CPPUNIT_ASSERT( mslave.isActive() );
    CPPUNIT_ASSERT( mslave.isRunning() );

    // calls step()
    CPPUNIT_ASSERT( mslave.execute() );
    CPPUNIT_ASSERT( r.stepped == true );
    CPPUNIT_ASSERT( mslave.execute() );
    CPPUNIT_ASSERT( !mslave.start() );

    // stopping...
    CPPUNIT_ASSERT( mslave.stop() );
    CPPUNIT_ASSERT( r.fini == true );
    CPPUNIT_ASSERT( !mslave.isActive() );
    CPPUNIT_ASSERT( !mslave.isRunning() );

    r.reset(true);
    
    // periodic:
    SlaveActivity mslave_p(0.001, &r);
    CPPUNIT_ASSERT( mslave_p.isActive() == false );
    CPPUNIT_ASSERT( mslave_p.isRunning() == false );
    CPPUNIT_ASSERT( mslave_p.isPeriodic() == true );
    CPPUNIT_ASSERT( mslave_p.getPeriod() == 0.001 );
    CPPUNIT_ASSERT( mslave_p.execute() == false );
    CPPUNIT_ASSERT( mslave_p.thread() == OS::MainThread::Instance() );

    CPPUNIT_ASSERT( mslave_p.start() );
    CPPUNIT_ASSERT( r.init == true );
    CPPUNIT_ASSERT( mslave_p.isActive() );
    CPPUNIT_ASSERT( mslave_p.isRunning() );
    CPPUNIT_ASSERT( mslave_p.execute() );
    CPPUNIT_ASSERT( r.stepped == true );
    CPPUNIT_ASSERT( !mslave_p.start() );

    // stopping...
    CPPUNIT_ASSERT( mslave_p.stop() );
    CPPUNIT_ASSERT( r.fini == true );
    CPPUNIT_ASSERT( !mslave_p.isActive() );
    CPPUNIT_ASSERT( !mslave_p.isRunning() );
    CPPUNIT_ASSERT( !mslave_p.execute() );
    
}


void ActivitiesThreadTest::testThreadConfig()
{
    TimerThreadPtr tt = TimerThread::Instance(15, 0.01);

    CPPUNIT_ASSERT( tt->isRunning() == false );

    CPPUNIT_ASSERT_EQUAL( 0.01, tt->getPeriod());
    CPPUNIT_ASSERT_EQUAL( 15, tt->getPriority());

    {
        // different priority, different thread.
        TimerThreadPtr tt2 = TimerThread::Instance(16, 0.01);
        CPPUNIT_ASSERT( tt2 != 0 );
        CPPUNIT_ASSERT( tt2 != tt );
        
        // different period, different thread.
        TimerThreadPtr tt3 = TimerThread::Instance(15, 0.1);
        CPPUNIT_ASSERT( tt3 != 0 );
        CPPUNIT_ASSERT( tt3 != tt );
        CPPUNIT_ASSERT( tt3 != tt2 );
    }

    tt = TimerThread::Instance(15, 0.01);
    CPPUNIT_ASSERT( tt != 0 );
    CPPUNIT_ASSERT( tt == TimerThread::Instance(15,0.01) );

    // switching hard/soft
    // do not ASSERT since the ret-value may be false...
    if ( tt->setScheduler(ORO_SCHED_OTHER) )
        CPPUNIT_ASSERT( tt->getScheduler() == ORO_SCHED_OTHER );
    if ( tt->setScheduler(ORO_SCHED_RT) )
        CPPUNIT_ASSERT( tt->getScheduler() == ORO_SCHED_RT );
    tt->setScheduler(ORO_SCHED_OTHER);
    tt->setScheduler(ORO_SCHED_RT);
    tt->setScheduler(ORO_SCHED_OTHER);
    tt->setScheduler(ORO_SCHED_RT);
    if ( tt->setPriority( 4 ) ) {
        CPPUNIT_ASSERT( tt->getPriority() == 4 );

        // even if the priority was changed after construction, 
        // the thread can be found:
        CPPUNIT_ASSERT( tt == TimerThread::Instance(4,0.01) );
    }

    CPPUNIT_ASSERT( tt->start() );

    sleep(1);

    // prints annoying warning messages...
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel(Logger::Critical);
    CPPUNIT_ASSERT( tt->setScheduler(0) == false );
    CPPUNIT_ASSERT( tt->setScheduler(1) == false );
    Logger::log().setLogLevel( ll );
    CPPUNIT_ASSERT( tt->setPeriod(0.3) == false );
    
    // reconfigure periodicity
    CPPUNIT_ASSERT( tt->stop() );
    CPPUNIT_ASSERT( tt->setPeriod(0.3) );
    CPPUNIT_ASSERT_EQUAL( Seconds_to_nsecs(0.3), tt->getPeriodNS() );

    // some quick start/stops.
    CPPUNIT_ASSERT( tt->start() );
    CPPUNIT_ASSERT( tt->stop() );
    CPPUNIT_ASSERT( tt->start() );

}

#ifndef ORO_EMBEDDED
void ActivitiesThreadTest::testExceptionRecovery()
{
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel( Logger::Never );
    CPPUNIT_ASSERT(t_task_np->start());
    CPPUNIT_ASSERT(t_task_np_bad->start());
    CPPUNIT_ASSERT(t_task_p->start());

    sleep(1);
    // thread should stop :
    Logger::log().setLogLevel( ll );
    CPPUNIT_ASSERT( !t_task_np_bad->thread()->isRunning() );

    CPPUNIT_ASSERT( !t_task_np->isRunning() );
    CPPUNIT_ASSERT( !t_task_np_bad->isRunning() );
    CPPUNIT_ASSERT( t_task_p->isRunning() );

    CPPUNIT_ASSERT( t_task_np->init );
    CPPUNIT_ASSERT( t_task_np_bad->init );
    CPPUNIT_ASSERT( t_task_p->init );

    CPPUNIT_ASSERT( t_task_np->stepped );
    CPPUNIT_ASSERT( t_task_np_bad->stepped );
    CPPUNIT_ASSERT( t_task_p->stepped );

    // must be stopped since bad throwed an exception
    CPPUNIT_ASSERT( t_task_np->fini );
    CPPUNIT_ASSERT( t_task_np_bad->fini );

    t_task_p->stop();

    // see if we recovered ok :
    CPPUNIT_ASSERT( t_task_np_bad->thread()->start() );

    CPPUNIT_ASSERT(t_task_np->start());

    sleep(1);
    t_task_p->reset(true);
    CPPUNIT_ASSERT( t_task_np->isRunning() );
    CPPUNIT_ASSERT( t_task_np->init );
    CPPUNIT_ASSERT( t_task_np->stepped );

    CPPUNIT_ASSERT(t_task_np->stop());
    CPPUNIT_ASSERT( t_task_np->fini );
}
#endif

void ActivitiesThreadTest::testAddAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( t_run_allocate ) );
}

void ActivitiesThreadTest::testRemoveAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
}
    

