
// this file must be included always first. 
#include "rtt/os/fosi_internal.hpp"
#include "rtt/os/MainThread.hpp"

namespace RTT
{ namespace OS {
    
    using namespace detail;

    boost::shared_ptr<ThreadInterface> MainThread::mt;


    MainThread::MainThread() {
        rtos_task_create_main(&main_task);
    }

    MainThread::~MainThread() {
        rtos_task_delete_main(&main_task);
    }
    
    ThreadInterface* MainThread::Instance()
    {
        if (mt)
            return mt.get();
        mt.reset( new MainThread() );
        return mt.get();
    }

    void MainThread::Release()
    {
        mt.reset();
    }
        
    bool MainThread::run( OS::RunnableInterface* ) { return false; }

    bool MainThread::start()  { return false; }

    bool MainThread::stop()  { return false; }

    ThreadInterface::Seconds MainThread::getPeriod() const { return 0; }

    ThreadInterface::nsecs MainThread::getPeriodNS() const { return 0; }

    bool MainThread::isRunning() const { return true; }

    const char* MainThread::getName() const { return "main"; }

    RTOS_TASK * MainThread::getTask() { return &main_task; }
      
    bool MainThread::setScheduler(int sched_type)
    {
        if ( rtos_task_set_scheduler(&main_task, sched_type) == 0)
            return true;
        return false;
    }
    int MainThread::getScheduler() const
    {
        return rtos_task_get_scheduler(&main_task);
    }

    bool MainThread::setPriority(int p) 
    {
        return rtos_task_set_priority(&main_task, p) == 0;
    }

    int MainThread::getPriority() const
    {
        return rtos_task_get_priority(&main_task);
    }

    unsigned int MainThread::threadNumber() const { return 0; }
}}


