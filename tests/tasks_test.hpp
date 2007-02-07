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

#include <RTT.hpp>
#include <PeriodicActivity.hpp>
#include <SimulationActivity.hpp>
#include <NonPeriodicActivity.hpp>

using namespace RTT;
using namespace RTT::detail;

struct TestRunnableInterface;
struct TestSelfRemove;

class ActivitiesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ActivitiesTest );
    // Periodic Thread Test :
    CPPUNIT_TEST( testThreads );
#if 0
    // Periodic Overrun Thread Test :
    // Does not work in GNU/Linux nor RAI
    //CPPUNIT_TEST( testOverrun );

    // Timer Test :
    CPPUNIT_TEST( testTimer );

    // Start - Stop Test :
    CPPUNIT_TEST( testStartStop );

    // Runnable Interface Test :
    CPPUNIT_TEST( testRunnableInterface );
    CPPUNIT_TEST( testFailInit );
    
    // Allocation Test : [disabled, kills RTAI/LXRT]
    CPPUNIT_TEST( testAddAllocate );

    // Self Removal Test :
    CPPUNIT_TEST( testSelfRemove );

    // non periodic Test :
    CPPUNIT_TEST( testNonPeriodic );
#endif
    CPPUNIT_TEST_SUITE_END();

    PeriodicActivity*   t_task_prio;

    TestRunnableInterface*  t_run_int_prio;
    TestRunnableInterface*  t_run_int_fail;

    RunnableInterface* t_run_allocate;
    TestSelfRemove* t_self_remove;

    TimerInterface* tti;
public:

    void setUp();
    void tearDown();

    void testThreads();
    void testOverrun();

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

    void testFailInit();
    void testRunnableInterface();
    void testAddRunnableInterface();
    void testRunnableInterfaceInit();
    void testRunnableInterfaceExecution();
    void testRemoveRunnableInterface();

};

#endif
