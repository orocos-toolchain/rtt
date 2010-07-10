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
        class MethodC;
        class OffsetDataSource;
        class SendHandleC;
        class SignalBase;
        class SimpleConnID;
        struct GenerateDataSource;
        struct IntrusiveStorage;
        struct LocalConnID;
        struct StreamConnID;
        struct UnknownType;
        template <typename function>
        class UnaryDataSource;
        template<
            typename Signature 
        >
        class SubscriberBase;
        template<
        typename Signature,
        typename TSlotFunction>
        class Signal;
        template< class T>
        class LockedQueue;
        template< class T>
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
        struct MethodBinder;
        template<class F>
        struct ReturnBase;
        template<class FunctionT>
        class LocalMethodImpl;
        template<class FunctionT>
        struct GetSignature;
        template<class FunctionT>
        struct GetSignatureDS;
        template<class FunctionT>
        struct LocalMethod;
        template<class MethodT>
        class RemoteMethod;
        template<class MethodT>
        class RemoteMethodImpl;
        template<class R>
        struct DataSourceResultStorage;
        template<class Seq, class Data, class Enable>
        struct GetPointerWrap;
        template<class T, class Enable>
        struct DSWrap;
        template<class T>
        class AtomicQueue;
        template<class T>
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
        template<typename Signature,typename ObjT>
        class OperationRepositoryPartFusedDS;
        template<typename Signature>
        class OperationRepositoryPartFused;
        template<typename Signature, class Enable>
        struct FusedFunctorDataSource;
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
        class OffsetPartDataSource;
        template<typename T>
        class PartDataSource;
        template<typename T>
        class ReferenceDataSource;
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
