/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:18 CET 2005  tasks_test.hpp 

                        tasks_test.hpp -  description
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
 
 
#ifndef TASKSTEST_H
#define TASKSTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/TaskNonRealTime.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskNonPreemptible.hpp>
#include <corelib/PriorityTask.hpp>
#include <corelib/TaskSimulation.hpp>
#include <corelib/TaskNonPeriodic.hpp>

using namespace ORO_CoreLib;

struct TestRunnableInterface;
struct TestSelfRemove;

class TasksTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TasksTest );
    // Self Removal Test :
    CPPUNIT_TEST( testTimer );

    // Start - Stop Test :
    CPPUNIT_TEST( testStartStop );

    // Runnable Interface Test :
    CPPUNIT_TEST( testRunnableInterface );
    
    // Allocation Test : [disabled, kills RTAI/LXRT]
    //CPPUNIT_TEST( testAddAllocate );

    // Self Removal Test :
    CPPUNIT_TEST( testSelfRemove );

    // non periodic Test :
    CPPUNIT_TEST( testNonPeriodic );

    CPPUNIT_TEST_SUITE_END();

    TaskNonRealTime*    t_task_nrt;
    TaskNonPreemptible* t_task_np;
    TaskPreemptible*    t_task_p;
    TaskSimulation*     t_task_sim;
    PriorityTask<15>*   t_task_prio;
    TaskNonPeriodic*    t_task_nonper;

    TestRunnableInterface*  t_run_int_np;
    TestRunnableInterface*  t_run_int_p;
    TestRunnableInterface*  t_run_int_nrt;
    TestRunnableInterface*  t_run_int_sim;
    TestRunnableInterface*  t_run_int_prio;
    TestRunnableInterface*  t_run_int_nonper;

    RunnableInterface* t_run_allocate;
    TestSelfRemove* t_self_remove;

    TaskTimerInterface* tti;
public:

    void setUp();
    void tearDown();

    void testStartStop();
    void testStart();
    void testPause();
    void testStop();

    void testSelfRemove();

    void testTimer();

    void testNonPeriodic();

    void testAllocation();
    void testRemoveAllocate();
    void testAddAllocate();
    
    void testRunnableInterface();
    void testAddRunnableInterface();
    void testRunnableInterfaceInit();
    void testRunnableInterfaceExecution();
    void testRemoveRunnableInterface();

};

#endif
