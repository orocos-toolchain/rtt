
#include "tasks_test.hpp"
#include <unistd.h>
#include <iostream>

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
    t_task_np = new TaskNonPreemptible(0.01);
    t_task_p = new TaskPreemptible(0.01);
    t_task_nrt = new TaskNonRealTime(0.01);

    t_run_int_np =   new TestRunnableInterface(true);
    t_run_int_p =    new TestRunnableInterface(true);
    t_run_int_nrt =  new TestRunnableInterface(true);

    t_run_allocate = new TestAllocate();
    t_self_remove  = new TestSelfRemove();
}


void 
TasksTest::tearDown()
{
    delete t_task_np;
    delete t_task_p;
    delete t_task_nrt;
    delete t_run_int_np;
    delete t_run_int_p;
    delete t_run_int_nrt;

    delete t_run_allocate;
    delete t_self_remove;
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
}

void TasksTest::testRemoveRunnableInterface()
{

    CPPUNIT_ASSERT( t_run_int_np->fini );
    CPPUNIT_ASSERT( t_run_int_p->fini );
    CPPUNIT_ASSERT( t_run_int_nrt->fini );

    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
    CPPUNIT_ASSERT( t_task_p->run( 0 ) );
    CPPUNIT_ASSERT( t_task_nrt->run( 0 ) );

}

void TasksTest::testStart()
{
    t_task_np->start();
    t_task_p->start();
    t_task_nrt->start();
    
    CPPUNIT_ASSERT( t_task_np->isRunning() );
    CPPUNIT_ASSERT( t_task_p->isRunning() );
    CPPUNIT_ASSERT( t_task_nrt->isRunning() );
}

void TasksTest::testPause()
{
    sleep(1);
}

void TasksTest::testRunnableInterfaceInit() {
    CPPUNIT_ASSERT( t_run_int_np->init );
    CPPUNIT_ASSERT( t_run_int_p->init );
    CPPUNIT_ASSERT( t_run_int_nrt->init );
}

void TasksTest::testRunnableInterfaceExecution() {

    CPPUNIT_ASSERT( t_run_int_np->stepped );
    CPPUNIT_ASSERT( t_run_int_p->stepped );
    CPPUNIT_ASSERT( t_run_int_nrt->stepped );
}

void TasksTest::testStop()
{
    t_task_np->stop();
    t_task_p->stop();
    t_task_nrt->stop();
    
    CPPUNIT_ASSERT( !t_task_np->isRunning() );
    CPPUNIT_ASSERT( !t_task_p->isRunning() );
    CPPUNIT_ASSERT( !t_task_nrt->isRunning() );
}

void TasksTest::testAddAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( t_run_allocate ) );
}

void TasksTest::testRemoveAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
}
    

