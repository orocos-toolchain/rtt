/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  startstop.cxx 

                        startstop.cxx -  description
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
/**
 * @file startstop.cxx
 *
 * This file defines the __os_init and __os_exit functions
 * which call manually crafted initialisation functions.
 * The target calls these functions at startup time.
 */

#include <pkgconf/system.h>
#include <pkgconf/os.h>

#include <os/startstop.h>

    
// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
using ORO_CoreLib::Logger;
#endif
#ifdef OROPKG_CORELIB_TIMING
#include "corelib/TimeService.hpp"
using ORO_CoreLib::TimeService;
#endif
 
#ifdef HAVE_MANUAL_MAIN
#include "os/MainThread.hpp"

static ORO_OS::MainThread* mainT;
#endif

#include "os/StartStopManager.hpp"
static ORO_OS::StartStopManager* initM;

extern "C"
int __os_init(int argc, char** argv )
{
#ifdef HAVE_MANUAL_CRT
    DO_GLOBAL_CTORS();
#endif

#ifdef HAVE_MANUAL_MAIN
    // this must be called to init the os scheduler, which is used in
    // any ComponentActive. Call this only once EVER.
    mainT = ORO_OS::MainThread::Instance();

    mainT->start();

#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "MainThread started." << Logger::endl;
#endif

#endif

#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Starting StartStopManager." << Logger::endl;
#endif
    initM = ORO_OS::StartStopManager::Instance();
    return initM->start();
}

extern "C"
void __os_exit(void)
{
#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "Stopping StartStopManager." << Logger::endl;
#endif
    initM->stop();
    ORO_OS::StartStopManager::Release();

#if defined(HAVE_MANUAL_MAIN) && defined(OROPKG_CORELIB_REPORTING)
    // This should be the (one but) last message to be logged :
    Logger::log() << Logger::Debug << "Stopping MainThread." << Logger::endl;
#endif

    // Stop logging
#ifdef OROPKG_CORELIB_REPORTING
    Logger::Release();
#endif
    // Stop TimeService if present.
#ifdef OROPKG_CORELIB_TIMING
    TimeService::Release();
#endif
    // Stop Main Thread
#ifdef HAVE_MANUAL_MAIN
    ORO_OS::MainThread::Release();
#endif

#ifdef HAVE_MANUAL_CRT
    DO_GLOBAL_DTORS();
#endif
}
