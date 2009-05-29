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

#include <RTT.hpp>
#include <PeriodicActivity.hpp>
#include <SimulationActivity.hpp>
#include <NonPeriodicActivity.hpp>

using namespace RTT;
using namespace RTT::detail;

struct TestRunnableInterface;
struct TestSelfRemove;

class ActivitiesTest
{
public:
    PeriodicActivity*   t_task_prio;

    TestRunnableInterface*  t_run_int_prio;
    TestRunnableInterface*  t_run_int_fail;

    RunnableInterface* t_run_allocate;
    TestSelfRemove* t_self_remove;

    ActivitiesTest(){setUp();};
    ~ActivitiesTest(){tearDown();};

    void setUp();
    void tearDown();

    void testThreads();
    void testOverrun();

    void testStartStop();
    void testStart();
    void testPause();
    void testStop();

    void testSelfRemove();

    void testNonPeriodic();

    void testAllocation();
    void testRemoveAllocate();
    void testAddAllocate();

    void testFailInit();
    void testRunnableInterface();
    void testAddRunnableInterface();
    void testRunnableInterfaceInit();
    void testRunnableInterfaceExecution();
    void testRemoveRunnableInterface();

};

#endif
