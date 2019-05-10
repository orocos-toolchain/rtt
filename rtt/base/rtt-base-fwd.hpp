/***************************************************************************
  tag: Peter Soetens Fri Aug 14 20:13:44 2009 +0200 rtt-base-fwd.hpp

                        rtt-base-fwd.hpp -  description
                           -------------------
    begin                : Fri Aug 14 2009
    copyright            : (C) 2009 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_RTT_base_FWD_HPP
#define ORO_RTT_base_FWD_HPP

namespace RTT {
    namespace base {
        class ActionInterface;
        class ActivityInterface;
        class AttributeBase;
        class BufferBase;
        class ChannelElementBase;
        class MultipleInputsChannelElementBase;
        class MultipleOutputsChannelElementBase;
        class MultipleInputsMultipleOutputsChannelElementBase;
        class DataSourceBase;
        class DisposableInterface;
        class ExecutableInterface;
        class InputPortInterface;
        class OperationBase;
        class OutputPortInterface;
        class PortInterface;
        class PropertyBagVisitor;
        class PropertyBase;
        class PropertyIntrospection;
        class RunnableInterface;
        class TaskCore;
        struct DataBuf;
        struct OperationCallerBaseInvoker;
        template <class T>
        class DataObjectInterface;
        template< class T >
        class DataObject;
        template< class T>
        class Buffer;
        template< class T>
        class BufferLockFree;
        template<class F>
        struct OperationCallerBase;
        template<class T>
        class BufferInterface;
        template<class T>
        class BufferLocked;
        template<class T>
        class BufferUnSync;
        template<class T>
        class DataObjectLockFree;
        template<class T>
        class DataObjectLocked;
        template<class T>
        class DataObjectUnSync;
        template<typename T>
        class ChannelElement;
        template<typename T>
        class MultipleInputsChannelElement;
        template<typename T>
        class MultipleOutputsChannelElement;
        template<typename T>
        class MultipleInputsMultipleOutputsChannelElement;
    }
    namespace detail {
        using namespace base;
    }
}
#endif
