#ifndef TASKSTEST_H
#define TASKSTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include <corelib/TaskNonRealTime.hpp>
#include <corelib/TaskPreemptible.hpp>
#include <corelib/TaskNonPreemptible.hpp>

using namespace ORO_CoreLib;

struct TestRunnableInterface;
struct TestSelfRemove;

class TasksTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( TasksTest );
    // Start - Stop Test :
    CPPUNIT_TEST( testStartStop );

    // Runnable Interface Test :
    CPPUNIT_TEST( testRunnableInterface );
    
    // Allocation Test :
    CPPUNIT_TEST( testAddAllocate );

    // Self Removal Test :
    CPPUNIT_TEST( testSelfRemove );

    CPPUNIT_TEST_SUITE_END();

    TaskNonRealTime*    t_task_nrt;
    TaskNonPreemptible* t_task_np;
    TaskPreemptible*    t_task_p;

    TestRunnableInterface*  t_run_int_np;
    TestRunnableInterface*  t_run_int_p;
    TestRunnableInterface*  t_run_int_nrt;

    RunnableInterface* t_run_allocate;
    TestSelfRemove* t_self_remove;
public:

    void setUp();
    void tearDown();

    void testStartStop();
    void testStart();
    void testPause();
    void testStop();

    void testSelfRemove();

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
