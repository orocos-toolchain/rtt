/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  NonRealTimeThread.cxx 

                        NonRealTimeThread.cxx -  description
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

#include "corelib/NonRealTimeThread.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Time.hpp"
#include "corelib/Logger.hpp"

#include "corelib/TaskNonRealTime.hpp"
#include "pkgconf/corelib_tasks.h"

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

namespace ORO_CoreLib
{

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
        : TimerThread(ORONUM_CORELIB_TASKS_NRT_PRIORITY, ORODAT_CORELIB_TASKS_NRT_NAME, ORONUM_CORELIB_TASKS_NRT_PERIOD )
    {
        Logger::log() << Logger::Info << ORODAT_CORELIB_TASKS_NRT_NAME <<" created with "<< ORONUM_CORELIB_TASKS_NRT_PERIOD <<"s periodicity";
        Logger::log() << Logger::Info << " and priority " << ORONUM_CORELIB_TASKS_NRT_PRIORITY << Logger::endl;
    }

    NonRealTimeThread::~NonRealTimeThread()
    {
        Logger::log() << Logger::Debug << ORODAT_CORELIB_TASKS_NRT_NAME <<" destructor." << Logger::endl;
    }
}
