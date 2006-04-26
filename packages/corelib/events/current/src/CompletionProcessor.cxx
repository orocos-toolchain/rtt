/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  CompletionProcessor.cxx 

                        CompletionProcessor.cxx -  description
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

#include "corelib/CompletionProcessor.hpp"
#include "corelib/Logger.hpp"

#include <pkgconf/corelib_events.h>

#ifdef OROSEM_CORELIB_EVENTS_AUTOSTART
#include <os/StartStopManager.hpp>
namespace ORO_CoreLib
{
    namespace
    {
        int startCPPThread()
        {
            CompletionProcessor::Instance()->start();
            return true;
        }

        void stopCPPThread()
        {
            CompletionProcessor::Release();
        }

        ORO_OS::InitFunction CPPInit( &startCPPThread );
        ORO_OS::CleanupFunction CPPCleanup( &stopCPPThread );
    }
}
#endif



namespace ORO_CoreLib
{

    CompletionProcessor* CompletionProcessor::cp;

    CompletionProcessor* CompletionProcessor::Instance()
    {
        if ( cp == 0 )
            {
                cp = new CompletionProcessor();
            }
        return cp;
    }

    bool CompletionProcessor::Release()
    {
        if ( cp != 0 )
            {
                cp->stop();
                delete cp;
                cp = 0;
                return true;
            }
        return false;
    }

    CompletionProcessor::CompletionProcessor()
        : NonPeriodicActivity(  ORONUM_CORELIB_EVENTS_CP_PRIORITY, 
                            ORODAT_CORELIB_EVENTS_CP_NAME )
    {
        Logger::log() << Logger::Info << ORODAT_CORELIB_EVENTS_CP_NAME <<" created with priority ";
        Logger::log() << ORONUM_CORELIB_EVENTS_CP_PRIORITY <<Logger::endl;
    }

    CompletionProcessor::~CompletionProcessor()
    {}

}
