
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
      
    bool MainThread::makeHardRealtime()
    {
        rtos_task_make_hard_real_time(&main_task);
        return rtos_task_is_hard_real_time(&main_task);
    }

    bool MainThread::makeSoftRealtime()
    {
        rtos_task_make_soft_real_time(&main_task);
        return !rtos_task_is_hard_real_time(&main_task);
    }

    bool MainThread::isHardRealtime() const
    {
        return rtos_task_is_hard_real_time(&main_task);
    }

    int MainThread::getPriority() const
    {
        return rtos_task_get_priority(&main_task);
    }

    unsigned int MainThread::threadNumber() const { return 0; }
}}


