/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  ZeroTimeThread.cxx 

                        ZeroTimeThread.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
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
        return TaskExecution::taskAdd( t, n);
    }

    void ZeroTimeThread::taskRemove( TaskNonPreemptible* t )
    {
        TaskExecution::taskRemove( t );
    }


}
