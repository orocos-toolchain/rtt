/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  rtt-fwd.hpp

                        rtt-fwd.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef ORO_RTT_FWD_HPP
#define ORO_RTT_FWD_HPP

#include "rtt-detail-fwd.hpp"
#include "os/rtt-os-fwd.hpp"
#include "base/rtt-base-fwd.hpp"
#include "internal/rtt-internal-fwd.hpp"
#include "plugin/rtt-plugin-fwd.hpp"
#include "types/rtt-types-fwd.hpp"
#include <boost/shared_ptr.hpp>


namespace RTT
{

    class Activity;
    class Alias;
    class CleanupHandle;
    class ConnPolicy;
    class ExecutionEngine;
    class Handle;
    class Logger;
    class PropertyBag;
    class ScopedHandle;
    class TaskContext;
    template<typename T>
    class Attribute;
    template<typename T>
    class Constant;
    template<typename T>
    class InputPort;
    template<typename FunctionT>
    class OperationCaller;
    template<class Signature>
    class Operation;
    template<typename T>
    class OutputPort;
    template<typename T>
    class Property;
    template<typename T>
    class SendHandle;
    struct ArgumentDescription;
    class ConfigurationInterface;
    class DataFlowInterface;
    class OperationInterface;
    class OperationInterfacePart;
    class Service;
    class ServiceRequester;
    typedef boost::shared_ptr<Service> ServicePtr;
}
#endif
