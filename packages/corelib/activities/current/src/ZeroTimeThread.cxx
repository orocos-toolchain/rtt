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

#include "rtt/ZeroTimeThread.hpp"
#include "rtt/NonPreemptibleActivity.hpp"
#include "rtt/Time.hpp"
#include "rtt/Logger.hpp"

#include <pkgconf/corelib_activities.h>
#include <rtt/os/threads.hpp>


#ifdef OROSEM_CORELIB_ACTIVITIES_AUTOSTART
#include <rtt/os/StartStopManager.hpp>
namespace RTT
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

        OS::InitFunction ZTTInit( &startZTTThread );
        OS::CleanupFunction ZTTCleanup( &stopZTTThread );
    }
}
#endif

namespace RTT
{
    // The static class variables
    TimerThreadPtr ZeroTimeThread::_instance;

    TimerThreadPtr ZeroTimeThread::Instance()
    {
        if ( !_instance )
        {
            _instance.reset( new ZeroTimeThread() );
        }

        return _instance;
    }

    bool ZeroTimeThread::Release()
    {
        _instance.reset();
        return true;
    }


    ZeroTimeThread::ZeroTimeThread()
        : TimerThread(OS::HighestPriority,
                        ORODAT_CORELIB_ACTIVITIES_ZTT_NAME, 
                        ORONUM_CORELIB_ACTIVITIES_ZTT_PERIOD )
    {
        makeHardRealtime();
        Logger::log() << Logger::Info << ORODAT_CORELIB_ACTIVITIES_ZTT_NAME <<" created with "<< ORONUM_CORELIB_ACTIVITIES_ZTT_PERIOD <<"s periodicity";
        Logger::log() << Logger::Info << " and priority " << this->getPriority() << Logger::endl;
    }

    ZeroTimeThread::~ZeroTimeThread()
    {
        Logger::log() << Logger::Debug << ORODAT_CORELIB_ACTIVITIES_ZTT_NAME <<" destructor." << Logger::endl;
    }

}
