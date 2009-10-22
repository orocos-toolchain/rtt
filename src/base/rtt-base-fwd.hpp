/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  rtt-base-fwd.hpp

                        rtt-base-fwd.hpp -  description
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


#ifndef ORO_RTT_base_FWD_HPP
#define ORO_RTT_base_FWD_HPP

namespace RTT {
    namespace base {
        class ActionInterface;
        class ActivityInterface;
        class AttributeBase;
        class BufferBase;
        class ChannelElementBase;
        class ConditionInterface;
        class DataSourceBase;
        class DispatchInterface;
        class InputPortInterface;
        class OutputPortInterface;
        class PortInterface;
        class ProgramInterface;
        class PropertyBagVisitor;
        class PropertyBase;
        class PropertyIntrospection;
        class RunnableInterface;
        class TaskCore;
        struct BlockingPolicy;
        struct DataBuf;
        struct NonBlockingPolicy;
        template <class T>
        class DataObjectInterface;
        template<
            typename Signature
        >
        class EventBase;
        template< class T >
        class DataObject;
        template< class T, class ReadPolicy, class WritePolicy>
        class Buffer;
        template< class T, class ReadPolicy, class WritePolicy>
        class BufferLockFree;
        template<class F>
        struct CommandBase;
        template<class F>
        struct MethodBase;
        template<class T, class ReadPolicy, class WritePolicy>
        class BufferLocked;
        template<class T>
        class BufferInterface;
        template<class T>
        class DataObjectLockFree;
        template<class T>
        class DataObjectLocked;
        template<class T>
        class ReadInterface;
        template<class T>
        class WriteInterface;
        template<typename T>
        class ChannelElement;
    }
    namespace detail {
        using namespace base;
    }
}
#endif
