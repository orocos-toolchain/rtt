/***************************************************************************
  tag: Peter Soetens  Fri Oct 23 11:02:38 CEST 2009  rtt-internal-fwd.hpp

                        rtt-internal-fwd.hpp -  description
                           -------------------
    begin                : Fri October 23 2009
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


#ifndef ORO_RTT_internal_FWD_HPP
#define ORO_RTT_internal_FWD_HPP

namespace RTT {
    namespace internal {
        class BlockingEventProcessor;
        class CommandC;
        class CommandFunction;
        class CommandProcessor;
        class CompletionProcessor;
        class ConditionFunction;
        class ConnFactory;
        class ConnID;
        class ConnectionBase;
        class ConnectionC;
        class ConnectionManager;
        class DispatchAction;
        class EventC;
        class MessageProcessor;
        class MethodC;
        class SignalBase;
        class SimpleConnID;
        struct EventCallBack;
        struct EventCatcher;
        struct EventHookBase;
        struct GenerateDataSource;
        struct IntrusiveStorage;
        struct LocalConnID;
        struct OnlyFirstCont;
        struct OnlyLastCont;
        struct StreamConnID;
        struct UnknownType;
        template <typename function>
        class UnaryDataSource;
        template<
            typename Signature
        >
        class LocalEvent;
        template<
            typename Signature
        >
        class SubscriberBase;
        template<
        typename Signature,
        typename SlotFunction>
        class Signal;
        template< class FunctionT>
        struct Functor;
        template< class T, class ReadPolicy, class WritePolicy>
        class AtomicQueue;
        template< class T, class ReadPolicy, class WritePolicy>
        class LockedQueue;
        template< class T, class ReadPolicy, class WritePolicy>
        class Queue;
        template< class T>
        class List;
        template< class T>
        class ListLockFree;
        template< class T>
        class ListLocked;
        template< class T>
        struct DataSourceTypeInfo;
        template< class TResult >
        struct AdaptAssignableDataSource;
        template< class TResult >
        struct AdaptDataSource;
        template<class A>
        struct DataSourceArgStorage;
        template<class CommandT,
                 class CommandF>
        class DataSourceArgsCommand;
        template<class CommandT>
        class LocalCommand;
        template<class CommandT>
        class LocalCommandImpl;
        template<class CommandT>
        class RemoteCommand;
        template<class CommandT>
        class RemoteCommandImpl;
        template<class DataType>
        struct DataSourceStorage;
        template<class DataType_>
        class SingleList;
        template<class DataType_>
        class SortedList;
        template<class EventT>
        class EventHookGenerator;
        template<class F, class BaseImpl>
        struct Invoker;
        template<class F>
        class ArgMember;
        template<class F>
        class UnMember;
        template<class F>
        class UnPointer;
        template<class F>
        struct InvokerBase;
        template<class F>
        struct MethodBinder;
        template<class From, class To>
        struct AssignableDataSourceAdaptor;
        template<class From, class To>
        struct DataSourceAdaptor;
        template<class FunctionT>
        class LocalMethodImpl;
        template<class FunctionT>
        struct LocalMethod;
        template<class MethodT>
        class RemoteMethod;
        template<class MethodT>
        class RemoteMethodImpl;
        template<class R>
        struct DataSourceResultStorage;
        template<class SignatureT, class FunctorT>
        class DataSourceArgsEvent;
        template<class SignatureT, class FunctorT>
        class DataSourceArgsMethod;
        template<class T, class Enable>
        struct DSWrap;
        template<class T>
        class CopyPropertyCommand;
        template<class T>
        class FixedSizeMemoryPool;
        template<class T>
        class MemoryPool;
        template<class T>
        class RefreshPropertyCommand;
        template<class T>
        class RefreshPropertyFromDSCommand;
        template<class T>
        class UpdatePropertyCommand;
        template<class T>
        struct AStore;
        template<class T>
        struct NA;
        template<class ToBind>
        struct BindStorage;
        template<typename BoundType>
        class UnboundDataSource;
        template<typename ComponentT, typename FunctorT, typename Arg1T>
        class FunctorDataSourceDS1;
        template<typename ComponentT, typename FunctorT>
        class FunctorDataSourceDS0;
        template<typename EventT>
        struct EventHook0;
        template<typename EventT>
        struct EventHook1;
        template<typename EventT>
        struct EventHook2;
        template<typename EventT>
        struct EventHook3;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T, typename Arg6T>
        class FunctorDataSource6;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
        class FunctorDataSource5;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
        class FunctorDataSource4;
        template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
        class FunctorDataSource3;
        template<typename FunctorT, typename Arg1T, typename Arg2T>
        struct FunctorDataSource2;
        template<typename FunctorT, typename Arg1T>
        struct FunctorDataSource1;
        template<typename FunctorT>
        struct FunctorDataSource0;
        template<typename R, typename FunctorT>
        struct FunctionForwarder;
        template<typename ResultT, typename FunctorT, typename arg1_type>
        class FunctorFactoryPart1;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart0;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart2;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart3;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart4;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart5;
        template<typename ResultT, typename FunctorT>
        class FunctorFactoryPart6;
        template<typename ResultT>
        class FunctorFactoryPart;
        template<typename SignatureT, typename FunctorT>
        class CommandFunctor;
        template<typename SignatureT, typename FunctorT>
        class ConditionFunctor;
        template<typename T, typename APred, typename S>
        class AssignContainerCommand;
        template<typename T, typename Index, typename SetType, typename IPred, typename APred>
        class IndexedValueDataSource;
        template<typename T, typename Index, typename SetType, typename Pred>
        class AssignIndexCommand;
        template<typename T, typename S>
        class AssignCommand;
        template<typename T>
        class AssignableDataSource;
        template<typename T>
        class ChannelBufferElement;
        template<typename T>
        class ChannelDataElement;
        template<typename T>
        class ConnInputEndpoint;
        template<typename T>
        class ConnOutputEndpoint;
        template<typename T>
        class ConstReferenceDataSource;
        template<typename T>
        class ConstantDataSource;
        template<typename T>
        class DataSource;
        template<typename T>
        class InputPortSource;
        template<typename T>
        class ReferenceDataSource;
        template<typename T>
        class ValueDataSource;
        template<typename function>
        class BinaryDataSource;
        template<typename function>
        class NArityDataSource;
        template<typename function>
        class SixaryDataSource;
        template<typename function>
        class TernaryDataSource;
    }
    namespace detail {
        using namespace internal;
    }
}
#endif
