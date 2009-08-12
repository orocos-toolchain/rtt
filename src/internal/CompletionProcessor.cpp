/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  CompletionProcessor.cxx

                        CompletionProcessor.cxx -  description
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

#include "CompletionProcessor.hpp"
#include "../Logger.hpp"
#include "../os/threads.hpp"

#include "../os/StartStopManager.hpp"
namespace RTT
{
   namespace {
       // this function is required to cleanup the CP
       // resources before the OS is being 'cleaned up'.
       void stopCPPThread()
       {
           CompletionProcessor::Release();
       }
       OS::CleanupFunction CPPCleanup( &stopCPPThread );
   }
}

namespace RTT
{

    boost::shared_ptr<CompletionProcessor> CompletionProcessor::cp;

    CompletionProcessor* CompletionProcessor::Instance()
    {
        if ( !cp )
            {
                cp.reset( new CompletionProcessor() );
                cp->start();
            }
        return cp.get();
    }

    bool CompletionProcessor::Release()
    {
        if ( cp )
            {
                cp->stop();
                cp.reset();
                return true;
            }
        return false;
    }

    CompletionProcessor::CompletionProcessor()
        : Activity( OS::LowestPriority + 2* OS::IncreasePriority, 0, 
                    this, "CompletionProcessor" ) // execute 'BlockingEventProcessor' base class.
    {
        Logger::log() << Logger::Info << "CompletionProcessor created with priority ";
        Logger::log() << this->thread()->getPriority() <<Logger::endl;
    }

    CompletionProcessor::~CompletionProcessor()
    {}

}
