
#include "corelib/NonRealTimeThread.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Time.hpp"

#include "corelib/TaskNonRealTime.hpp"

#ifdef OROSEM_CORELIB_TASKS_INTEGRATE_COMPLETION
#include <corelib/CompletionProcessor.hpp>
#else

#ifdef OROSEM_CORELIB_TASKS_AUTOSTART
#include <os/StartStopManager.hpp>
namespace ORO_CoreLib
{
    namespace
    {
        int startNRTThread()
        {
            NonRealTimeThread::Instance()->start();
            return true;
        }

        void stopNRTThread()
        {
            NonRealTimeThread::Release();
        }

        ORO_OS::InitFunction NRTInit( &startNRTThread );
        ORO_OS::CleanupFunction NRTCleanup( &stopNRTThread );
    }
}
#endif
#endif

namespace ORO_CoreLib
{

#ifndef OROSEM_CORELIB_TASKS_INTEGRATE_COMPLETION
    NonRealTimeThread* NonRealTimeThread::cp;

    NonRealTimeThread* NonRealTimeThread::Instance()
    {
        if ( cp == 0 )
            {
                cp = new NonRealTimeThread();
            }
        return cp;
    }

    bool NonRealTimeThread::Release()
    {
        if ( cp != 0 )
            {
                delete cp;
                cp = 0;
                return true;
            }
        return false;
    }

    NonRealTimeThread::NonRealTimeThread()
        : TaskExecution(ORONUM_CORELIB_TASKS_NRT_PRIORITY, ORODAT_CORELIB_TASKS_NRT_NAME, ORONUM_CORELIB_TASKS_NRT_PERIOD )
    {
    }
#else
    NonRealTimeThread* NonRealTimeThread::Instance()
    {
        return CompletionProcessor::Instance();
    }

    bool NonRealTimeThread::Release()
    {
        return CompletionProcessor::Release();
    }

    NonRealTimeThread::NonRealTimeThread(int priority, const std::string& name, double periodicity)
        : TaskExecution( priority, name, periodicity )
    {
    }
#endif

    bool NonRealTimeThread::taskAdd( TaskNonRealTime* t, const nsecs n )
    {
        return TaskExecution::taskAdd( t->handler(), n);
    }

    void NonRealTimeThread::taskRemove( TaskNonRealTime* t )
    {
        TaskExecution::taskRemove( t->handler() );
    }

    void NonRealTimeThread::step()
    {
        // Execute the TaskNonRealTime instances.
        TaskExecution::step();
    }
}
