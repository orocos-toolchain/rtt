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
#include <unistd.h>
#include <iostream>

#include <corelib/ZeroTimeThread.hpp>
#include <corelib/ZeroLatencyThread.hpp>
#include <corelib/NonRealTimeThread.hpp>
#include <corelib/SimulationThread.hpp>
#include <corelib/PriorityThread.hpp>
#include <corelib/TaskTimerOneShot.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TasksTest );

using namespace ORO_CoreLib;

struct TestRunnableInterface
    : public RunnableInterface
{
    bool result;
    bool init, stepped, fini;

    TaskInterface* owner;

    TestRunnableInterface(bool fail)
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

struct TestSelfRemove
    : public RunnableInterface
{
    int c;
    bool fini;
    bool initialize() {
        c = 0;
        fini = false;
        return true;
    }
    void step() {
        ++c;
        if (c == 5)
            this->getTask()->stop();
    }
    void finalize() {
        fini = true;
    }
};

void 
TasksTest::setUp()
{
    PriorityThread<15>::Instance()->start();
    t_task_np = new TaskNonPreemptible( ZeroTimeThread::Instance()->getPeriod() );
    t_task_p = new TaskPreemptible( ZeroLatencyThread::Instance()->getPeriod() );
    t_task_nrt = new TaskNonRealTime( NonRealTimeThread::Instance()->getPeriod() );
    t_task_prio = new PriorityTask<15>( PriorityThread<15>::Instance()->getPeriod() );
    t_task_sim = new TaskSimulation( SimulationThread::Instance()->getPeriod() );
    t_task_nonper = new TaskNonPeriodic( 15 );

    t_run_int_np =   new TestRunnableInterface(true);
    t_run_int_p =    new TestRunnableInterface(true);
    t_run_int_nrt =  new TestRunnableInterface(true);
    t_run_int_prio = new TestRunnableInterface(true);
    t_run_int_sim =  new TestRunnableInterface(true);
    t_run_int_nonper =  new TestRunnableInterface(true);

    t_run_allocate = new TestAllocate();
    t_self_remove  = new TestSelfRemove();

    tti = new TaskTimerOneShot(1);
}


void 
TasksTest::tearDown()
{
    PriorityThread<15>::Instance()->stop();
    PriorityThread<15>::Release();
    delete t_task_np;
    delete t_task_p;
    delete t_task_nrt;
    delete t_task_prio;
    delete t_task_sim;
    delete t_task_nonper;
    delete t_run_int_np;
    delete t_run_int_p;
    delete t_run_int_nrt;
    delete t_run_int_prio;
    delete t_run_int_sim;
    delete t_run_int_nonper;

    delete t_run_allocate;
    delete t_self_remove;
    delete tti;
}

void TasksTest::testTimer()
{
    // Add 5 tasks.
    CPPUNIT_ASSERT( tti->addTask( t_task_np ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_p ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_nrt ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_prio ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_sim ) );

    // Remove last :
    CPPUNIT_ASSERT( tti->removeTask( t_task_sim ) );
    tti->tick();

    // Remove First:
    CPPUNIT_ASSERT( tti->removeTask( t_task_np ) );
    tti->tick();

    // Remove middle :
    CPPUNIT_ASSERT( tti->removeTask( t_task_nrt ) );
    tti->tick();

    CPPUNIT_ASSERT( tti->addTask( t_task_np ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_nrt ) );
    CPPUNIT_ASSERT( tti->addTask( t_task_sim ) );

    //Remove 2 in middle :
    CPPUNIT_ASSERT( tti->removeTask( t_task_prio ) );
    CPPUNIT_ASSERT( tti->removeTask( t_task_nrt ) );
    tti->tick();

    //Remove all :
    CPPUNIT_ASSERT( tti->removeTask( t_task_np ) );
    CPPUNIT_ASSERT( tti->removeTask( t_task_p ) );
    CPPUNIT_ASSERT( tti->removeTask( t_task_sim ) );
    tti->tick();

}

void TasksTest::testNonPeriodic()
{
    t_task_nonper->run( t_run_int_nonper );
    CPPUNIT_ASSERT( t_task_nonper->start() );
    sleep(1);
    CPPUNIT_ASSERT( t_run_int_nonper->stepped );
    CPPUNIT_ASSERT( t_run_int_nonper->init );
    CPPUNIT_ASSERT( t_run_int_nonper->fini );
    CPPUNIT_ASSERT( !t_task_nonper->isRunning() );
    t_task_nonper->run( 0 );
    CPPUNIT_ASSERT( t_task_nonper->start() );
    // now blocks on events :
    CPPUNIT_ASSERT( t_task_nonper->isRunning() );
    CPPUNIT_ASSERT( t_task_nonper->stop() );
    sleep(1);
    CPPUNIT_ASSERT( !t_task_nonper->isRunning() );

}

void TasksTest::testSelfRemove()
{
    t_task_np->run(t_self_remove);
    t_task_np->start();
    sleep(1);
    CPPUNIT_ASSERT( !t_task_np->isRunning() );
    CPPUNIT_ASSERT( t_self_remove->fini );
}

void TasksTest::testStartStop()
{
    testStart();
    testPause();
    testStop();
}

void TasksTest::testRunnableInterface()
{
    testAddRunnableInterface();
    testStart();
    testRunnableInterfaceInit();
    testPause();
    testRunnableInterfaceExecution();
    testStop();
    testRemoveRunnableInterface();
}

void TasksTest::testAllocation()
{
    testAddAllocate();
    testStart();
    testPause();
    testStop();
    testRemoveAllocate();
}

void TasksTest::testAddRunnableInterface()
{
    bool adding_np = t_task_np->run( t_run_int_np );
    CPPUNIT_ASSERT( adding_np );
    bool adding_p =  t_task_p->run( t_run_int_p );
    CPPUNIT_ASSERT( adding_p );
    bool adding_nrt = t_task_nrt->run( t_run_int_nrt );
    CPPUNIT_ASSERT( adding_nrt );
    bool adding_prio = t_task_prio->run( t_run_int_prio );
    CPPUNIT_ASSERT( adding_prio );
    bool adding_sim = t_task_sim->run( t_run_int_sim );
    CPPUNIT_ASSERT( adding_sim );
}

void TasksTest::testRemoveRunnableInterface()
{

    CPPUNIT_ASSERT( t_run_int_np->fini );
    CPPUNIT_ASSERT( t_run_int_p->fini );
    CPPUNIT_ASSERT( t_run_int_nrt->fini );
    CPPUNIT_ASSERT( t_run_int_prio->fini );
    CPPUNIT_ASSERT( t_run_int_sim->fini );

    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
    CPPUNIT_ASSERT( t_task_p->run( 0 ) );
    CPPUNIT_ASSERT( t_task_nrt->run( 0 ) );
    CPPUNIT_ASSERT( t_task_prio->run( 0 ) );
    CPPUNIT_ASSERT( t_task_sim->run( 0 ) );

}

void TasksTest::testStart()
{
    t_task_np->start();
    t_task_p->start();
    t_task_nrt->start();
    t_task_prio->start();
    SimulationThread::Instance()->start();
    t_task_sim->start();
    
    CPPUNIT_ASSERT( t_task_np->isRunning() );
    CPPUNIT_ASSERT( t_task_p->isRunning() );
    CPPUNIT_ASSERT( t_task_nrt->isRunning() );
    CPPUNIT_ASSERT( t_task_prio->isRunning() );
    CPPUNIT_ASSERT( t_task_sim->isRunning() );
}

void TasksTest::testPause()
{
    sleep(1);
}

void TasksTest::testRunnableInterfaceInit() {
    CPPUNIT_ASSERT( t_run_int_np->init );
    CPPUNIT_ASSERT( t_run_int_p->init );
    CPPUNIT_ASSERT( t_run_int_nrt->init );
    CPPUNIT_ASSERT( t_run_int_prio->init );
    CPPUNIT_ASSERT( t_run_int_sim->init );
}

void TasksTest::testRunnableInterfaceExecution() {

    CPPUNIT_ASSERT( t_run_int_np->stepped );
    CPPUNIT_ASSERT( t_run_int_p->stepped );
    CPPUNIT_ASSERT( t_run_int_nrt->stepped );
    CPPUNIT_ASSERT( t_run_int_prio->stepped );
    CPPUNIT_ASSERT( t_run_int_sim->stepped );
}

void TasksTest::testStop()
{
    t_task_np->stop();
    t_task_p->stop();
    t_task_nrt->stop();
    t_task_prio->stop();
    t_task_sim->stop();
    SimulationThread::Instance()->stop();
    
    CPPUNIT_ASSERT( !t_task_np->isRunning() );
    CPPUNIT_ASSERT( !t_task_p->isRunning() );
    CPPUNIT_ASSERT( !t_task_nrt->isRunning() );
    CPPUNIT_ASSERT( !t_task_prio->isRunning() );
    CPPUNIT_ASSERT( !t_task_sim->isRunning() );
}

void TasksTest::testAddAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( t_run_allocate ) );
}

void TasksTest::testRemoveAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
}
    

