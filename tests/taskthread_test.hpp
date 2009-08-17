/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:26 CEST 2005  taskthread_test.hpp

                        taskthread_test.hpp -  description
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



#ifndef TASKSTHREADTEST_H
#define TASKSTHREADTEST_H

#include <extras/PeriodicActivity.hpp>
#include <extras/SimulationActivity.hpp>
#include <extras/SlaveActivity.hpp>
#include <Activity.hpp>
#include <rtt-detail-fwd.hpp>
using namespace RTT::detail;

using namespace RTT;

template<class T>
struct TestActivity;


/**
 * Tests task-thread relationships.
 */
class ActivitiesThreadTest
{
public:

    TestActivity<PeriodicActivity>* t_task_np;
    TestActivity<PeriodicActivity>* t_task_np_bad;
    TestActivity<PeriodicActivity>* t_task_p;
    TestActivity<SlaveActivity>*          t_task_slave;
    TestActivity<Activity>*         t_task_a;

    ActivitiesThreadTest(){ setUp(); };
    ~ActivitiesThreadTest(){ tearDown(); };
    void setUp();
    void tearDown();

#ifndef ORO_EMBEDDED
    void testExceptionRecovery();
#endif
    void testThreadConfig();
    void testPeriodic();
    void testNonPeriodic();
    void testSlave();
    void testSequential();
    void testScheduler();
    void testAllocation();
};

#endif
