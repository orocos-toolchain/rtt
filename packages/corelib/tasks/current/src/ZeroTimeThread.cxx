/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  ZeroTimeThread.cpp 

                       ZeroTimeThread.cpp -  description
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

#include "corelib/ZeroTimeThread.hpp"
#include "corelib/TaskNonPreemptible.hpp"
#include "corelib/Time.hpp"

#include <pkgconf/corelib_tasks.h>


#ifdef OROSEM_CORELIB_TASKS_AUTOSTART
#include <os/StartStopManager.hpp>
namespace ORO_CoreLib
{
    namespace
    {
        int startZTTThread()
        {
            ZeroTimeThread::Instance()->start();
            return true;
        }

        void stopZTTThread()
        {
            ZeroTimeThread::Release();
        }

        ORO_OS::InitFunction ZTTInit( &startZTTThread );
        ORO_OS::CleanupFunction ZTTCleanup( &stopZTTThread );
    }
}
#endif

namespace ORO_CoreLib
{
    // The static class variables
    ZeroTimeThread* ZeroTimeThread::_instance;

    ZeroTimeThread* ZeroTimeThread::Instance()
    {
        if ( _instance == 0 )
        {
            _instance = new ZeroTimeThread();
        }

        return _instance;
    }

    bool ZeroTimeThread::Release()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
            return true;
        }

        return false;
    }


    ZeroTimeThread::ZeroTimeThread()
        : TaskExecution(ORONUM_CORELIB_TASKS_ZTT_PRIORITY,
                        ORODAT_CORELIB_TASKS_ZTT_NAME, 
                        ORONUM_CORELIB_TASKS_ZTT_PERIOD )
    {
        makeHardRealtime();
        //rt_std::cout << "ZeroTimeThread Created with 1ms periodicity\n";
    }

    ZeroTimeThread::~ZeroTimeThread()
    {
        //rt_std::cout << "ZTT DESTRUCTOR\n";
    }

    bool ZeroTimeThread::taskAdd( TaskNonPreemptible* t, const nsecs n )
    {
        return TaskExecution::taskAdd( t->handler(), n);
    }

    void ZeroTimeThread::taskRemove( TaskNonPreemptible* t )
    {
        TaskExecution::taskRemove( t->handler() );
    }


}
