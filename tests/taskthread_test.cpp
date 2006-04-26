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

#include <corelib/ZeroTimeThread.hpp>
#include <corelib/TimerThread.hpp>
#include <corelib/ZeroLatencyThread.hpp>
#include <corelib/SimulationThread.hpp>
#include <corelib/Logger.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ActivitiesThreadTest );

using namespace ORO_CoreLib;

struct A {};

template<class T>
struct TestTask
    : public T
{
    bool result, _dothrow;
    bool init, stepped, fini;

    ActivityInterface* owner;

    TestTask(double per, bool fail, bool dothrow = false)
        :T(per), _dothrow(dothrow)
    {
        this->reset(fail);
    }

    bool initialize() {
        init    = true;
        return result;
    }
    void step() {
        stepped = true;
        if ( _dothrow )
            throw A();
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
        if ( _dothrow )
            throw A();
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
    t_task_np = new TestTask<NonPreemptibleActivity>( ZeroTimeThread::Instance()->getPeriod(), true );
    t_task_np_bad = new TestTask<NonPreemptibleActivity>( ZeroTimeThread::Instance()->getPeriod(), true, true );
    t_task_p = new TestTask<PreemptibleActivity>( ZeroLatencyThread::Instance()->getPeriod(), true );
    t_task_sim = new TestTask<SimulationActivity>( SimulationThread::Instance()->getPeriod(), true );
}


void 
ActivitiesThreadTest::tearDown()
{
    ZeroTimeThread::Instance()->start();
    delete t_task_np;
    delete t_task_np_bad;
    delete t_task_p;
    delete t_task_sim;
}

void ActivitiesThreadTest::testPeriodic()
{
    // Test periodic task sequencing...

    PeriodicActivity mtask( 15, 0.01 );
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

void ActivitiesThreadTest::testThreadConfig()
{
    TimerThreadPtr tt = TimerThread::Instance(15, 0.01);

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

    // get it again.
    tt = TimerThread::Instance(15, 0.01);
    CPPUNIT_ASSERT( tt != 0 );
    CPPUNIT_ASSERT( tt == TimerThread::Instance(15,0.01) );

    // switching hard/soft
    CPPUNIT_ASSERT( tt->makeHardRealtime() );
    CPPUNIT_ASSERT( tt->makeHardRealtime() );
    CPPUNIT_ASSERT( tt->makeSoftRealtime() );
    CPPUNIT_ASSERT( tt->makeHardRealtime() );
    CPPUNIT_ASSERT( tt->makeSoftRealtime() );
    CPPUNIT_ASSERT( tt->makeSoftRealtime() );
    CPPUNIT_ASSERT( tt->makeHardRealtime() );
    CPPUNIT_ASSERT( tt->start() );

    sleep(1);

    // prints annoying warning messages...
    Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel(Logger::Critical);
    CPPUNIT_ASSERT( tt->makeHardRealtime() );
    CPPUNIT_ASSERT( tt->makeSoftRealtime() == false );
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

void ActivitiesThreadTest::testExceptionRecovery()
{
    //Logger::LogLevel ll = Logger::log().getLogLevel();
    Logger::log().setLogLevel( Logger::Never );
    CPPUNIT_ASSERT(t_task_np->start());
    CPPUNIT_ASSERT(t_task_np_bad->start());
    CPPUNIT_ASSERT(t_task_p->start());

    sleep(1);
    // thread should stop :
    Logger::log().setLogLevel( Logger::Warning );
    CPPUNIT_ASSERT( !ZeroTimeThread::Instance()->isRunning() );

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
    CPPUNIT_ASSERT( ZeroTimeThread::Instance()->start() );

    CPPUNIT_ASSERT(t_task_np->start());

    sleep(1);
    t_task_p->reset(true);
    CPPUNIT_ASSERT( t_task_np->isRunning() );
    CPPUNIT_ASSERT( t_task_np->init );
    CPPUNIT_ASSERT( t_task_np->stepped );

    CPPUNIT_ASSERT(t_task_np->stop());
    CPPUNIT_ASSERT( t_task_np->fini );
}

void ActivitiesThreadTest::testAddAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( t_run_allocate ) );
}

void ActivitiesThreadTest::testRemoveAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
}
    

