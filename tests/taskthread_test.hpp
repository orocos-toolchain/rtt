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

#include <cppunit/extensions/HelperMacros.h>

#include <rtt/NonPreemptibleActivity.hpp>
#include <rtt/PreemptibleActivity.hpp>
#include <rtt/SimulationActivity.hpp>
#include <rtt/SlaveActivity.hpp>

using namespace RTT;

template<class T>
struct TestTask;


/**
 * Tests task-thread relationships.
 */
class ActivitiesThreadTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ActivitiesThreadTest );
    // Configuration Test :
    CPPUNIT_TEST( testThreadConfig );
    CPPUNIT_TEST( testPeriodic );
    CPPUNIT_TEST( testSlave );

#ifndef ORO_EMBEDDED
    CPPUNIT_TEST( testExceptionRecovery );
#endif
    
    // Allocation Test : [disabled, kills RTAI/LXRT]
    //CPPUNIT_TEST( testAddAllocate );

    CPPUNIT_TEST_SUITE_END();

    TestTask<NonPreemptibleActivity>* t_task_np;
    TestTask<NonPreemptibleActivity>* t_task_np_bad;
    TestTask<PreemptibleActivity>*    t_task_p;
    TestTask<SimulationActivity>*     t_task_sim;
    TestTask<SlaveActivity>*          t_task_slave;

    RunnableInterface* t_run_allocate;
public:

    void setUp();
    void tearDown();

#ifndef ORO_EMBEDDED
    void testExceptionRecovery();
#endif
    void testThreadConfig();
    void testPeriodic();
    void testSlave();

    void testAllocation();
    void testRemoveAllocate();
    void testAddAllocate();
};

#endif
