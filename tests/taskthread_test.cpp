 

#include "taskthread_test.hpp"
#include <unistd.h>
#include <iostream>

#include <corelib/ZeroTimeThread.hpp>
#include <corelib/PriorityThread.hpp>
#include <corelib/ZeroLatencyThread.hpp>
#include <corelib/SimulationThread.hpp>
#include <corelib/Logger.hpp>

using namespace std;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TasksThreadTest );

using namespace ORO_CoreLib;

struct A {};

template<class T>
struct TestTask
    : public T
{
    bool result, _dothrow;
    bool init, stepped, fini;

    TaskInterface* owner;

    TestTask(double p, bool fail, bool dothrow = false)
        :T(p), _dothrow(dothrow)
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
TasksThreadTest::setUp()
{
    t_task_np = new TestTask<TaskNonPreemptible>( ZeroTimeThread::Instance()->getPeriod(), true );
    t_task_np_bad = new TestTask<TaskNonPreemptible>( ZeroTimeThread::Instance()->getPeriod(), true, true );
    t_task_p = new TestTask<TaskPreemptible>( ZeroLatencyThread::Instance()->getPeriod(), true );
    t_task_sim = new TestTask<TaskSimulation>( SimulationThread::Instance()->getPeriod(), true );
}


void 
TasksThreadTest::tearDown()
{
    ZeroTimeThread::Instance()->start();
    delete t_task_np;
    delete t_task_np_bad;
    delete t_task_p;
    delete t_task_sim;
}

void TasksThreadTest::testThreadConfig()
{
    PriorityThread<15>::Instance();
    // stop thread
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->stop() );
    // switching hard/soft
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeHardRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeHardRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeSoftRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeHardRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeSoftRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeSoftRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeHardRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->start() );

    sleep(1);

    // all must fail, except first statement
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeHardRealtime() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->makeSoftRealtime() == false );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->setPeriod(0.3) == false );
    
    // reconfigure periodicity
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->stop() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->setPeriod(0.3) );
    CPPUNIT_ASSERT_EQUAL( 0.3, PriorityThread<15>::Instance()->getPeriod() );

    // some quick start/stops.
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->start() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->stop() );
    CPPUNIT_ASSERT( PriorityThread<15>::Instance()->start() );

    PriorityThread<15>::Release();
}

void TasksThreadTest::testExceptionRecovery()
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

void TasksThreadTest::testAddAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( t_run_allocate ) );
}

void TasksThreadTest::testRemoveAllocate()
{
    CPPUNIT_ASSERT( t_task_np->run( 0 ) );
}
    

