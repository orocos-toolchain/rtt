
#include "os/MainThread.hpp"
#include <linux/module.h>
#include <sched.h>
#include <sys/mman.h>
#include <iostream>
#include "fosi.h"

namespace ORO_OS
{
    SchedPolicy::SchedPolicy()
    {
        std::cout <<"Sched Policy Init"<<std::endl;
        /* check to see if rtai_lxrt module is loaded */
        struct module_info modInfo;
        size_t retSize;
        if ( query_module("rtai_lxrt", QM_INFO, &modInfo, 
                          sizeof(modInfo), &retSize) != 0 ) {
            std::cerr <<"It appears the rtai_lxrt module is not loaded !"<<std::endl;
            exit();
        }
        struct sched_param param;
        param.sched_priority = 99;
        sched_setscheduler( SELECT_THIS_PID, OROSEM_OS_LXRT_SCHEDTYPE, &param);
        //init_linux_scheduler( OROSEM_OS_LXRT_SCHEDTYPE, 99);
        unsigned long name = nam2num("MAINTHREAD");
        if( !(rt_task = rt_task_init(name, 10,0,0)) ) // priority, stack, msg_size
            {
                printf("Fatal : Cannot init MainThread!");
                exit(1);
            }
                
        //rt_set_periodic_mode();
        //start_rt_timer( nano2count( usecs_to_nsecs(100) ) ); // 0.1ms = 100us

        // XXX Debugging, this works
        // BE SURE TO SET rt_preempt_always(1) when using one shot mode
        rt_set_oneshot_mode();
        //rt_preempt_always(1);
        start_rt_timer(0);
    }

    SchedPolicy::~SchedPolicy()
    {
        // we don't stop the timer
        //stop_rt_timer();
        rt_task_delete(rt_task);
    }

    MemPolicy::MemPolicy(int hp, int stk)
        : alloc_heap(hp), alloc_stack(hp)
    {
        std::cout <<"Memory Policy Init"<<std::endl;
        // stack, heap : see touchall.c in lxrt/lib
        // lock_all(stk,hp);
        mlockall(MCL_CURRENT | MCL_FUTURE );
    }

    MainThread* MainThread::mt;
}
