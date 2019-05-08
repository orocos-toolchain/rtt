/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  RealTimeTypekitTypes2.cpp

                        RealTimeTypekitTypes2.cpp -  description
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
#include "Types2.inc"
#include "RealTimeTypekit.hpp"
#include "../types/Types.hpp"
#include "../types/TypeInfoName.hpp"
#include "../types/TemplateTypeInfo.hpp"
#include "../types/SequenceTypeInfo.hpp"
#include "StdTypeInfo.hpp"
#include "../types/StructTypeInfo.hpp"

#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../ConnPolicy.hpp"
#include "ConnPolicyType.hpp"
#include "TaskContext.hpp"

namespace RTT
{
    using namespace std;
    using namespace detail;


     namespace types {
         /**
          * This is a placeholder for scripting purposes.
          */
         struct EmptySendHandle {};

         // load the Orocos specific types:
         void loadOrocosTypes( TypeInfoRepository::shared_ptr ti ) {
             ti->addType( new StdTypeInfo<FlowStatus>("FlowStatus"));
             ti->addType( new StdTypeInfo<WriteStatus>("WriteStatus"));
             ti->addType( new StdTypeInfo<SendStatus>("SendStatus"));
             ti->addType( new TemplateTypeInfo<PropertyBag, true>("PropertyBag") );
             ti->addType( new StructTypeInfo<ConnPolicy>("ConnPolicy") );
             ti->addType( new StdTypeInfo<BufferPolicy>("BufferPolicy") );
             ti->addType( new TemplateTypeInfo<EmptySendHandle>("SendHandle") ); //dummy, replaced by real stuff when seen by parser.
             ti->addType( new TemplateTypeInfo<TaskContext*>("TaskContext"));
         }
     }
}


