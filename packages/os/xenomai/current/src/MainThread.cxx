
#define OROBLD_OS_INTERNAL
#include "os/MainThread.hpp"
#include "os/fosi.h"
#include <iostream>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
using ORO_CoreLib::Logger;
#endif

namespace ORO_OS
{
  SchedPolicy::SchedPolicy()
  {

      if ( getuid() != 0 ) {
          std::cerr << "You are not root. This program requires that you are root." << std::endl;
          exit(1);
      }

        struct sched_param param;
        // we set the MT to the highest sched priority to allow the console
        // to interrupt a loose running thread.
        param.sched_priority = sched_get_priority_max(OROSEM_OS_SCHEDTYPE);
        if (param.sched_priority != -1 )
            sched_setscheduler( SELECT_THIS_PID, OROSEM_OS_SCHEDTYPE, &param);

        if ( rt_task_shadow( rt_task, 0, 10, 0) != 0 ) {
                std::cerr << "Cannot rt_task_shadow() MainThread." << std::endl;
                exit(1);
        }

#ifdef OROSEM_OS_XENO_PERIODIC
        // time in nanoseconds
        rt_timer_start( ORODAT_OS_XENO_PERIODIC_TICK*1000*1000*1000 );
#ifdef OROPKG_CORELIB_REPORTING
        Logger::In in("Scheduler");
        Logger::log() << Logger::Info << "Xenomai Periodic Timer ticks at "<<ORODAT_OS_XENO_PERIODIC_TICK<<" seconds." << Logger::endl;
#endif
#else
        rt_timer_start( TM_ONESHOT );
#ifdef OROPKG_CORELIB_REPORTING
        Logger::log() << Logger::Info << "Xenomai Periodic Timer runs in preemptive 'one-shot' mode." << Logger::endl;
#endif
#endif
#ifdef OROPKG_CORELIB_REPORTING
      Logger::log() << Logger::Debug << "Xenomai Timer and Main Task Created" << Logger::endl;
#endif
    }

    SchedPolicy::~SchedPolicy()
    {
        rt_task_delete(rt_task);
    }

    MainThread* MainThread::mt;
}
