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

#include <corelib/TaskNonPreemptible.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskSimulation.hpp>

using namespace ORO_CoreLib;

template<class T>
struct TestTask;


/**
 * Tests task-thread relationships.
 */
class TasksThreadTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TasksThreadTest );
    // Configuration Test :
    //CPPUNIT_TEST( testThreadConfig );

    CPPUNIT_TEST( testExceptionRecovery );
    
    // Allocation Test : [disabled, kills RTAI/LXRT]
    //CPPUNIT_TEST( testAddAllocate );

    CPPUNIT_TEST_SUITE_END();

    TestTask<TaskNonPreemptible>* t_task_np;
    TestTask<TaskNonPreemptible>* t_task_np_bad;
    TestTask<TaskPreemptible>*    t_task_p;
    TestTask<TaskSimulation>*     t_task_sim;

    RunnableInterface* t_run_allocate;
public:

    void setUp();
    void tearDown();

    void testExceptionRecovery();
    void testThreadConfig();

    void testAllocation();
    void testRemoveAllocate();
    void testAddAllocate();
};

#endif
