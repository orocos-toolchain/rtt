/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  RealTimeTypekitGlobals.cpp

                        RealTimeTypekitGlobals.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include "rtt-typekit-config.h"
#include "RealTimeTypekit.hpp"
#include "../types/Types.hpp"
#include "../types/GlobalsRepository.hpp"
#include "../FlowStatus.hpp"
#include "../SendStatus.hpp"
#include "../ConnPolicy.hpp"
#include "../BufferPolicy.hpp"
#include "../typekit/Types.hpp"
#include "../os/fosi.h"

namespace RTT
{
    using namespace types;
    bool RealTimeTypekitPlugin::loadGlobals() {
        GlobalsRepository::shared_ptr globals = GlobalsRepository::Instance();

        // Data Flow enums:
        globals->setValue( new Constant<FlowStatus>("NoData", NoData) );
        globals->setValue( new Constant<FlowStatus>("OldData", OldData) );
        globals->setValue( new Constant<FlowStatus>("NewData", NewData) );
        globals->setValue( new Constant<WriteStatus>("WriteSuccess", WriteSuccess) );
        globals->setValue( new Constant<WriteStatus>("WriteFailure", WriteFailure) );
        globals->setValue( new Constant<WriteStatus>("NotConnected", NotConnected) );
        globals->setValue( new Constant<SendStatus>("SendFailure", SendFailure) );
        globals->setValue( new Constant<SendStatus>("SendNotReady", SendNotReady) );
        globals->setValue( new Constant<SendStatus>("SendSuccess", SendSuccess) );
#ifndef ORO_EMBEDDED
        globals->addAttribute( "DefaultConnPolicy", ConnPolicy::Default() );
        globals->setValue( new Constant<int>("DATA", ConnPolicy::DATA) );
        globals->setValue( new Constant<int>("BUFFER", ConnPolicy::BUFFER) );
        globals->setValue( new Constant<int>("CIRCULAR_BUFFER", ConnPolicy::CIRCULAR_BUFFER) );
        globals->setValue( new Constant<int>("LOCKED", ConnPolicy::LOCKED) );
        globals->setValue( new Constant<int>("LOCK_FREE", ConnPolicy::LOCK_FREE) );
        globals->setValue( new Constant<int>("UNSYNC", ConnPolicy::UNSYNC) );
        globals->setValue( new Constant<BufferPolicy>("UnspecifiedBufferPolicy", UnspecifiedBufferPolicy) );
        globals->setValue( new Constant<BufferPolicy>("PerConnection", PerConnection) );
        globals->setValue( new Constant<BufferPolicy>("PerInputPort", PerInputPort) );
        globals->setValue( new Constant<BufferPolicy>("PerOutputPort", PerOutputPort) );
        globals->setValue( new Constant<BufferPolicy>("Shared", Shared) );
        globals->setValue( new Constant<int>("ORO_SCHED_RT", ORO_SCHED_RT) );
        globals->setValue( new Constant<int>("ORO_SCHED_OTHER", ORO_SCHED_OTHER) );
        globals->setValue( new Constant<int>("ORO_WAIT_ABS", ORO_WAIT_ABS) );
        globals->setValue( new Constant<int>("ORO_WAIT_REL", ORO_WAIT_REL) );
#endif
        return true;
    }
}
