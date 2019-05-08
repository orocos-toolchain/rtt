/***************************************************************************
  tag: Peter Soetens Fri Aug 14 20:13:44 2009 +0200 rtt-internal-fwd.hpp

                        rtt-internal-fwd.hpp -  description
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


#ifndef ORO_RTT_internal_FWD_HPP
#define ORO_RTT_internal_FWD_HPP

namespace RTT {
    namespace internal {
        class ConnFactory;
        class ConnID;
        class ConnectionBase;
        class ConnectionManager;
        class DataSourceCommand;
        class GlobalEngine;
        class OffsetDataSource;
        class OperationCallerC;
        class OperationInterfacePartHelper;
        class SendHandleC;
        class SignalBase;
        class SimpleConnID;
        class PortConnectionLock;
        struct GenerateDataSource;
        struct IntrusiveStorage;
        struct LocalConnID;
        struct StreamConnID;
        struct SharedConnID;
        class SharedConnectionBase;
        template <class T>
        class SharedConnection;
        struct UnknownType;
        template <typename function>
        class UnaryDataSource;
        template<
        typename Signature,
        typename TSlotFunction>
        class Signal;
        template< class T>
        class List;
        template< class T>
        class ListLockFree;
        template< class T>
        class ListLocked;
        template< class T>
        class LockedQueue;
        template< class T>
        struct DataSourceTypeInfo;
        template<class A>
        struct DataSourceArgStorage;
        template<class DataType>
        struct DataSourceStorage;
        template<class F, class BaseImpl>
        struct Collect;
        template<class F, class BaseImpl>
        struct Invoker;
        template<class F, class BaseImpl>
        struct Return;
        template<class F,class Class>
        class AddMember;
        template<class F>
        class ArgMember;
        template<class F>
        class UnMember;
        template<class F>
        class UnPointer;
        template<class F>
        struct CollectBase;
        template<class F>
        struct CollectType;
        template<class F>
        struct InvokerBase;
        template<class F>
        struct OperationCallerBinder;
        template<class F>
        struct ReturnBase;
        template<class FunctionT>
        class LocalOperationCallerImpl;
        template<class FunctionT>
        struct GetSignature;
        template<class FunctionT>
        struct GetSignatureDS;
        template<class FunctionT>
        struct LocalOperationCaller;
        template<class OperationCallerT>
        class RemoteOperationCaller;
        template<class OperationCallerT>
        class RemoteOperationCallerImpl;
        template<class R>
        struct DataSourceResultStorage;
        template<class Seq, class Data, class Enable>
        struct AssignHelper;
        template<class Seq, class Data, class Enable>
        struct GetPointerWrap;
        template<class T, class Enable>
        struct DSWrap;
        template<class T>
        class AtomicQueue;
        template<class T>
        class AtomicMWMRQueue;
        template<class T>
        class AtomicMWSRQueue;
        template<class T>
        class MWSRQueue;
        template<class T>
        class Queue;
        template<class T>
        struct AStore;
        template<class T>
        struct DSRStore;
        template<class T>
        struct NA;
        template<class T>
        struct RStore;
        template<class T>
        struct UpdateHelper;
        template<class ToBind>
        struct BindStorage;
        template<typename BoundType>
        class UnboundDataSource;
        template<typename Signature, class Enable>
        struct FusedFunctorDataSource;
        template<typename Signature,typename ObjT>
        class OperationInterfacePartFusedDS;
        template<typename Signature>
        class OperationInterfacePartFused;
        template<typename Signature>
        class SynchronousOperationInterfacePartFused;
        template<typename Signature>
        struct FusedMCallDataSource;
        template<typename Signature>
        struct FusedMCollectDataSource;
        template<typename Signature>
        struct FusedMSendDataSource;
        template<typename Signature>
        struct FusedMSignal;
        template<typename T, typename S>
        class AssignCommand;
        template<typename T>
        class ActionAliasAssignableDataSource;
        template<typename T>
        class ActionAliasDataSource;
        template<typename T>
        class ArrayPartDataSource;
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
        class LateConstReferenceDataSource;
        template<typename T>
        class LateReferenceDataSource;
        template<typename T>
        class OffsetPartDataSource;
        template<typename T>
        class PartDataSource;
        template<typename T>
        class ReferenceDataSource;
        template<typename T>
        class TsPool;
        template<typename T>
        class ValueDataSource;
        template<typename function>
        class BinaryDataSource;
        template<typename function>
        class NArityDataSource;
    }
    namespace detail {
        using namespace internal;
    }
}
#endif
