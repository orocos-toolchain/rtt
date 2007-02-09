/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  startstop.cxx 

                        startstop.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
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

#include <rtt-config.h>
#include <os/startstop.h>
#include "os/MainThread.hpp"
#include "os/StartStopManager.hpp"

    
#include "Logger.hpp"
#include "TimeService.hpp"

using namespace RTT;
static OS::StartStopManager* initM;

extern "C"
int __os_init(int argc, char** argv )
{
#ifdef OS_HAVE_MANUAL_CRT
    DO_GLOBAL_CTORS();
#endif

    OS::MainThread::Instance();
    Logger::log() << Logger::Debug << "MainThread started." << Logger::endl;

    Logger::log() << Logger::Debug << "Starting StartStopManager." << Logger::endl;
    initM = OS::StartStopManager::Instance();
    return initM->start();
}

extern "C"
void __os_exit(void)
{
    Logger::log() << Logger::Debug << "Stopping StartStopManager." << Logger::endl;
    initM->stop();
    OS::StartStopManager::Release();

    // This should be the (one but) last message to be logged :
    Logger::log() << Logger::Debug << "Stopping MainThread." << Logger::endl;

    // Stop logging
    Logger::Release();

    // Stop TimeService if present.
    TimeService::Release();

    // Stop Main Thread
    OS::MainThread::Release();

#ifdef OS_HAVE_MANUAL_CRT
    DO_GLOBAL_DTORS();
#endif
}
