/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  ZeroLatencyThread.cpp 

                       ZeroLatencyThread.cpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "corelib/ZeroLatencyThread.hpp"
#include "corelib/TaskPreemptible.hpp"

#include "pkgconf/corelib_tasks.h"

#ifdef OROSEM_CORELIB_TASKS_AUTOSTART
#include <os/StartStopManager.hpp>
namespace ORO_CoreLib
{
    namespace
    {
        int startZLTThread()
        {
            ZeroLatencyThread::Instance()->start();
            return true;
        }

        void stopZLTThread()
        {
            ZeroLatencyThread::Release();
        }

        ORO_OS::InitFunction ZLTInit( &startZLTThread );
        ORO_OS::CleanupFunction ZLTCleanup( &stopZLTThread );
    }
}
#endif


namespace ORO_CoreLib
{
    
    // The static class variables
    ZeroLatencyThread* ZeroLatencyThread::_instance;

    ZeroLatencyThread* ZeroLatencyThread::Instance()
    {
        if ( _instance == 0 )
        {
            _instance = new ZeroLatencyThread();
        }

        return _instance;
    }

    bool ZeroLatencyThread::Release()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
            return true;
        }

        return false;
    }


    ZeroLatencyThread::ZeroLatencyThread()
        : TaskExecution(ORONUM_CORELIB_TASKS_ZLT_PRIORITY,
                        ORODAT_CORELIB_TASKS_ZLT_NAME, 
                        ORONUM_CORELIB_TASKS_ZLT_PERIOD)
    {
        makeHardRealtime();
        //rt_std::cout << "ZeroLatencyThread Created with 5ms periodicity\n";
    }

    ZeroLatencyThread::~ZeroLatencyThread()
    {
        //rt_std::cout << "ZLT DESTRUCTOR\n";
    }

    bool ZeroLatencyThread::taskAdd( TaskPreemptible* t, const nsecs n )
    {
        return TaskExecution::taskAdd( t->handler(), n );
    }

    void ZeroLatencyThread::taskRemove( TaskPreemptible* t )
    {
        TaskExecution::taskRemove( t->handler() );
    }


}
